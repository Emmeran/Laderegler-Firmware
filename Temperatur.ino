#include "PinConfig.h"
#include "OneWire.h"

// todo: make copy array so less ram is used
TemperatureAddress tempAddr[MAX_SENSORS];

TemperatureRole roles[MAX_SENSORS];
u8 nRoles = 0;

TemperatureMeasurement tempVals[MAX_SENSORS];

u8 sensorsFound = 0;

Timer tempInterval(5000);
Timer discInterval(60000);
u8 messIndex = 0;
u8 tempWartend = 0;

i16 vbatTC = 0;

OneWire ds(pinI2C);

#define RESOLUTION(x) ((x >> 5) & 0b00000011)

void tempSetup()
{
	tempDiscover();
}

void tempDiscover()
{
	int ptr = 0;
	ds.reset_search();
	
	while (ptr < MAX_SENSORS && ds.search(tempAddr[ptr].data))
		ptr++;
	
	sensorsFound = ptr;
	
	if (messIndex > sensorsFound)
		messIndex = 0;
}

void logTemperature()
{
/*	SDlog(" - Temperatures: (%2d sensors):\n", sensorsFound);
	for (int i = 0; i < sensorsFound; i++)
		SDlog("\t\t%08lX:%08lX: %4dC\n", tempAddr[i].s.a1, tempAddr[i].s.a2, tempVals[i].s.celcius10);*/
    #warning todo!
}

#define HEXOUT(x) if (x < 0x10) Serial.print("0"); Serial.print(x, HEX);

void messeTemperatur()
{
	if (discInterval.check())
		tempDiscover();
		
	if (sensorsFound == 0)
		return;
		
	if (!tempInterval.check())
		return;
	
	if (tempWartend && !ds.done())
		return;
		
	if (tempWartend)
	{
		u8 present = ds.reset();
		ds.select(tempAddr[messIndex].data);
		ds.write(0xBE); // read scratchpad
		
		byte data[9];
		for (int i = 0; i < 9; i++) 
			data[i] = ds.read();
			
		if (data[8] != OneWire::crc8( data, 8))
		{
			tempVals[messIndex].data[0] = 0xFF;
			tempVals[messIndex].data[1] = 0xFF;
			tempVals[messIndex].data[2] = 0xFF;
			tempVals[messIndex].data[3] = 0xFF;
		    
			// only 1 sensors available: invalid => no compensation
			vbatTC = 0;
		}
			
		else
		{
			// valid measurement
			tempVals[messIndex].s.raw = (((int16_t)data[1]) << 8) | data[0];
			tempVals[messIndex].s.config = data[4];
			tempVals[messIndex].s.celcius10 = tempVals[messIndex].s.raw * 5 / 8;
			
			// only 1 temperature sensor planned => use for temperature compensation
			vbatTC = (tempVals[messIndex].s.celcius10 - 250) * 6 * 4 / 10;
		}
			
		++messIndex %= sensorsFound;
	}
	
	// starte neue Messung
	ds.reset();
	ds.select(tempAddr[messIndex].data);
	ds.write(0x44, 1);
	tempWartend = 1;
}



























