/*
 * Display.h
 *
 * Created: 7/15/2018 12:14:54 PM
 *  Author: piotr
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

typedef struct Display Display_t;

struct Display
{
	const uint8_t DigitSegmentBits[10] =
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

	uint32_t _timeCounter;
	uint8_t _digitCount;
	uint8_t _digitIndex;
	uint8_t* _digitSelectionPins;
	uint8_t _digitSelectionPin;
	uint8_t _value;
	

	void Initialize(uint8_t digitCount, uint8_t* digitSelectionPins);
	void SetValue(uint8_t value);
	void Update();
	void DisplayCurrentDigit();
	void DisplayDigit(uint8_t digitIndex);
};



#endif /* DISPLAY_H_ */