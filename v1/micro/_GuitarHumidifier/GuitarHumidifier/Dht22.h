/*
 * Dht22.h
 *
 * Created: 7/15/2018 10:31:19 AM
 *  Author: piotr
 */ 


#ifndef DHT22_H_
#define DHT22_H_

typedef Dht22 Dht22_t;

struct Dht22
{
	uint8_t _pin;
	uint8_t _data[5];
	void Initialize();
	bool Read();
	bool IsReady();
	uint32_t ExpectPulse(uint8_t level);
};


#endif /* DHT22_H_ */