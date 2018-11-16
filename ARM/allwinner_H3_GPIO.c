#include "allwinner_H3_GPIO.h"

void GPIO_Init (GPIO_TypeDef * GPIOx, uint32_t Pin, GPIO_FuncTypeDef Func)
{
	for (uint8_t i = 0; i < GPIO_PIN_MAX; i++)
		if (Pin & (1 << i))
			if (i <= 7) //CFG0
			{
				GPIOx->CFG0 &= ~(7 << i); // clear config bits
				GPIOx->CFG0 |= Func << i; // set config bits
			}
			else if (i <= 15) //CFG1
			{
				GPIOx->CFG1 &= ~(7 << (i - 8)); // clear config bits
				GPIOx->CFG1 |= Func << (i - 8); // set config bits
			}
			else //CFG2
			{
				GPIOx->CFG2 &= ~(7 << (i - 16)); // clear config bits
				GPIOx->CFG2 |= Func << (i - 16); // set config bits
			}
}

uint32_t GPIO_Read (GPIO_TypeDef * GPIOx, uint32_t Pin)
{
	return GPIOx->DAT & Pin;
}

void GPIO_Change (GPIO_TypeDef * GPIOx, uint32_t Pin, bool State)
{
	if (State)
		GPIOx->DAT |= Pin;
	else
		GPIOx->DAT &= ~Pin;
}

void GPIO_Set (GPIO_TypeDef * GPIOx, uint32_t Pin)
{
	GPIOx->DAT |= Pin;
}

void GPIO_Clear (GPIO_TypeDef * GPIOx, uint32_t Pin)
{
	GPIOx->DAT &= ~Pin;
}
