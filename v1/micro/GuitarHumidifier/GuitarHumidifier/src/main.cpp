/*
 * GuitarHumidifier.c
 *
 * Created: 7/14/2018 12:19:43 PM
 * Author : piotr
 */ 

#include "GlobalConstants.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#include "Wire.h"

#include "BoschSensortec/bme280_defs.h"
#include "BoschSensortec/bme280.h"

#include "Pins.h"
#include "Timer.h"
#include "Display.h"

#define __avc_BR 9600UL // bit rate
#define __avc_UBRR (F_CPU / (16UL * __avc_BR)) - 1 // usart baud rate register

const uint8_t DataBitPin = PIND2;
const uint8_t DataBitClockPin = PIND3;
const uint8_t DigitLatchPin = PIND4;

const uint8_t DigitSelectionPins[2] = { PINC3, PINC2 };

const uint32_t CounterDivider = 2000;

uint32_t _counter = 0;
uint8_t _digitIx = 0;

Timer _timer;
Display _display;


bme280_dev _bme280dev;
int8_t _rslt = BME280_OK;
bme280_data _data;

void InitUSART(uint8_t ubrr);
void SendByteUSART(uint8_t byte);
void SendValue(char type, float v);
void SendValue(char type, int32_t v);
void SendValue(char type, uint32_t v);


int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	int8_t rslt = 0;

	Wire.beginTransmission(dev_id);
	Wire.write(reg_addr);
	Wire.endTransmission();

	Wire.requestFrom(dev_id, len);
	uint16_t bIx = 0;
	while (Wire.available())
	{
		*(data + bIx) = Wire.read();
		++bIx;
	}
	Wire.endTransmission();

	return rslt;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	int8_t rslt = 0;

	Wire.beginTransmission(dev_id);
	Wire.write(reg_addr);
	uint16_t bIx = 0;
	while (bIx < len)
	{
		Wire.write(data[bIx]);
		++bIx;
	}
	Wire.endTransmission();

	return rslt;
}

void user_delay_ms(uint32_t period)
{
	_delay_ms(period);
}


int main(void)
{
	DDRD |= _BV(5) | _BV(6) | _BV(7);
	//PORTB |= _BV(1);

	//_timer.Initialize(F_CPU);
	_display.Initialize(2, DigitLatchPin, DataBitClockPin, DataBitPin, DigitSelectionPins);
	//_dht.Initialize(PinDht22Data, &_timer);

	InitUSART(__avc_UBRR);

	sei();

	Wire.begin();


	_bme280dev.dev_id = BME280_I2C_ADDR_PRIM;
	_bme280dev.intf = BME280_I2C_INTF;
	_bme280dev.read = user_i2c_read;
	_bme280dev.write = user_i2c_write;
	_bme280dev.delay_ms = user_delay_ms;

	_rslt = bme280_init(&_bme280dev);

	//uint8_t chipId;
	//user_i2c_read(BME280_I2C_ADDR_PRIM, BME280_CHIP_ID_ADDR, &chipId, 1);
	//user_i2c_read(0x0A, 0x04, &chipId, 1);
	//_rslt = -chipId;

	

	_display.SetValue(_bme280dev.chip_id);
	_display.Update();

	uint8_t th = 0;

	//_bme280.bme280_init();

	//Setup();

	
	_bme280dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	_bme280dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	_bme280dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	_bme280dev.settings.filter = BME280_FILTER_COEFF_16;
	_bme280dev.settings.standby_time = BME280_STANDBY_TIME_1_MS;

	uint8_t settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	bme280_set_sensor_settings(settings_sel, &_bme280dev);
	bme280_set_sensor_mode(BME280_NORMAL_MODE, &_bme280dev);
	

	while (1)
    {
		//uint32_t ms = _timer.Microseconds();
		//uint32_t dispVal = (ms / 1000000) % 100;
		//_display.SetValue(dispVal);

		
		
		//float rh = _dht.ReadHumidity();
		//_display.SetValue(rh);
		
		//if (_counter % 100 == 0)
		//{
		//	_a = CurrentVolume() * 100;
		//}
		

		if (_counter % 50000 == 0)
		{
			int32_t t = 3;
			uint32_t h = 3;
			uint32_t p = 3;
			
			bme280_get_sensor_data(BME280_ALL, &_data, &_bme280dev);

			t = _data.temperature / 100;
			h = _data.humidity >> 10;
			p = (_data.pressure >> 8) / 10;

			if (th == 0)
			{
				SendValue('T', t);
				PORTD &= 0b00011111;
				PORTD |= _BV(5);
			}
			else if (th == 1)
			{
				SendValue('H', h);
				PORTD &= 0b00011111;
				PORTD |= _BV(6);
			}
			else
			{
				SendValue('P', p);
				PORTD &= 0b00011111;
				PORTD |= _BV(7);
			}

			_display.SetValue(19);
			++th;
			if (th == 3)
				th = 0;
		}
		
		_display.Update();

		++ _counter;
    }
}


ISR (USART_RX_vect)
{
	uint8_t b = UDR0;
}

void InitUSART(uint8_t ubrr)
{
	// Set the baud rate.
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	
	// Enable UART receiver and transmitter and receiver interrupt handlings
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
	
	// Set to 8 data bits, 1 stop bit, async
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void SendByteUSART(uint8_t byte)
{
	while (!(UCSR0A & _BV(UDRE0))) ;
	UDR0 = byte;
}

void SendValue(char type, float v)
{
	char s[10];
	for (int i = 0; i < 10; ++i)
		s[i] = '#';

	dtostrf(v, 10, 5, s);
	
	SendByteUSART('|');
	SendByteUSART(type);
	SendByteUSART('=');
	for (int i = 0; i < 10; ++i)
		SendByteUSART(s[i]);
	SendByteUSART('|');
	SendByteUSART('\n');
}


void SendValue(char type, int32_t v)
{
	char c[50];
	for (int i = 0; i < 50; ++i)
		c[i] = ' ';
	itoa((int)v, c, 10);
	SendByteUSART('|');
	SendByteUSART(type);
	SendByteUSART('=');
	for (int i = 0; i < 50; ++i)
		SendByteUSART(c[i]);
	SendByteUSART('|');
	SendByteUSART('\n');
}

void SendValue(char type, uint32_t v)
{
	SendValue(type, (int32_t)v);
}