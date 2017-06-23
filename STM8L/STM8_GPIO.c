/* VERSION 2.21 */
/*  22.04.2016  */
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

u8 PIN_SYG_FILTERED (GPIO_TypeDef * GPIOx, u8 PINx)
{
  u8 tmp = 0;
  u8 cnt;
  for (cnt = 0; cnt!=0xFF; cnt++)
    if (PIN_SYG(GPIOx, PINx)) tmp++;
  if (tmp > 127) return 1;
  return 0;
}

u8 PIN_SYG_FILTERED_WORD (GPIO_TypeDef * GPIOx, u8 PINx)
{
  const u16 MaxVal = 0xFFF0;
  u16 tmp = 0;
  u16 cnt;
  for (cnt = 0; cnt != MaxVal; cnt++) 
    if (PIN_SYG(GPIOx, PINx)) tmp++;
  if (tmp > MaxVal / 2) return 1;
  return 0;
}
