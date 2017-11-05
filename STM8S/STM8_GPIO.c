#include "STM8_GPIO.h"

void PIN_ON(GPIO_TypeDef * GPIOx, uint8_t PINx)
{
	GPIOx->ODR = GPIOx->ODR | PINx;
}

void PIN_OFF(GPIO_TypeDef * GPIOx, uint8_t PINx)								
{
	GPIOx->ODR = GPIOx->ODR & (~PINx);
}

uint8_t PIN_SYG(GPIO_TypeDef * GPIOx, uint8_t PINx)
{
        return GPIOx->IDR & PINx;
}

void PIN_IN (GPIO_TypeDef * GPIOx, uint8_t PINx)
{
        GPIOx->DDR &= (~PINx);
        GPIOx->CR1 &= (~PINx);
}

void PIN_OUT_PP (GPIO_TypeDef * GPIOx, uint8_t PINx)
{
        GPIOx->DDR |= PINx;
        GPIOx->CR1 |= PINx;
}

void PIN_INV(GPIO_TypeDef * GPIOx, uint8_t PINx)
{
	GPIOx->ODR = GPIOx->ODR ^ PINx;
}

void PIN_IN_PU (GPIO_TypeDef * GPIOx,uint8_t PINx)
{
        GPIOx->DDR &= (~PINx);
        GPIOx->CR1 |= PINx;
}
