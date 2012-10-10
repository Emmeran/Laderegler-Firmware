#include "PinConfig.h"

bool headerValid = false;
SerialHeader sHead;

Time sLastPong;

// maximum size of a packet in the serial protocol, defined mainly by the Serial buffer size
#define MAX_SERIAL_PACKET_LENGTH 64 // <= SERIAL_BUFFER_SIZE!
#define MAX_SERIAL_BODY_LENGTH MAX_SERIAL_PACKET_LENGTH

void serialSetup()
{
	Serial.begin(115200);
}

void sendHeader(SerialCommand cmd, u8 len)
{
    SerialHeader h;
    h.s.magicNumber = SERIAL_MAGIC_NUMBER;
    h.s.commandByte = cmd;
    h.s.packetLength = len;
    
    Serial.write(h.data, 3);
}

void sendBinaryStatus()
{    
    char buf[MAX_SERIAL_BODY_LENGTH];
    int bytes;
    
    sendHeader(CMDS_BSTATUS, LADEREGLER_BSTATUS_LENGTH);
    
    u32 now = millis();
    buf[0] = deviceState;
    buf[1] = mosfetStates();
    memcpy(buf +  2, &now, 4); 
    memcpy(buf +  6, &vbat, 2); 
    memcpy(buf +  8, &vgen, 2); 
    memcpy(buf + 10, &tempVals[0].s.celcius10, 2); 
    memcpy(buf + 12, &stroeme, 12);
    Serial.write((u8*)buf, LADEREGLER_BSTATUS_LENGTH);
    
    delay(2); // wait a bit to let the other device catch up (23 * 9 bit with 115200 baud/s takes 1.8ms
}

void sendStatus()
{
    char buf[MAX_SERIAL_BODY_LENGTH];
    int bytes;
    
    bytes = snprintf(buf, sizeof(buf), 
	"Status: m(%lu) s(%d) bat(%lu V/100) gen(%lu V/100)",
	millis(), deviceState,
	vbat, vgen);
    
    sendHeader(CMDS_STATUS, bytes);
    Serial.write((u8*)buf, bytes);
	
    if (bytes > MAX_SERIAL_PACKET_LENGTH)
	bytes = MAX_SERIAL_PACKET_LENGTH;
    
    delay(5); // wait a bit to let the other device catch up (64 * 9 bit with 115200 baud/s takes 5ms)
    
    bytes = snprintf(buf, sizeof(buf), 
	"..: T(%d C/10) I(%d,%d,%d,%d,%d,%d)",
	tempVals[0].s.celcius10,
	stroeme[0],
	stroeme[1],
	stroeme[2],
	stroeme[3],
	stroeme[4],
	stroeme[5]);
	
    if (bytes > MAX_SERIAL_PACKET_LENGTH)
	bytes = MAX_SERIAL_PACKET_LENGTH;
    
    sendHeader(CMDS_STATUS, bytes);
    Serial.write((u8*)buf, bytes);
    delay(5); // wait a bit to let the other device catch up (64 * 9 bit with 115200 baud/s takes 5ms)
}

void serialMaintain()
{
	if (!headerValid && Serial.available() >= sizeof(SerialHeader))
	{
	    // skip data until magic number is found
	    while (Serial.available() && Serial.peek() != SERIAL_MAGIC_NUMBER)
		Serial.read();

    
	    if (Serial.available() < sizeof(SerialHeader))
		return;
	    
	    Serial.readBytes((char*)sHead.data, sizeof(SerialHeader));
	    headerValid = true;
			
	    // invalidate header of overlong packets
	    if (sHead.s.packetLength > MAX_SERIAL_BODY_LENGTH)
		headerValid = false;
	}
	
	// nothing to do?
	if (!headerValid || Serial.available() < sHead.s.packetLength)
		return;
		
	int bytesToRead = sHead.s.packetLength;
		
	switch (sHead.s.commandByte)
	{
	default:
		// invalid command
		break;
		
	case CMDS_PING:
	{
		char buf[15];
		int bytes = snprintf(buf, sizeof(buf), 
		    "%lu %d\n",
		    millis(), deviceState);
	    
		if (bytes > sizeof(buf))
		    bytes = sizeof(buf);
		
		sendHeader(CMDS_PONG, bytes);
		Serial.write((u8*)buf, bytes);
		break;
	}
	}

	// discard remaining data
	while (bytesToRead --> 0)
		Serial.read();
		
	headerValid = false;			
}


































