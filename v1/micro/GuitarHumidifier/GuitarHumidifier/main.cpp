/*
 * GuitarHumidifier.c
 *
 * Created: 7/14/2018 12:19:43 PM
 * Author : piotr
 */ 

 #include <GlobalConstants.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <i2c_master.h>

#include <Pins.h>
#include <Timer.h>
#include <Display.h>

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

int main(void)
{
	DDRB |= _BV(1);
	//PORTB |= _BV(1);

	//_timer.Initialize(F_CPU);
	_display.Initialize(2, DigitLatchPin, DataBitClockPin, DataBitPin, DigitSelectionPins);
	//_dht.Initialize(PinDht22Data, &_timer);

	sei();

	_display.SetValue(99);

	uint8_t th = 0;

	while (1)
    {
		//uint32_t ms = _timer.Microseconds();
		//uint32_t dispVal = (ms / 1000000) % 100;
		//_display.SetValue(dispVal);

		
		
		//float rh = _dht.ReadHumidity();
		//_display.SetValue(rh);
		
		if (_counter % 10000 == 0)
		{
			float t;
			float h;
			//dht_gettemperaturehumidity(&t, &h);
			float x = th == 0 ? t : h;
			_display.SetValue(x);
			if (th == 0)
				th = 1;
			else
				th = 0;
		}
		
		_display.Update();

		PORTD ^= _BV(6);

		++ _counter;
    }
}
