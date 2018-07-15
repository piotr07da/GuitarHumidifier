/*
 * GuitarHumidifier.c
 *
 * Created: 7/14/2018 12:19:43 PM
 * Author : piotr
 */ 

 #define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Pins.h"
#include "Timer.h"
#include "Display.h"
#include "Dht22.h"


const uint8_t PinHdSelection = 0;
const uint8_t PinLdSelection = 1;
const uint8_t PinDht22Data = 2;
const uint8_t PinData = 3;
const uint8_t PinBitClock = 4;
const uint8_t PinDigitClock = 5;

const uint8_t DigitSelectionPins[2] = { 0, 1 };

const uint32_t CounterDivider = 2000;

uint32_t _counter = 0;
uint8_t _digitIx = 0;

Timer_t _timer;
Display_t _display;
Dht22_t _dht;

int main(void)
{
	DDRD |= _BV(PinHdSelection) | _BV(PinLdSelection) | _BV(PinData) | _BV(PinBitClock) | _BV(PinDigitClock);
    
	_timer.Initialize();
	_display.Initialize(2, DigitSelectionPins));
	_dht.Initialize(PinDht22Data);

	sei();

	uint8_t dVal = 0;
	uint8_t digitSelectionPin = DigitSelectionPins[0];;

	

	while (1)
    {
		uint32_t dispVal = _timer.Microseconds() / 1000000;

		_display.SetValue(dispVal);
		_display.Update();
		
		//_dht.Read
		
    }
}
