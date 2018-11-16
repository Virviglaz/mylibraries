#include "allwinner_H3.h"
#include <stdbool.h>

void GPIO_Init (GPIO_TypeDef * GPIOx, uint32_t Pin, GPIO_FuncTypeDef Func);
uint32_t GPIO_Read (GPIO_TypeDef * GPIOx, uint32_t Pin);
void GPIO_Change (GPIO_TypeDef * GPIOx, uint32_t Pin, bool State);
void GPIO_Set (GPIO_TypeDef * GPIOx, uint32_t Pin);
void GPIO_Clear (GPIO_TypeDef * GPIOx, uint32_t Pin);
