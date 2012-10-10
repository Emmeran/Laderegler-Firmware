#ifndef PINCONFIG
#define PINCONFIG

#include <Arduino.h>

#define pinI2C 7

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define i16 int16_t
#define i32 int32_t

#define AN HIGH
#define AUS LOW

#include "Protocols.h"

#include "AtkaSPOT_Laderegler_v1.h"
#include "SerialCommunication.h"
#include "Spannungsmessung.h"
#include "Strommessung.h"
#include "Temperatur.h"
#include "State.h"

#define ms 
#define Minuten * 60000 ms // 60.000ms = 1min
#define Sekunden * 1000 ms // 1.000ms = 1s

typedef u32 Time;
#define passed(since, amount) (millis() - since >= amount)

class Timer
{
public:
	Time interval;
	Time lastCheck;
	
	Timer( Time intval );
	bool check();
	void reset();
};

extern u32 vbat, vgen;
extern i16 vbatTC;

extern Time sLastPong;
extern Time lastStateChange;
extern Time lastWatchdogOverride;

extern unsigned char mosfet[5];

enum MosFETs
{
    MAIN = 7,
    OUTPUT0 = 3,
    OUTPUT1 = 2,
    WIND_SHORT = 9,
    ISENSE = 8,
};

#define ISENSE_COUNT 6
extern i16 stroeme[ISENSE_COUNT];
extern u8 Ichannels[ISENSE_COUNT];
extern u8 generatorenState;

enum Inputs
{
    ISENSE0 = A0,
    ISENSE1 = A1,
    ISENSE2 = A2,
    ISENSE3 = A3,
    ISENSE4 = A4,
    ISENSE5 = A5,
    
    VSENSE_BAT = A6,
    VSENSE_GEN = A7,
};

u8 mosfetStates();
void pinSetup();
#define schalteAusgang(index, status) schalteMOSFET(index, status)
void schalteMOSFET(unsigned int index, unsigned char status);
void schalteMOSFETs(i16 indices, unsigned char status);
void alleAusgaenge(u8 to);
void schalteGeneratoren(u8 to);

#endif

