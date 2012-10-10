#include <PinConfig.h>

Timer::Timer( u32 intval )
	: interval(intval), lastCheck(millis())
{
}

bool Timer::check()
{
	if (millis() - lastCheck <= interval)
		return false;

	lastCheck = millis();
	return true;
}

void Timer::reset()
{
    lastCheck = millis();
}

unsigned char mosfet[] = 
{
    MAIN, 	// main MOSFET
    OUTPUT0, 	// output 1 (left)
    OUTPUT1, 	// output 2
    WIND_SHORT, // gpio mosfet (planned for shorting wind turbines)
    ISENSE, 	// mosfets for turning off current sensors
};

u8 mosfetStates()
{
    u8 res = 0;
    
    for (int i = 0; i < sizeof(mosfet); i++)
	res |= digitalRead(mosfet[i]) << i;

    return res;
}

void pinSetup()
{
	for (u8 i = 0; i < sizeof(mosfet); i++)
	{
		pinMode(mosfet[i], OUTPUT);
		digitalWrite(mosfet[i], LOW);
	}
		
	// setzte ADC auf langsamen Modus (128 prescaler)
	ADCSRA |= 0b00000111;
}

void schalteMOSFET(unsigned int index, unsigned char status)
{
	if (index > sizeof(mosfet))
		return;
		
	digitalWrite(mosfet[index], status);
}

void schalteMOSFETs(i16 indices, unsigned char status)
{
	for (u8 i = 0; i < sizeof(mosfet); i++)
		if (indices & (1 << (sizeof(mosfet) - i)))
			digitalWrite(mosfet[i], status);
}

u8 generatorenState = AUS;
void schalteGeneratoren(u8 to)
{
    schalteMOSFET(MAIN, to);
    generatorenState = to;
}

void alleAusgaenge(u8 to)
{
	// Setzte alle MOSFET Pins auf to
	schalteMOSFET(OUTPUT0, to);
	schalteMOSFET(OUTPUT1, to);
}


