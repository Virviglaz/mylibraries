#include "gpio.h"
#include "stm32f0xx_gpio.h"

GPIO_InitTypeDef GPIO_InitStruct;

void PIN_ON(GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIOx->BSRR = PINx;
}

void PIN_OFF(GPIO_TypeDef * GPIOx, uint16_t PINx)
{	
	GPIOx->BRR = PINx;
}

uint16_t PIN_SYG(GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	return GPIOx->IDR & PINx;
}

void PIN_IN (GPIO_TypeDef * GPIOx, uint16_t PINx)
{	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Pin  = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_IN_AN (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Pin  = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_PP_LS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_PP_MS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_PP_HS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_OD_LS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_OD_MS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_OD_HS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_INV(GPIO_TypeDef * GPIOx, uint16_t PINx)
{	
	GPIOx->ODR ^= PINx;
}

void PIN_IN_PU (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_IN_AF_PU (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_IN_AF (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_IN_PD (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_AF_PP_HS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_AF_OD_HS (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_Pin   = PINx;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}
