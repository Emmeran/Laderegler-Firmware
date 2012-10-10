#include "PinConfig.h"

i16 stroeme[] = {0,0,0,0,0,0};
u8 Ichannels[] =
    {
	ISENSE0,
	ISENSE1,
	ISENSE2,
	ISENSE3,
	ISENSE4,
	ISENSE5,
    };

void stromSetup()
{
    for (int i = 0; i < ISENSE_COUNT; i++)
	stroeme[i] = 0;
}

Timer tStrom(500 ms);
u8 tSensorenStatus = AUS;
void schalteStromSensoren(u8 to)
{
    schalteMOSFET(ISENSE, to);
    tSensorenStatus = to;
}

// misst Strom an Pin mit ACS758
// gibt gemessenen Wert in mA zurück
void messeStrom()
{
    if (!tStrom.check())
	return;
    
    schalteStromSensoren(AN);
    
    for (int i = 0; i < ISENSE_COUNT; i++)
    {
	// [0:1023] -> [0:3.3]V -> [0:33]A
	// ACS758-50U Daten:
	//   Messbereich:  0 - 50A unidirectional
	//   Sensitivität: 61mV / A (typ. -40°C)
	//   Offset:       0.6V
	//   Offset Drift: +/-40mV bei -40°C
	
	i32 wert     = analogRead(Ichannels[i]);
	i32 spannung = wert * 5000 /* mV */ / 1023; // [0:1023] -> [0:3300]mV
	stroeme[i]   = (spannung - 590 /* mV */) * 1000 /* mA/A */ / (60 /* mV/A */);
    }
    
    schalteStromSensoren(AUS);
}
