/*
 * Display.h
 *
 * Created: 7/15/2018 12:14:54 PM
 *  Author: piotr
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

static const uint8_t DigitSegmentBits[10] =
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
};

class Display
{
private:
	
	uint32_t _timeCounter;
	uint8_t _digitCount;
	uint8_t _digitIndex;
	uint8_t _digitLatchPin;
	uint8_t _dataBitClockPin;
	uint8_t _dataBitPin;
	uint8_t* _digitSelectionPins;
	uint8_t _digitSelectionPin;
	uint8_t _value;
	
public:
	void Initialize(uint8_t digitCount, uint8_t digitLatchPin, uint8_t dataBitClockPin, uint8_t dataBitPin, const uint8_t* digitSelectionPins);
	void SetValue(uint8_t value);
	void Update();

private:
	void DisplayCurrentDigit();
	void DisplayDigit(uint8_t digitIndex);
};



#endif /* DISPLAY_H_ */