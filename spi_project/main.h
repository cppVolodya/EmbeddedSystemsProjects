#ifndef MAIN_H
#define MAIN_H

#include "stm32f103xb.h"

#define MULTIPLY_FOR_1MS 1000

#define SPI_WRITE 0x7F
#define SPI_READ  0x80

#define BMP280_ADDRESS_ID 0xD0

#define BMP280_ADDRESS_RESET 0xE0
#define BMP280_VALUE_RESET 0xB6

#define BMP280_ADDRESS_CTRL_MEAS 0xF4
#define BMP280_VALUE_CTRL_MEAS 0x57  // oversampling temperature x2, oversampling pressure x16, normal mode

#define BMP280_ADDRESS_CONFIG 0xF5
#define BMP280_VALUE_CONFIG 0x50  // IIR filter coefficient 16, standby time 125 ms

#define BMP280_ADDRESS_TEMP_XLSB 0xFC
#define BMP280_ADDRESS_TEMP_LSB  0xFB
#define BMP280_ADDRESS_TEMP_MSB  0xFA

#define BMP280_ADDRESS_PRESSURE_XLSB 0xF9
#define BMP280_ADDRESS_PRESSURE_LSB  0xF8
#define BMP280_ADDRESS_PRESSURE_MSB  0xF7

#define BMP280_ADDRESS_DIG_T1_LSB 0x88
#define BMP280_ADDRESS_DIG_T1_MSB 0x89

#define BMP280_ADDRESS_DIG_T2_LSB 0x8A
#define BMP280_ADDRESS_DIG_T2_MSB 0x8B

#define BMP280_ADDRESS_DIG_T3_LSB 0x8C
#define BMP280_ADDRESS_DIG_T3_MSB 0x8D

#define BMP280_ADDRESS_DIG_P1_LSB 0x8E
#define BMP280_ADDRESS_DIG_P1_MSB 0x8F

#define BMP280_ADDRESS_DIG_P2_LSB 0x90
#define BMP280_ADDRESS_DIG_P2_MSB 0x91

#define BMP280_ADDRESS_DIG_P3_LSB 0x92
#define BMP280_ADDRESS_DIG_P3_MSB 0x93

#define BMP280_ADDRESS_DIG_P4_LSB 0x94
#define BMP280_ADDRESS_DIG_P4_MSB 0x95

#define BMP280_ADDRESS_DIG_P5_LSB 0x96
#define BMP280_ADDRESS_DIG_P5_MSB 0x97

#define BMP280_ADDRESS_DIG_P6_LSB 0x98
#define BMP280_ADDRESS_DIG_P6_MSB 0x99

#define BMP280_ADDRESS_DIG_P7_LSB 0x9A
#define BMP280_ADDRESS_DIG_P7_MSB 0x9B

#define BMP280_ADDRESS_DIG_P8_LSB 0x9C
#define BMP280_ADDRESS_DIG_P8_MSB 0x9D

#define BMP280_ADDRESS_DIG_P9_LSB 0x9E
#define BMP280_ADDRESS_DIG_P9_MSB 0x9F

#define DIVIDER_TEMPERATURE_PRESSURE 100

#define SpiTransmitData SpiSelectRegister


uint16_t G_DIG_T1,
		 G_DIG_P1;
int16_t G_DIG_T2, G_DIG_T3,
		G_DIG_P2, G_DIG_P3, G_DIG_P4, G_DIG_P5, G_DIG_P6, G_DIG_P7, G_DIG_P8, G_DIG_P9;

int32_t G_T_FINE = 0;

void Delay(const uint32_t count_ms);

void EnableClocking(void);
void GpioInit	   (void);
void SpiInit	   (void);

void SpiEnable(void);
void SpiDisable(void);

void SelectBmp280(void);
void DeselectBmp280(void);

void ClearOverunFlag(void);

void SpiTransmitData(const uint8_t data);
uint8_t SpiReciveData(void);

void Bmp280WriteDataToRegister(const uint8_t data, const uint8_t register_);
uint8_t Bmp280ReadDataFromRegister(const uint8_t register_);

void Bmp280Config		();
void Bmp280Reset		();
void Bmp280SetСtrlMeas	();
void Bmp280SetConfig	();

void Bmp280ReadCompensationData		  ();
void Bmp280ReadCompensationTemperature();
void Bmp280ReadCompensationPressure   ();

void Bmp280ReadData(int32_t *temperature, uint32_t *pressure);
int32_t  Bmp280ReadTemperature();
uint32_t Bmp280ReadPressure   ();

int32_t Bmp280CompensateTemperatureInteger32(const int32_t  adc_temperture);
uint32_t Bmp280CompensatePressureInteger32  (const uint32_t adc_pressure);

#endif // MAIN_H
