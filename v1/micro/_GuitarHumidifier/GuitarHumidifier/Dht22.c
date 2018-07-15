/*
 * Dht22.c
 *
 * Created: 7/15/2018 10:35:52 AM
 *  Author: piotr
 */ 

#include "Dht22.h"
#include "Pins.h"

void Dht22::Initialize(uint8_t pin)
{
	_pin = pin;
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



	uint32_t cycles[80];
	{
		
		delayMicroseconds(10);  // Delay a bit to let sensor pull data line low.

		// First expect a low signal for ~80 microseconds followed by a high signal
		// for ~80 microseconds again.
		if (expectPulse(LOW) == 0) {
			DEBUG_PRINTLN(F("Timeout waiting for start signal low pulse."));
			_lastresult = false;
			return _lastresult;
		}
		if (expectPulse(HIGH) == 0) {
			DEBUG_PRINTLN(F("Timeout waiting for start signal high pulse."));
			_lastresult = false;
			return _lastresult;
		}

		// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
		// microsecond low pulse followed by a variable length high pulse.  If the
		// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
		// then it's a 1.  We measure the cycle count of the initial 50us low pulse
		// and use that to compare to the cycle count of the high pulse to determine
		// if the bit is a 0 (high state cycle count < low state cycle count), or a
		// 1 (high state cycle count > low state cycle count). Note that for speed all
		// the pulses are read into a array and then examined in a later step.
		for (int i=0; i<80; i+=2) {
			cycles[i]   = expectPulse(LOW);
			cycles[i+1] = expectPulse(HIGH);
		}
	} // Timing critical code is now complete.

	// Inspect pulses and determine which ones are 0 (high state cycle count < low
	// state cycle count), or 1 (high state cycle count > low state cycle count).
	for (int i=0; i<40; ++i) {
		uint32_t lowCycles  = cycles[2*i];
		uint32_t highCycles = cycles[2*i+1];
		if ((lowCycles == 0) || (highCycles == 0)) {
			DEBUG_PRINTLN(F("Timeout waiting for pulse."));
			_lastresult = false;
			return _lastresult;
		}
		data[i/8] <<= 1;
		// Now compare the low and high cycle times to see if the bit is a 0 or 1.
		if (highCycles > lowCycles) {
			// High cycles are greater than 50us low cycle count, must be a 1.
			data[i/8] |= 1;
		}
		// Else high cycles are less than (or equal to, a weird case) the 50us low
		// cycle count so this must be a zero.  Nothing needs to be changed in the
		// stored data.
	}

	DEBUG_PRINTLN(F("Received:"));
	DEBUG_PRINT(data[0], HEX); DEBUG_PRINT(F(", "));
	DEBUG_PRINT(data[1], HEX); DEBUG_PRINT(F(", "));
	DEBUG_PRINT(data[2], HEX); DEBUG_PRINT(F(", "));
	DEBUG_PRINT(data[3], HEX); DEBUG_PRINT(F(", "));
	DEBUG_PRINT(data[4], HEX); DEBUG_PRINT(F(" =? "));
	DEBUG_PRINTLN((data[0] + data[1] + data[2] + data[3]) & 0xFF, HEX);

	// Check we read 40 bits and that the checksum matches.
	if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
		_lastresult = true;
		return _lastresult;
	}
	else {
		DEBUG_PRINTLN(F("Checksum failure!"));
		_lastresult = false;
		return _lastresult;
	}
}

bool Dht22::IsReady()
{
	PinModeD(_pin, 0);
	return DigitalReadD(_pin);
}

uint32_t Dht22::ExpectPulse(uint8_t level)
{
	
}