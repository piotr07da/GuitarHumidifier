/*
 * Pins.h
 *
 * Created: 7/15/2018 10:43:28 AM
 *  Author: piotr
 */ 

#ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>

void PinModeC(uint8_t pin, uint8_t mode);
void PinModeD(uint8_t pin, uint8_t mode);
void ShiftOutD(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
void DigitalReadD(uint8_t pin, uint8_t* value);
void DigitalWriteC(uint8_t pin, uint8_t value);
void DigitalWriteD(uint8_t pin, uint8_t value);

#endif /* PINS_H_ */