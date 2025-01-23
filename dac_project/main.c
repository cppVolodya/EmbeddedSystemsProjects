#include <stdbool.h>
#include <stdio.h>

#include "main.h"


int main(void)
{
    EnableClocking();
    GpioInit();
    I2cInit();

    const uint8_t address = I2cScan();
    float voltage = 0;
    while(true)
    {
        if(voltage >= 3.3)
        {
            voltage = 0;
        }

        DacSetVoltage(address, voltage);
        voltage += 0.1;

        Delay(50);
    }
}

void Delay(const uint32_t count_ms)
{
    uint32_t count_ms_ = count_ms * MULTIPLY_FOR_1MS;
    while(count_ms_--)
    {
        __asm("nop");
    }
}

void EnableClocking(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
}

void GpioInit(void)
{
    GPIOB->CRH &= ~(GPIO_CRH_MODE10_Msk | GPIO_CRH_MODE11_Msk);
    GPIOB->CRH &= ~(GPIO_CRH_CNF10_Msk | GPIO_CRH_CNF11_Msk);

    GPIOB->CRH |= GPIO_CRH_MODE10 | GPIO_CRH_MODE11;

    GPIOB->CRH |= GPIO_CRH_CNF10 | GPIO_CRH_CNF11;
    //GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_CNF11_1;

    //GPIOB->ODR |= GPIO_ODR_ODR10;
    //GPIOB->ODR |= GPIO_ODR_ODR11;
}

void I2cInit(void)
{
    I2C2->CR1 |= I2C_CR1_SWRST;
    Delay(1);
    I2C2->CR1 &= ~I2C_CR1_SWRST;

    I2C2->CR2   |= I2C_FREQUENCY_PERIPHERAL;
    I2C2->CCR   |= I2C_FREQUENCY_CLOCK;
    I2C2->TRISE |= I2C_TRISE;

    I2C2->CR1 |= I2C_CR1_PE;
}

uint8_t I2cScan(void)
{
    for(uint8_t address = 1; address < MAX_DEVICES; ++address)
    {
        I2cStart();

        if(I2cSendAddress(address << 1) == 1)
        {
            I2cResetAddrFlag();

            I2cStop();
            return address;
        }
    }

    return 0;
}

void I2cStart(void)
{
    I2C2->CR1 |= I2C_CR1_START;

    while(!(I2C2->SR1 & I2C_SR1_SB)){}

    (void)(I2C2->SR1);
}

void I2cStop(void)
{
    I2C2->CR1 |= I2C_CR1_STOP;
}

void I2cResetAddrFlag(void)
{
    (void)(I2C2->SR1);
    (void)(I2C2->SR2);
}

bool I2cIsAck(void)
{
    if(I2C2->SR1 & I2C_SR1_AF)
    {
        I2C2->SR1 &= ~I2C_SR1_AF;
        return false;
    }

    return true;
}

uint8_t I2cSendAddress(const uint8_t address)
{
    I2C2->DR = address;
    while(!(I2C2->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))){}

    return I2cIsAck();
}

void I2cWriteData(const uint8_t data)
{
    while(!(I2C2->SR1 & I2C_SR1_TXE)){}

    I2C2->DR = data;

    while(!(I2C2->SR1 & I2C_SR1_BTF));
}

uint8_t I2cReadData(void)
{
    while(!(I2C2->SR1 & I2C_SR1_RXNE)){}

    return I2C2->DR;
}

void DacSetVoltage(const uint8_t address, const float voltage)
{
    const uint16_t value = (voltage / MCP4725_MAX_VOLTAGE) * MCP4725_MAX_RESOLUTION;
    MCP4725SetValue(address, value);
}

void MCP4725SetValue(const uint8_t address, const uint16_t value)
{
    I2cStart();

    I2cSendAddress(address << 1 | I2C_WRITE);
    I2cResetAddrFlag();

    uint8_t temp_data = MCP4725_FAST_AND_NORMAL_MODE | (value >> 8);
    I2cWriteData(temp_data);
    I2cWriteData(value & 0xFF);

    I2cStop();
}
