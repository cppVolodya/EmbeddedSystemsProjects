#include <stdbool.h>
#include <stdio.h>

#include "main.h"

#define MULTIPLY_FOR_1MS         1000
#define MAX_ADC_VALUE_FOR_LED_ON 100


void Delay(uint32_t count_ms);

void EnableClocking(void);

void AdcInit (void);
void GpioInit(void);

uint16_t AdcRead(void);
void LedSetState(uint8_t state);

int main(void)
{
    EnableClocking();
    AdcInit       ();
    GpioInit      ();

    while(true)
    {
        uint16_t adc_value = AdcRead();

        if (adc_value < MAX_ADC_VALUE_FOR_LED_ON)
        {
            LedSetState(1);
        }
        else
        {
            LedSetState(0);
        }
    }
}

void EnableClocking(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}

void AdcInit(void)
{
    ADC1->CR1 = 0;
    ADC1->CR2 = ADC_CR2_ADON;

    ADC1->SMPR2 = ADC_SMPR2_SMP1;

    ADC1->SQR1 = 0;
    ADC1->SQR3 = 0;
}

uint16_t AdcRead(void)
{
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;

    while (!(ADC1->SR & ADC_SR_EOC))
    {
    }

    return ADC1->DR;
}

void GpioInit(void)
{
    GPIOA->CRL &= ~GPIO_CRL_MODE0;
    GPIOA->CRL &= ~GPIO_CRL_CNF0;

    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_MODE4_1;
}

void LedSetState(uint8_t state)
{
    if (state)
    {
        GPIOA->ODR |= GPIO_ODR_ODR4;
    }
    else
    {
        GPIOA->ODR &= ~GPIO_ODR_ODR4;
    }
}

void Delay(uint32_t count_ms)
{
    count_ms = count_ms * MULTIPLY_FOR_1MS;
    while(count_ms--)
    {
        __asm("nop");
    }
}
