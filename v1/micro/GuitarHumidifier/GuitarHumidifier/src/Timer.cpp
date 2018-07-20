/*
 * Timer.c
 *
 * Created: 7/15/2018 12:49:40 PM
 *  Author: piotr
 */ 

#include "Timer.h"
#include <avr/io.h>
#include <util/atomic.h>

volatile uint32_t Timer::_overflowCount = 0;

void Timer::Initialize(uint64_t fcpu)
{
	_fcpu = fcpu;
	_cyclesPerMicrosecond = _fcpu / 1000000;

	//TCCR0A |= _BV(COM0A0) | _BV(WGM01);

	// Clock/8
	TCCR0A |= _BV(CS01);
	
	//OCF0A

	// Output compare register for timer 0.
	//OCR0A = 0xFF;
	
	// Enable the compare match interrupt
	TIMSK0 |= _BV(TOIE0);
}

uint32_t Timer::Microseconds()
{
	uint32_t ovf;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		ovf = Timer::_overflowCount;
	}

	return ((ovf << 8) + TCNT0) * 8 / _cyclesPerMicrosecond; // Because there is clock/8 preascale enabled
}


ISR (TIMER0_OVF_vect)
{
    ++Timer::_overflowCount;
}