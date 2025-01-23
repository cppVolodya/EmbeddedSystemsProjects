#ifndef MAIN_H
#define MAIN_H

#include "stm32f103xb.h"

#define MULTIPLY_FOR_1MS 1000

#define I2C_FREQUENCY_PERIPHERAL 36
#define I2C_FREQUENCY_CLOCK      180
#define I2C_TRISE                37

#define I2C_WRITE 0
#define I2C_READ  1

#define MAX_DEVICES 128

//#define MCP4725_ADDRESS  0x60

#define MCP4725_FAST_AND_NORMAL_MODE 0x00

#define MCP4725_MAX_VOLTAGE	3.3
#define MCP4725_MAX_RESOLUTION 4095


void Delay(const uint32_t count_ms);

void EnableClocking(void);

void GpioInit(void);
void I2cInit (void);

uint8_t I2cScan(void);

void I2cStart(void);
void I2cStop (void);

void I2cResetAddrFlag(void);
bool I2cIsAck(void);

uint8_t I2cSendAddress(const uint8_t address);

void I2cWriteData(const uint8_t data);
uint8_t I2cReadData(void);

void DacSetVoltage(const uint8_t address, const float voltage);
void MCP4725SetValue(const uint8_t address, const uint16_t value);

#endif // MAIN_H
