#include <stdbool.h>
#include <stdio.h>

#include "main.h"

#define MULTIPLY_FOR_1MS 1000


void Delay(const uint32_t count_ms);

void EnableClocking(void);

void GpioInit(void);
void GpioInitForUsart2(void);
void GpioInitForUsart3(void);

void UsartInit (void);
void Usart2Init(void);
void Usart3Init(void);

void UsartSendData(USART_TypeDef* USARTx, uint8_t data);
uint8_t UsartReceiveData(USART_TypeDef* USARTx);

int main(void)
{
    EnableClocking();
    GpioInit      ();
    UsartInit     ();

    uint8_t data1 = 42;
    uint8_t data2 = 0;
    while(true)
    {
        UsartSendData(USART2, data1);
        Delay(1000);
        data2 = UsartReceiveData(USART3);
        Delay(1000);
        ++data1;
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
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
}

void GpioInit(void)
{
    GpioInitForUsart2();
    GpioInitForUsart3();
}

void GpioInitForUsart2(void)
{
    GPIOA->CRL &= ~GPIO_CRL_MODE2_Msk;
    GPIOA->CRL &= ~GPIO_CRL_MODE3_Msk;

    GPIOA->CRL &= ~GPIO_CRL_CNF2_Msk;
    GPIOA->CRL &= ~GPIO_CRL_CNF3_Msk;

    GPIOA->CRL |= GPIO_CRL_MODE2;
    GPIOA->CRL |= GPIO_CRL_CNF2_1;

    GPIOA->CRL |= GPIO_CRL_CNF3_1;
    GPIOA->ODR |= GPIO_ODR_ODR3;
}

void GpioInitForUsart3(void)
{
    GPIOB->CRH &= ~GPIO_CRH_MODE10_Msk;
    GPIOB->CRH &= ~GPIO_CRH_MODE11_Msk;

    GPIOB->CRH &= ~GPIO_CRH_CNF10_Msk;
    GPIOB->CRH &= ~GPIO_CRH_CNF11_Msk;

    GPIOB->CRH |= GPIO_CRH_MODE10;
    GPIOB->CRH |= GPIO_CRH_CNF10_1;

    GPIOB->CRH |= GPIO_CRH_CNF11_1;
    GPIOB->ODR |= GPIO_ODR_ODR11;
}

void UsartInit(void)
{
    Usart2Init();
    Usart3Init();
}

void Usart2Init(void)
{
    USART2->BRR = (234 << 4) | (6 << 0); // 9600 baudrate and 36MHz
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}

void Usart3Init(void)
{
    USART3->BRR = (234 << 4) | (6 << 0); // 9600 baudrate and 36MHz
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART3->CR1 |= USART_CR1_UE;
}

void UsartSendData(USART_TypeDef* USARTx, uint8_t data)
{
    while(!(USARTx->SR & USART_SR_TXE)){}
    USARTx->DR = data;
    while(!(USARTx->SR & USART_SR_TC)){}
    USARTx->SR &= ~USART_SR_TC;
}

uint8_t UsartReceiveData(USART_TypeDef* USARTx)
{
    while(!(USARTx->SR & USART_SR_RXNE)){}

    return USARTx->DR;
}
