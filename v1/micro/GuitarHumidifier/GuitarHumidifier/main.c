/*
 * GuitarHumidifier.c
 *
 * Created: 7/14/2018 12:19:43 PM
 * Author : piotr
 */ 

#include <avr/io.h>
#include <util/delay.h>

const uint8_t digitPatterns[16] =
{
	0b00111111,  // 0
	0b00000110,  // 1
	0b01011011,  // 2
	0b01001111,  // 3
	0b01100110,  // 4
	0b01101101,  // 5
	0b01111101,  // 6
	0b00000111,  // 7
	0b01111111,  // 8
	0b01101111,  // 9
	0b01110111,  // A
	0b01111100,  // b
	0b00111001,  // C
	0b01011110,  // d
	0b01111001,  // E
	0b01110001   // F
};

const uint8_t PinHdSelection = 0;
const uint8_t PinLdSelection = 1;
const uint8_t PinData = 3;
const uint8_t PinBitClock = 4;
const uint8_t PinDigitClock = 5;

const uint8_t DigitSelectionPins[2] = { 0, 1 };

const uint32_t CounterDivider = 200;

uint32_t _counter = 0;
uint8_t _digitIx = 0;

void DisplayDigit(uint8_t digitIndex);
void ShiftOutD(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
void DigitalWriteD(uint8_t pin, uint8_t value);

int main(void)
{
	DDRD |= _BV(PinHdSelection) | _BV(PinLdSelection) | _BV(PinData) | _BV(PinBitClock) | _BV(PinDigitClock);
    
	uint8_t dVal = 0;
	uint8_t digitSelectionPin = DigitSelectionPins[0];;

	while (1) 
    {
		uint32_t val = (_counter / CounterDivider) % 100;
		dVal = val;
		for (uint8_t i = 0; i < _digitIx; ++i)
			dVal /= 10;
		dVal %= 10;

		DigitalWriteD(digitSelectionPin, 0);
		digitSelectionPin = DigitSelectionPins[_digitIx];

		DisplayDigit(dVal);

		DigitalWriteD(digitSelectionPin, 1);

		++_counter;
		++_digitIx;
		if (_digitIx == 2)
			_digitIx = 0;

		_delay_ms(5);
    }
}

void DisplayDigit(uint8_t digitIndex)
{
	uint8_t dp = digitPatterns[digitIndex];

	// turn off the output of 74HC595
	DigitalWriteD(PinDigitClock, 0);

	// update data pattern to be outputed from 74HC595
	// because it's a common anode LED, the pattern needs to be inverted
	ShiftOutD(PinData, PinBitClock, 1, ~dp);

	// turn on the output of 74HC595
	DigitalWriteD(PinDigitClock, 1);
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

void DigitalWriteD(uint8_t pin, uint8_t value)
{
	if (value == 0)
		PORTD &= ~_BV(pin);
	else
		PORTD |= _BV(pin);
}