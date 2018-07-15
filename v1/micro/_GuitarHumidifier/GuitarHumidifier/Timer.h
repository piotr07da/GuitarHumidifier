/*
 * Timer.h
 *
 * Created: 7/15/2018 12:17:24 PM
 *  Author: piotr
 */ 


#ifndef TIMER_H_
#define TIMER_H_

typedef struct Timer Timer_t;

struct Timer
{
	volatile uint32_t _us;
	void Initialize();
	uint32_t Microseconds();
};



#endif /* TIMER_H_ */