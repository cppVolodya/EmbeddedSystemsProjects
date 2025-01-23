#include <stdbool.h>
#include <stdio.h>

#include "main.h"


void Delay(uint32_t count_ms);

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= GPIO_CRL_MODE5_0;

    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
    GPIOA->CRL |= GPIO_CRL_CNF4_0;

    while(true)
    {
        if((GPIOA->IDR & GPIO_IDR_IDR4) == GPIO_IDR_IDR4)
        {
            GPIOA->ODR |= GPIO_ODR_ODR5;
        }
        else
        {
            GPIOA->ODR &= ~GPIO_ODR_ODR5;
        }

        Delay(100);
    }
}

void Delay(uint32_t count_ms)
{
    count_ms = count_ms * 1000;
    while(count_ms--)
    {
        __asm("nop");
    }
}
