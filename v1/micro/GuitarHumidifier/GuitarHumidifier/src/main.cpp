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
#include "Display.h"

#define __avc_BR 9600UL // bit rate
#define __avc_UBRR (F_CPU / (16UL * __avc_BR)) - 1 // usart baud rate register

const uint8_t DataBitPin = PINB0;
const uint8_t DataBitClockPin = PINB2;
const uint8_t DigitLatchPin = PINB1;

const uint8_t DigitSelectionPins[2] = { PIND6, PIND7 };

const uint32_t CounterDivider = 2000;

uint8_t _digitIx = 0;

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

uint16_t _counter = 0;
int32_t t = 0;
uint32_t h = 0;
uint32_t p = 0;
uint8_t dv = 0;
uint8_t thp = 0;

int main(void)
{
	DDRD |= _BV(3);

	_display.Initialize(2, DigitLatchPin, DataBitClockPin, DataBitPin, DigitSelectionPins);

	InitUSART(__avc_UBRR);

	sei();

	Wire.begin();

	_bme280dev.dev_id = BME280_I2C_ADDR_PRIM;
	_bme280dev.intf = BME280_I2C_INTF;
	_bme280dev.read = user_i2c_read;
	_bme280dev.write = user_i2c_write;
	_bme280dev.delay_ms = user_delay_ms;

	_rslt = bme280_init(&_bme280dev);

	uint8_t settings_sel = 0;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_FILTER_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	_bme280dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	_bme280dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	_bme280dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	_bme280dev.settings.filter = BME280_FILTER_COEFF_16;
	_bme280dev.settings.standby_time = BME280_STANDBY_TIME_1000_MS;

	bme280_set_sensor_settings(settings_sel, &_bme280dev);
	bme280_set_sensor_mode(BME280_NORMAL_MODE, &_bme280dev);
	
	while (1)
    {
		if (_counter == 30000)
		{
			_counter = 0;

			bme280_get_sensor_data(BME280_ALL, &_data, &_bme280dev);

			t = _data.temperature;
			h = _data.humidity >> 10;
			p = _data.pressure / 10000;

			if (thp == 0)
			{
				SendValue('T', t);
				dv = t / 100;
			}
			else if (thp == 1)
			{
				SendValue('H', h);
				dv = h;

				if (h < 45)
				{
					PORTD |= _BV(3);	
				}
				else
				{
					PORTD &= ~_BV(3);
				}
			}
			else if (thp == 2)
			{
				SendValue('P', p);
				dv = p % 100;
			}
			
			_display.SetValue(dv);
			++thp;
			if (thp == 2)
				thp = 0;
		}
		
		_display.Update();

		++_counter;
    }
}

void InitUSART(uint8_t ubrr)
{
	// Set the baud rate.
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	
	// Enable UART receiver and transmitter and receiver interrupt handlings
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	
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