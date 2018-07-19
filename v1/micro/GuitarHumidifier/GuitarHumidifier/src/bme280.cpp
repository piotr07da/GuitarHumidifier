/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */


#include "bme280.h"
#include <math.h>
#include <avr/io.h>
#include "Wire.h"

uint16_t Bme280::read8(uint8_t reg)
{
	uint8_t b;
	if (ReadRegister(reg, &b))
	{
		return (uint16_t)b;
	}
	return 0;
}


uint16_t Bme280::read16(uint8_t reg)
{
  uint8_t b[2];
  if (ReadRegister(reg, b))
  {
	  return ((uint16_t)b[0] << 8) | (uint16_t)b[1];
  }
  return 0;
}

uint16_t Bme280::read16_LE(uint8_t reg)
{
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);
  
}

int16_t Bme280::readS16(uint8_t reg)
{
  return (int16_t)read16(reg);
  
}

int16_t Bme280::readS16_LE(uint8_t reg)
{
  return (int16_t)read16_LE(reg);
  
}

uint32_t Bme280::read24(uint8_t reg)
{
  uint8_t b[3];
  if (ReadRegister(reg, b))
  {
	  return ((uint32_t)b[0] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[2];
  }
  return 0;
}

void Bme280::bme280_readCoefficients(void)
{
  _bme280_calib.dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
  _bme280_calib.dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
  _bme280_calib.dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);
  
  _bme280_calib.dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
  _bme280_calib.dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
  _bme280_calib.dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
  _bme280_calib.dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
  _bme280_calib.dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
  _bme280_calib.dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
  _bme280_calib.dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
  _bme280_calib.dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
  _bme280_calib.dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);
  
  _bme280_calib.dig_H1 = read8(BME280_REGISTER_DIG_H1);
  _bme280_calib.dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
  _bme280_calib.dig_H3 = read8(BME280_REGISTER_DIG_H3);
  _bme280_calib.dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
  _bme280_calib.dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
  _bme280_calib.dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
}


void Bme280::bme280_init()
{
	Wire.beginTransmission(BME280_ADDRESS);
	Wire.write(BME280_REGISTER_CONTROLHUMID);
	Wire.write(0x05);
	Wire.write(BME280_REGISTER_CONTROL);
	Wire.write(0xB7);
	//Wire.write(BME280_REGISTER_CHIPID);
	Wire.endTransmission();
	bme280_readCoefficients();
}

uint8_t Bme280::bme280_readChipId(void)
{
	//uint8_t chipId;
	//ReadRegister(BME280_REGISTER_CHIPID, &chipId);
	//return chipId;
	return read8(BME280_REGISTER_CHIPID);

	//Wire.beginTransmission(BME280_ADDRESS);
	//Wire.write(BME280_REGISTER_CHIPID);
	//Wire.endTransmission();
	//Wire.requestFrom(BME280_ADDRESS, 1);
	//if (Wire.available())
	//{
		//return Wire.read();
	//}
	//return 19;
}


float Bme280::bme280_readTemperature(void)
{
  int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);

  int32_t var1, var2;
  
  adc_T >>= 4;
  
  var1  = ((((adc_T>>3) - ((int32_t)_bme280_calib.dig_T1 <<1))) *
           ((int32_t)_bme280_calib.dig_T2)) >> 11;
  
  var2  = (((((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1)) *
             ((adc_T>>4) - ((int32_t)_bme280_calib.dig_T1))) >> 12) *
           ((int32_t)_bme280_calib.dig_T3)) >> 14;
  
  t_fine = var1 + var2;
  
  float T  = (t_fine * 5 + 128) >> 8;

  return T/100;
}


float Bme280::bme280_readPressure(void)
{
  int64_t var1, var2, p;
  
  bme280_readTemperature(); // must be done first to get t_fine
  
  int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
  
  adc_P >>= 4;
  
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bme280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bme280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3)>>8) +
  ((var1 * (int64_t)_bme280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bme280_calib.dig_P1)>>33;
  
  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bme280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bme280_calib.dig_P8) * p) >> 19;
  
  return var2;
  
  p = ((p + var1 + var2) >> 8) + (((int64_t)_bme280_calib.dig_P7)<<4);
  
  return (float)p/256;
}

float Bme280::bme280_readHumidity(void)
{
  bme280_readTemperature(); // must be done first to get t_fine
  
  int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
  
  int32_t v_x1_u32r;
  
  v_x1_u32r = (t_fine - ((int32_t)76800));
  
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)_bme280_calib.dig_H4) << 20) -
                  (((int32_t)_bme280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
               (((((((v_x1_u32r * ((int32_t)_bme280_calib.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)_bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                  ((int32_t)2097152)) * ((int32_t)_bme280_calib.dig_H2) + 8192) >> 14));
  
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                             ((int32_t)_bme280_calib.dig_H1)) >> 4));
  
  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}

float Bme280::bme280_readAltitude(float seaLevel)
{
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
  
  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
  
  float atmospheric = bme280_readPressure() / 100.0F;
  
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

bool Bme280::ReadRegister(uint8_t registerAddress, uint8_t* bytes)
{
	Wire.beginTransmission(BME280_ADDRESS);
	Wire.write(registerAddress);
	Wire.endTransmission();
	Wire.requestFrom(BME280_ADDRESS, 1);
	uint8_t bIx = 0;
	while (Wire.available())
	{
		*(bytes + bIx) = Wire.read();
		++bIx;
	}
	return true;
}