/*
 * Pins.cpp
 *
 * Created: 7/15/2018 1:23:58 PM
 *  Author: piotr
 */ 

#include "Pins.h"

void PinModeC(uint8_t pin, uint8_t mode)
{
	if (mode == 0)
	DDRC &= ~_BV(pin);
	else
	DDRC |= _BV(pin);
}

void PinModeD(uint8_t pin, uint8_t mode)
{
	if (mode == 0)
		DDRD &= ~_BV(pin);
	else
		DDRD |= _BV(pin);
}

void ShiftOutB(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; ++i)
	{
		if (bitOrder == 0)
			DigitalWriteB(dataPin, !!(val & (1 << i)));
		else
			DigitalWriteB(dataPin, !!(val & (1 << (7 - i))));

		DigitalWriteB(clockPin, 1);
		DigitalWriteB(clockPin, 0);
	}
}

void DigitalReadD(uint8_t pin, uint8_t* value)
{
	*value = !!(PIND & _BV(pin));
}

void DigitalWriteB(uint8_t pin, uint8_t value)
{
	if (value == 0)
		PORTB &= ~_BV(pin);
	else
		PORTB |= _BV(pin);
}

void DigitalWriteC(uint8_t pin, uint8_t value)
{
	if (value == 0)
		PORTC &= ~_BV(pin);
	else
		PORTC |= _BV(pin);
}

void DigitalWriteD(uint8_t pin, uint8_t value)
{
	if (value == 0)
		PORTD &= ~_BV(pin);
	else
		PORTD |= _BV(pin);
}