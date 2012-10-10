#include "PinConfig.h"
#include <Arduino.h>

void setup() 
{
    serialSetup();
    
    pinSetup();
    tempSetup();
  
    stromSetup();
    spannungsSetup();
}

DeviceState deviceState = STATE_BOOTUP;

Timer messungen(500 ms);
Timer serialBinary(5 Sekunden);
Timer serialPWRSF(5 Minuten);
Timer serialNRM(1 Minuten);
Timer debug(2 Sekunden);
Time lastStateChange;

void loop()
{
    if (messungen.check())
    {
	pruefeBatterieSpannung();
	messeTemperatur();
    }
    
    if (serialBinary.check())
	sendBinaryStatus();
    
    serialMaintain();
    
    switch (deviceState)
    {
    default:
    case STATE_BOOTUP:
	// wait for device to be ready
	break;
    
    // powersafe mode, do nothing
    case STATE_POWERSAFE:
    {
	if (serialPWRSF.check())
	    sendStatus();
	break;
    }
    
    case STATE_OVERPOWER:
    case STATE_NORMAL:
    {    
	messeStrom();
	
	if (serialNRM.check())
	    sendStatus();
	
	break;
    }
    }
}

































