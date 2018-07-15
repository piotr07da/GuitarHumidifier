/*
 * Dht22.c
 *
 * Created: 7/15/2018 10:35:52 AM
 *  Author: piotr
 */ 

#include "Dht22.h"
#include <avr/io.h>
#include <util/delay.h>
#include "Pins.h"

void Dht22::Initialize(uint8_t pin, Timer* timer)
{
	_pin = pin;
	_timer = timer;
}

float Dht22::ReadHumidity()
{
	float f = 0;
	if (Read())
	{
		f = _data[0];
		f *= 256;
		f += _data[1];
		f *= 0.1;
	}
	return f;
}

bool Dht22::Read()
{
	uint8_t i;
	
	// Reset data.
	for (i = 0; i < 5; ++i)
		_data[i] = 0;
	
	if (!IsReady())
		return false;
	
	// Send start signal.
	// Set low for at least 1 ms.
	PinModeD(_pin, 0);
	DigitalWriteD(_pin, 0);
	_delay_ms(2);
	DigitalWriteD(_pin, 1);

	// Wait 40us for reasponse to start signal.
	_delay_us(40);
	PinModeD(_pin, 0);

	// Wait additional 10us for dht to pull data line low.
	_delay_us(10);

	// First expect a low signal for ~80 microseconds followed by a high signal
	// for ~80 microseconds again.
	if (ExpectPulse(0) == 0) {
		return false;
	}
	if (ExpectPulse(1) == 0) {
		return false;
	}

	uint16_t cycles[80];

	// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
	// microsecond low pulse followed by a variable length high pulse.  If the
	// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
	// then it's a 1.  We measure the cycle count of the initial 50us low pulse
	// and use that to compare to the cycle count of the high pulse to determine
	// if the bit is a 0 (high state cycle count < low state cycle count), or a
	// 1 (high state cycle count > low state cycle count). Note that for speed all
	// the pulses are read into a array and then examined in a later step.
	for (int i = 0; i < 80; i += 2)
	{
		cycles[i]   = ExpectPulse(0);
		cycles[i+1] = ExpectPulse(1);
	}

	
	// Timing critical code is now complete.

	// Inspect pulses and determine which ones are 0 (high state cycle count < low
	// state cycle count), or 1 (high state cycle count > low state cycle count).
	for (int i=0; i<40; ++i)
	{
		uint16_t lowCycles  = cycles[2 * i];
		uint16_t highCycles = cycles[2 * i + 1];
		if ((lowCycles == 0) || (highCycles == 0))
		{
			return false;
		}
		_data[i / 8] <<= 1;
		// Now compare the low and high cycle times to see if the bit is a 0 or 1.
		if (highCycles > lowCycles)
		{
			// High cycles are greater than 50us low cycle count, must be a 1.
			_data[i / 8] |= 1;
		}
		// Else high cycles are less than (or equal to, a weird case) the 50us low
		// cycle count so this must be a zero.  Nothing needs to be changed in the
		// stored data.
	}

		// Check we read 40 bits and that the checksum matches.
	if (_data[4] == ((_data[0] + _data[1] + _data[2] + _data[3]) & 0xFF))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool Dht22::IsReady()
{
	PinModeD(_pin, 0);
	uint8_t isReady;
	DigitalReadD(_pin, &isReady);
	return isReady;
}

uint16_t Dht22::ExpectPulse(uint8_t level)
{
	// cache the port and bit of the pin in order to speed up the
	// pulse width measuring loop and achieve finer resolution.  calling
	// digitalRead() instead yields much coarser resolution.
	uint8_t bit = _BV(_pin);
	uint8_t stateMask = (level ? bit : 0);

	uint16_t maxMicros = _timer->Microseconds() + 1000;

	// wait for any previous pulse to end
	while ((PORTD & bit) == stateMask)
		if (_timer->Microseconds() > maxMicros)
			return 0;

	// wait for the pulse to start
	while ((PORTD & bit) != stateMask)
		if (_timer->Microseconds() > maxMicros)
			return 0;

	uint16_t start = _timer->Microseconds();
	// wait for the pulse to stop
	while ((PORTD & bit) == stateMask) {
		if (_timer->Microseconds() > maxMicros)
		return 0;
	}
	return _timer->Microseconds() - start;
}