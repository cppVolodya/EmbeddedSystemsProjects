#include <stdbool.h>
#include <stdio.h>

#include "main.h"


int main(void)
{
    EnableClocking();
    GpioInit();
    I2cInit();

    uint8_t address = I2cScan();

    int32_t temperature = 0;
    uint32_t pressure   = 0;

    Bmp280Config(address);
    Bmp280ReadCompensationData(address);
    uint8_t bmp280_id = Bmp280ReadDataFromRegister(BMP280_ADDRESS_ID, address);
    (void)bmp280_id;

    while(true)
    {
        Bmp280ReadData(&temperature, &pressure, address);
        Delay(1000);
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

uint8_t I2cSendAddress(const uint8_t address)
{
    I2C2->DR = address;
    while(!(I2C2->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))){}

    if(I2C2->SR1 & I2C_SR1_AF)
    {
        I2C2->SR1 &= ~I2C_SR1_AF;
        return 0;
    }

    return 1;
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

void I2cReceiveSingleByte(void)
{
    I2C2->CR1 &= ~I2C_CR1_ACK;
    I2cResetAddrFlag();
    I2cStop();
}

void Bmp280WriteDataToRegister(const uint8_t data, const uint8_t register_, const uint8_t address)
{
    I2cStart();

    I2cSendAddress(address << 1 | I2C_WRITE);
    I2cResetAddrFlag();
    I2cWriteData(register_);
    I2cWriteData(data);

    I2cStop();
}

void Bmp280Config(const uint8_t address)
{
    Bmp280Reset      (address);
    Bmp280SetСtrlMeas(address);
    Bmp280SetConfig  (address);
}

void Bmp280Reset(const uint8_t address)
{
    Bmp280WriteDataToRegister(BMP280_VALUE_RESET, BMP280_ADDRESS_RESET, address);
}

void Bmp280SetСtrlMeas(const uint8_t address)
{
    Bmp280WriteDataToRegister(BMP280_VALUE_CTRL_MEAS, BMP280_ADDRESS_CTRL_MEAS, address);
}

void Bmp280SetConfig(const uint8_t address)
{
    Bmp280WriteDataToRegister(BMP280_VALUE_CONFIG, BMP280_ADDRESS_CONFIG, address);
}

void Bmp280ReadCompensationData(const uint8_t address)
{
    Bmp280ReadCompensationTemperature(address);
    Bmp280ReadCompensationPressure   (address);
}

void Bmp280ReadCompensationTemperature(const uint8_t address)
{
    uint8_t u_temp_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T1_MSB, address);
    uint8_t u_temp_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T1_LSB, address);
    G_DIG_T1 = (uint16_t)((u_temp_msb << 8) | u_temp_lsb);

    u_temp_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T2_MSB, address);
    u_temp_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T2_LSB, address);
    G_DIG_T2 = (int16_t)((u_temp_msb << 8) | u_temp_lsb);

    u_temp_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T3_MSB, address);
    u_temp_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T3_LSB, address);
    G_DIG_T3 = (int16_t)((u_temp_msb << 8) | u_temp_lsb);

    Delay(1000);
}

void Bmp280ReadCompensationPressure(const uint8_t address)
{
    uint8_t u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P1_MSB, address);
    uint8_t u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P1_LSB, address);
    G_DIG_P1 = (uint16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P2_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P2_LSB, address);
    G_DIG_P2 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P3_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P3_LSB, address);
    G_DIG_P3 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P4_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P4_LSB, address);
    G_DIG_P4 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P5_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P5_LSB, address);
    G_DIG_P5 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P6_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P6_LSB, address);
    G_DIG_P6 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P7_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P7_LSB, address);
    G_DIG_P7 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P8_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P8_LSB, address);
    G_DIG_P8 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P9_MSB, address);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P9_LSB, address);
    G_DIG_P9 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    Delay(1000);
}

uint8_t Bmp280ReadDataFromRegister(const uint8_t register_, const uint8_t address)
{
    I2cStart();
    I2cSendAddress(address << 1 | I2C_WRITE);
    I2cResetAddrFlag();
    I2cWriteData(register_);

    I2cStart();
    I2cSendAddress(address << 1 | I2C_READ);
    I2cReceiveSingleByte();

    return I2cReadData();
}

void Bmp280ReadData(int32_t *temperature, uint32_t *pressure, const uint8_t address)
{
    *temperature = Bmp280ReadTemperature(address);
    *pressure    = Bmp280ReadPressure   (address);

    *temperature = Bmp280CompensateTemperatureInteger32(*temperature);
    *pressure    = Bmp280CompensatePressureInteger32   (*pressure);

    *temperature /= DIVIDER_TEMPERATURE_PRESSURE;
    *pressure    /= DIVIDER_TEMPERATURE_PRESSURE;
}

int32_t Bmp280ReadTemperature(const uint8_t address)
{
    const uint8_t temp_xlsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_TEMP_XLSB, address);
    const uint8_t temp_lsb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_TEMP_LSB,  address);
    const uint8_t temp_msb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_TEMP_MSB,  address);

    int32_t result = (int32_t)(((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | ((uint32_t)temp_xlsb >> 4));

    return result;
}

uint32_t Bmp280ReadPressure(const uint8_t address)
{
    const uint8_t press_xlsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_PRESSURE_XLSB, address);
    const uint8_t press_lsb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_PRESSURE_LSB,  address);
    const uint8_t press_msb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_PRESSURE_MSB,  address);

    uint32_t result = (int32_t)(((uint32_t)press_msb << 12) | ((uint32_t)press_lsb << 4) | ((uint32_t)press_xlsb >> 4));

    return result;
}

int32_t Bmp280CompensateTemperatureInteger32(const int32_t adc_temperture)
{
    int32_t var1 = ((((adc_temperture >> 3) - ((int32_t)G_DIG_T1 << 1))) * ((int32_t)G_DIG_T2)) >> 11;
    int32_t var2 = (((((adc_temperture >> 4) - ((int32_t)G_DIG_T1)) * ((adc_temperture >> 4) - ((int32_t)G_DIG_T1))) >> 12) * ((int32_t)G_DIG_T3)) >> 14;
    G_T_FINE = var1 + var2;

    int32_t result = (G_T_FINE * 5 + 128) >> 8;

    return result;
}

uint32_t Bmp280CompensatePressureInteger32(const uint32_t adc_pressure)
{
    int64_t var1 = (((int32_t)G_T_FINE) >> 1) - ((int32_t)64000);
    int64_t var2 = (((var1 >> 2) * (var1 >> 2)) >> 11 ) * ((int32_t)G_DIG_P6);
    var2 = var2 + ((var1 * ((int32_t)G_DIG_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)G_DIG_P4) << 16);
    var1 = (((G_DIG_P3 * (((var1>>2) * (var1 >> 2)) >> 13 )) >> 3) + ((((int32_t)G_DIG_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768+var1)) * ((int32_t)G_DIG_P1)) >> 15);

    if (var1 == 0)
    {
        return 0;
    }

    uint32_t result = (((uint32_t)(((int32_t)1048576) - adc_pressure) - (var2 >> 12))) * 3125;

    if (result < 0x80000000)
    {
        result = (result << 1) / ((uint32_t)var1);
    }
    else
    {
        result = (result / (uint32_t)var1) * 2;
    }

    var1 = (((int32_t)G_DIG_P9) * ((int32_t)(((result>>3) * (result >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(result>>2)) * ((int32_t)G_DIG_P8))>>13;

    result = (uint32_t)((int32_t)result + ((var1 + var2 + G_DIG_P7) >> 4));

    return result;
}
