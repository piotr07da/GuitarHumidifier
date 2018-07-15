/*
 * GuitarHumidifier.c
 *
 * Created: 7/14/2018 12:19:43 PM
 * Author : piotr
 */ 

#define F_CPU 14745600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Pins.h"
#include "Timer.h"
#include "Display.h"
#include "Dht22.h"


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
Dht22 _dht;


int main(void)
{
	DDRD |= _BV(6);
	//PORTD |= _BV(6);

	_timer.Initialize(F_CPU);
	_display.Initialize(2, DigitLatchPin, DataBitClockPin, DataBitPin, DigitSelectionPins);
	_dht.Initialize(PinDht22Data, &_timer);

	sei();

	while (1)
    {
		uint32_t ms = _timer.Microseconds();
		uint32_t dispVal = (ms / 1000000) % 100;

		_display.SetValue(dispVal);

		
		
		//float rh = _dht.ReadHumidity();
		//_display.SetValue(rh);
		

		_display.Update();

		PORTD ^= _BV(6);
    }
}
