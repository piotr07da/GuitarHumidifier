/*
 * Dht22.h
 *
 * Created: 7/15/2018 10:31:19 AM
 *  Author: piotr
 */ 


#ifndef DHT22_H_
#define DHT22_H_

#include <avr/io.h>

#include "Timer.h"

class Dht22
{
private:
	uint8_t _pin;
	uint8_t _data[5];
	Timer* _timer;

public:
	void Initialize(uint8_t pin, Timer* timer);
	float ReadHumidity();

private:
	bool Read();
	bool IsReady();
	uint16_t ExpectPulse(uint8_t level);
};


#endif /* DHT22_H_ */