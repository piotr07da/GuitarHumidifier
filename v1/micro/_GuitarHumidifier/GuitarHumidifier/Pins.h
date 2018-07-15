/*
 * Pins.h
 *
 * Created: 7/15/2018 10:43:28 AM
 *  Author: piotr
 */ 

 #ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>

void PinModeD(uint8_t pin, uint8_t mode);
void ShiftOutD(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
void DigitalReadD(uint8_t pin, uint8_t* value);
void DigitalWriteD(uint8_t pin, uint8_t value);

void PinModeD(uint8_t pin, uint8_t mode)
{
	if (mode == 0)
		DDRD |= _BV(pin);
	else
		DDRD &= ~_BV(pin);
}

void ShiftOutD(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; ++i)
	{
		if (bitOrder == 0)
		DigitalWriteD(dataPin, !!(val & (1 << i)));
		else
		DigitalWriteD(dataPin, !!(val & (1 << (7 - i))));

		DigitalWriteD(clockPin, 1);
		DigitalWriteD(clockPin, 0);
	}
}

void DigitalReadD(uint8_t pin, uint8_t* value)
{
	*value = !!(PORTD & _BV(pin));
}

void DigitalWriteD(uint8_t pin, uint8_t value)
{
	if (value == 0)
		PORTD &= ~_BV(pin);
	else
		PORTD |= _BV(pin);
}

#endif /* PINS_H_ */