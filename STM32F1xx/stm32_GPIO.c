#include "stm32_GPIO.h"
#include "stm32f10x_gpio.h"

void PIN_ON(GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIOx->BSRR=PINx;
}

void PIN_OFF(GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIOx->BRR=PINx;
}

u16 PIN_SYG(GPIO_TypeDef * GPIOx, u16 PINx)
{
	return GPIOx->IDR & PINx;
}

void PIN_SET(GPIO_TypeDef * GPIOx, u16 PINx, u8 State)
{
	State ? PIN_ON(GPIOx, PINx) : PIN_OFF(GPIOx, PINx);
}

void PIN_IN (GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_PP (GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_OD (GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_INV(GPIO_TypeDef * GPIOx, u16 PINx)
{
	GPIOx->ODR ^= PINx;
}

void PIN_IN_PU (GPIO_TypeDef * GPIOx, u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_IN_PD (GPIO_TypeDef * GPIOx, u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_AF_PP (GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void PIN_OUT_AF_OD (GPIO_TypeDef * GPIOx,u16 PINx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=PINx;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void gpio_init (GPIO_TypeDef * GPIOx, u16 PINx, GPIOSpeed_TypeDef GPIO_Speed, GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_InitStruct = { PINx, GPIO_Speed, GPIO_Mode };
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}
