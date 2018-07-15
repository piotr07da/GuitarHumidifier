/*
 * Timer.c
 *
 * Created: 7/15/2018 12:49:40 PM
 *  Author: piotr
 */ 

#include "Timer.h"

void Timer::Initialize()
{
	_us = 0;

	TCCR0B |= _BV(CS00);
	TIMSK |= _BV(TOIE0);
}

uint32_t Timer::Microseconds()
{
	return _us;
}

ISR(TIMER0_OVF_vect)
{
	++_us;
}
