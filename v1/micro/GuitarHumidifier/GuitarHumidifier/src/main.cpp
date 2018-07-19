/*
 * GuitarHumidifier.c
 *
 * Created: 7/14/2018 12:19:43 PM
 * Author : piotr
 */ 

#include "GlobalConstants.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#include "Wire.h"

#include "bme280.h"

#include "Pins.h"
#include "Timer.h"
#include "Display.h"

const uint8_t PinHdSelection = 0;
const uint8_t PinLdSelection = 1;
const uint8_t PinDht22Data = 5;
const uint8_t DataBitPin = 2;
const uint8_t DataBitClockPin = 3;
const uint8_t DigitLatchPin = 4;

const uint8_t DigitSelectionPins[2] = { 0, 1 };

const uint32_t CounterDivider = 2000;

uint32_t _counter = 0;
uint8_t _digitIx = 0;

Timer _timer;
Display _display;
Bme280 _bme280;



float _targetVolume = .5;
float _minAngle = -110;
float _maxAngle = 165;
float _smoothRatio = .2;
float _prevVol = 0;


void Setup();

bool ReadAxisAcceleration(uint8_t axisIndex, float* acceleration);

bool ReadRegister(uint8_t registerAddress, uint8_t* byte);

float CurrentVolume();

float _a = 0;

int main(void)
{
	DDRB |= _BV(1);
	//PORTB |= _BV(1);

	//_timer.Initialize(F_CPU);
	_display.Initialize(2, DigitLatchPin, DataBitClockPin, DataBitPin, DigitSelectionPins);
	//_dht.Initialize(PinDht22Data, &_timer);

	Wire.begin();

	sei();

	_display.SetValue(99);

	uint8_t th = 0;

	_bme280.bme280_init();

	//Setup();

	while (1)
    {
		//uint32_t ms = _timer.Microseconds();
		//uint32_t dispVal = (ms / 1000000) % 100;
		//_display.SetValue(dispVal);

		
		
		//float rh = _dht.ReadHumidity();
		//_display.SetValue(rh);
		
		//if (_counter % 100 == 0)
		//{
		//	_a = CurrentVolume() * 100;
		//}
		

		if (_counter % 10000 == 0)
		{
			float t = 3;
			float h = 3;
			float p = 3;
			t = _bme280.bme280_readTemperature();
			h = _bme280.bme280_readHumidity();
			//p = _bme280.bme280_readPressure();
			
			float x;
			if (th == 0)
				x = t;
			else if (th == 1)
				x = h;
			else
				x = p;

			uint8_t chipId = _bme280.bme280_readChipId();
			if (chipId == 2)
				chipId = 13;

			_display.SetValue(x);
			++th;
			if (th == 3)
				th = 0;
		}
		
		_display.Update();

		PORTD ^= _BV(6);

		++ _counter;
    }
}


void Setup()
{
		Wire.beginTransmission(0x0A);
		Wire.write(0x22); // set sensitivity register
		Wire.write(0x00); // write mode value to sensitivity register
		Wire.write(0x20); // set filter register
		Wire.write(0x05); // write mode value to filter register
		Wire.endTransmission();
}

bool ReadAxisAcceleration(uint8_t axisIndex, float* acceleration)
{
	uint8_t reg;
	switch (axisIndex)
	{
		case 0: reg = 0x04; break;
		case 1: reg = 0x06; break;
		case 2: reg = 0x08; break;
		default: return false;
	}

	uint8_t rb;
	if (!ReadRegister(reg, &rb))
		return false;

	int8_t srb = (int8_t)rb >> 2;
	*acceleration = (float)srb / (float)16.0;
		return true;
}

bool ReadRegister(uint8_t registerAddress, uint8_t* byte)
{
	{
		Wire.beginTransmission(0x0A);
		Wire.write(registerAddress);
		Wire.endTransmission();
		Wire.requestFrom(0x0A, 1);
		if (Wire.available())
		{
			*byte = Wire.read();
			return true;
		}
		return false;
	}
}

float CurrentVolume()
{

	float x0;
	float x1;
	ReadAxisAcceleration(1, &x0);
	ReadAxisAcceleration(2, &x1);
	
	float mag = (float)sqrt(x0 * x0 + x1 * x1);
	x0 /= mag;
	x1 /= mag;

	float angle = (float)atan2(x1, x0);
	angle = 180.0 * angle / M_PI;

	float vol = (angle - _minAngle) / (_maxAngle - _minAngle);

	_prevVol = _prevVol + _smoothRatio * (vol - _prevVol);
	return _prevVol;
}