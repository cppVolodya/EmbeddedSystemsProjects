#include <stdbool.h>
#include <stdio.h>

#include "main.h"


int main(void)
{
    EnableClocking();
    GpioInit();
    SpiInit();

    int32_t temperature = 0;
    uint32_t pressure   = 0;

    SpiEnable();
    Bmp280Config();
    Bmp280ReadCompensationData();
    const uint8_t bmp280_id = Bmp280ReadDataFromRegister(BMP280_ADDRESS_ID);
    (void)bmp280_id;

    while(true)
    {
        Bmp280ReadData(&temperature, &pressure);
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
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
}

void GpioInit(void)
{
    GPIOA->CRL &= ~(GPIO_CRL_MODE4_Msk | GPIO_CRL_MODE5_Msk | GPIO_CRL_MODE6_Msk | GPIO_CRL_MODE7_Msk);
    GPIOA->CRL &= ~(GPIO_CRL_CNF4_Msk  | GPIO_CRL_CNF5_Msk  | GPIO_CRL_CNF6_Msk  | GPIO_CRL_CNF7_Msk);

    GPIOA->CRL |= GPIO_CRL_MODE5 | GPIO_CRL_MODE7;
    GPIOA->CRL |= GPIO_CRL_CNF5_1 | GPIO_CRL_CNF7_1;

    GPIOA->CRL |= GPIO_CRL_CNF6_1;
    GPIOA->ODR |= GPIO_ODR_ODR6;

    GPIOA->CRL |= GPIO_CRL_MODE4;
    GPIOA->ODR |= GPIO_ODR_ODR4;
}

void SpiInit(void)
{
    SPI1->CR1 = 0;

    SPI1->CR1 |= (SPI_CR1_CPOL | SPI_CR1_CPHA);
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1);
    SPI1->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);

    SPI1->CR2 = 0;
}

void SpiEnable(void)
{
    SPI1->CR1 |= SPI_CR1_SPE;
}

void SpiDisable(void)
{
    SPI1->CR1 &= ~SPI_CR1_SPE;
}

void SelectBmp280(void)
{
    GPIOA->ODR &= ~GPIO_ODR_ODR4;
}

void DeSelectBmp280(void)
{
    GPIOA->ODR |= GPIO_ODR_ODR4;
}

void ClearOverunFlag(void)
{
    while(SPI1->SR & SPI_SR_BSY){}
    while(!(SPI1->SR & SPI_SR_TXE)){}

    (void)SPI1->DR;
    (void)SPI1->SR;
}

void SpiTransmitData(const uint8_t data)
{
    while(!(SPI1->SR & SPI_SR_TXE)){}
    SPI1->DR = data;
    ClearOverunFlag();
}

uint8_t SpiReciveData(void)
{
    while(SPI1->SR & SPI_SR_BSY){}
    SPI1->DR = 0;
    while(!(SPI1->SR & SPI_SR_RXNE)){}

    return SPI1->DR;
}

void Bmp280WriteDataToRegister(const uint8_t data, const uint8_t register_)
{
    SelectBmp280();

    SpiSelectRegister(SPI_WRITE & register_);
    SpiTransmitData(data);

    DeSelectBmp280();
}

uint8_t Bmp280ReadDataFromRegister(const uint8_t register_)
{
    SelectBmp280();

    SpiSelectRegister(SPI_READ | register_);

    const uint8_t data = SpiReciveData();

    DeSelectBmp280();

    return data;
}

void Bmp280Config()
{
    Bmp280Reset      ();
    Bmp280SetСtrlMeas();
    Bmp280SetConfig  ();
}

void Bmp280Reset()
{
    Bmp280WriteDataToRegister(BMP280_VALUE_RESET, BMP280_ADDRESS_RESET);
    Delay(1000);
}

void Bmp280SetСtrlMeas()
{
    Bmp280WriteDataToRegister(BMP280_VALUE_CTRL_MEAS, BMP280_ADDRESS_CTRL_MEAS);
}

void Bmp280SetConfig()
{
    Bmp280WriteDataToRegister(BMP280_VALUE_CONFIG, BMP280_ADDRESS_CONFIG);
}

void Bmp280ReadCompensationData(void)
{
    Bmp280ReadCompensationTemperature();
    Bmp280ReadCompensationPressure   ();
}

void Bmp280ReadCompensationTemperature(void)
{
    uint8_t u_temp_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T1_MSB);
    uint8_t u_temp_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T1_LSB);
    G_DIG_T1 = (uint16_t)((u_temp_msb << 8) | u_temp_lsb);

    u_temp_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T2_MSB);
    u_temp_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T2_LSB);
    G_DIG_T2 = (int16_t)((u_temp_msb << 8) | u_temp_lsb);

    u_temp_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T3_MSB);
    u_temp_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_T3_LSB);
    G_DIG_T3 = (int16_t)((u_temp_msb << 8) | u_temp_lsb);
}

void Bmp280ReadCompensationPressure(void)
{
    uint8_t u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P1_MSB);
    uint8_t u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P1_LSB);
    G_DIG_P1 = (uint16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P2_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P2_LSB);
    G_DIG_P2 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P3_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P3_LSB);
    G_DIG_P3 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P4_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P4_LSB);
    G_DIG_P4 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P5_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P5_LSB);
    G_DIG_P5 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P6_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P6_LSB);
    G_DIG_P6 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P7_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P7_LSB);
    G_DIG_P7 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P8_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P8_LSB);
    G_DIG_P8 = (int16_t)((u_press_msb << 8) | u_press_lsb);

    u_press_msb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P9_MSB);
    u_press_lsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_DIG_P9_LSB);
    G_DIG_P9 = (int16_t)((u_press_msb << 8) | u_press_lsb);
}

void Bmp280ReadData(int32_t *temperature, uint32_t *pressure)
{
    *temperature = Bmp280ReadTemperature();
    *pressure    = Bmp280ReadPressure   ();

    *temperature = Bmp280CompensateTemperatureInteger32(*temperature);
    *pressure    = Bmp280CompensatePressureInteger32   (*pressure);

    *temperature /= DIVIDER_TEMPERATURE_PRESSURE;
    *pressure    /= DIVIDER_TEMPERATURE_PRESSURE;
}

int32_t Bmp280ReadTemperature(void)
{
    const uint8_t temp_xlsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_TEMP_XLSB);
    const uint8_t temp_lsb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_TEMP_LSB);
    const uint8_t temp_msb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_TEMP_MSB);

    int32_t result = (int32_t)(((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | ((uint32_t)temp_xlsb >> 4));

    return result;
}

uint32_t Bmp280ReadPressure(void)
{
    const uint8_t press_xlsb = Bmp280ReadDataFromRegister(BMP280_ADDRESS_PRESSURE_XLSB);
    const uint8_t press_lsb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_PRESSURE_LSB);
    const uint8_t press_msb  = Bmp280ReadDataFromRegister(BMP280_ADDRESS_PRESSURE_MSB);

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
