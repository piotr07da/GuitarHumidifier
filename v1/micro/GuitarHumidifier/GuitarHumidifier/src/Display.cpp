/*
 * Display.c
 *
 * Created: 7/15/2018 12:15:40 PM
 *  Author: piotr
 */ 

#include "Display.h"
#include "Pins.h"

void Display::Initialize(uint8_t digitCount, uint8_t digitLatchPin, uint8_t dataBitClockPin, uint8_t dataBitPin, const uint8_t* digitSelectionPins)
{
	_timeCounter = 0;

	_digitCount = digitCount;
	_digitIndex = 0;
	_digitLatchPin = digitLatchPin;
	_dataBitClockPin = dataBitClockPin;
	_dataBitPin = dataBitPin;
	_digitSelectionPins = (uint8_t*)digitSelectionPins;
	_digitSelectionPin = _digitSelectionPins[0];

	DDRB |= _BV(_digitLatchPin) | _BV(_dataBitClockPin) | _BV(_dataBitPin);
	for (uint8_t ix = 0; ix < _digitCount; ++ix)
		DDRD |= _BV(_digitSelectionPins[ix]);
}

void Display::SetValue(uint8_t value)
{
	_value = value;
}

void Display::Update()
{
	++_timeCounter;
	if (_timeCounter % 100 != 0)
		return;

	DisplayCurrentDigit();

	++_digitIndex;
	if (_digitIndex == _digitCount)
		_digitIndex = 0;
}

void Display::DisplayCurrentDigit()
{
	DigitalWriteD(_digitSelectionPin, 0);
	
	_digitSelectionPin = _digitSelectionPins[_digitIndex];

	uint8_t digitValue = _value;
	for (uint8_t i = 0; i < _digitIndex; ++i)
	digitValue /= 10;
	digitValue %= 10;

	DisplayDigit(digitValue);

	DigitalWriteD(_digitSelectionPin, 1);

}

void Display::DisplayDigit(uint8_t digitIndex)
{
	uint8_t dp = DigitSegmentBits[digitIndex];

	// turn off the output of 74HC595
	DigitalWriteB(_digitLatchPin, 0);

	// update data pattern to be outputed from 74HC595
	// because it's a common anode LED, the pattern needs to be inverted
	ShiftOutB(_dataBitPin, _dataBitClockPin, 1, ~dp);

	// turn on the output of 74HC595
	DigitalWriteB(_digitLatchPin, 1);
}