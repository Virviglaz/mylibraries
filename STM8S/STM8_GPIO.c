#include "STM8_GPIO.h"

void PIN_ON(GPIO_TypeDef * GPIOx, u8 PINx)
{
	GPIOx->ODR = GPIOx->ODR | PINx;
}

void PIN_OFF(GPIO_TypeDef * GPIOx, u8 PINx)								
{
	GPIOx->ODR = GPIOx->ODR & (~PINx);
}

u8 PIN_SYG(GPIO_TypeDef * GPIOx, u8 PINx)
{
        return GPIOx->IDR & PINx;
}

void PIN_IN (GPIO_TypeDef * GPIOx, u8 PINx)
{
        GPIOx->DDR &= (~PINx);
        GPIOx->CR1 &= (~PINx);
}

void PIN_OUT_PP (GPIO_TypeDef * GPIOx, u8 PINx)
{
        GPIOx->DDR |= PINx;
        GPIOx->CR1 |= PINx;
}

void PIN_INV(GPIO_TypeDef * GPIOx, u8 PINx)
{
	GPIOx->ODR = GPIOx->ODR ^ PINx;
}

void PIN_IN_PU (GPIO_TypeDef * GPIOx,u8 PINx)
{
        GPIOx->DDR &= (~PINx);
        GPIOx->CR1 |= PINx;
}
