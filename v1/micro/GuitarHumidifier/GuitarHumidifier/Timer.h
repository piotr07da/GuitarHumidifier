/*
 * Timer.h
 *
 * Created: 7/15/2018 12:17:24 PM
 *  Author: piotr
 */ 


#ifndef TIMER_H_
#define TIMER_H_


#include <avr/io.h>

class Timer
{
public:
	static volatile uint32_t _overflowCount;

private:
	uint64_t _fcpu;
	uint32_t _cyclesPerMicrosecond;

public:
	void Initialize(uint64_t fcpu);
	uint32_t Microseconds();
};



#endif /* TIMER_H_ */