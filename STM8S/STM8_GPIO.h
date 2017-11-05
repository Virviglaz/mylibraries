#ifndef STM8_GPIO_H
#define STM8_GPIO_H

#define _STDINT
#include "stm8s_gpio.h"

#define PA0			GPIOA, GPIO_PIN_0
#define PA1			GPIOA, GPIO_PIN_1
#define PA2			GPIOA, GPIO_PIN_2
#define PA3			GPIOA, GPIO_PIN_3
#define PA4			GPIOA, GPIO_PIN_4
#define PA5			GPIOA, GPIO_PIN_5
#define PA6			GPIOA, GPIO_PIN_6
#define PA7			GPIOA, GPIO_PIN_7

#define PB0			GPIOB, GPIO_PIN_0
#define PB1			GPIOB, GPIO_PIN_1
#define PB2			GPIOB, GPIO_PIN_2
#define PB3			GPIOB, GPIO_PIN_3
#define PB4			GPIOB, GPIO_PIN_4
#define PB5			GPIOB, GPIO_PIN_5
#define PB6			GPIOB, GPIO_PIN_6
#define PB7			GPIOB, GPIO_PIN_7

#define PC0			GPIOC, GPIO_PIN_0
#define PC1			GPIOC, GPIO_PIN_1
#define PC2			GPIOC, GPIO_PIN_2
#define PC3			GPIOC, GPIO_PIN_3
#define PC4			GPIOC, GPIO_PIN_4
#define PC5			GPIOC, GPIO_PIN_5
#define PC6			GPIOC, GPIO_PIN_6
#define PC7			GPIOC, GPIO_PIN_7

#define PD0			GPIOD, GPIO_PIN_0
#define PD1			GPIOD, GPIO_PIN_1
#define PD2			GPIOD, GPIO_PIN_2
#define PD3			GPIOD, GPIO_PIN_3
#define PD4			GPIOD, GPIO_PIN_4
#define PD5			GPIOD, GPIO_PIN_5
#define PD6			GPIOD, GPIO_PIN_6
#define PD7			GPIOD, GPIO_PIN_7

#define PE0			GPIOE, GPIO_PIN_0
#define PE1			GPIOE, GPIO_PIN_1
#define PE2			GPIOE, GPIO_PIN_2
#define PE3			GPIOE, GPIO_PIN_3
#define PE4			GPIOE, GPIO_PIN_4
#define PE5			GPIOE, GPIO_PIN_5
#define PE6			GPIOE, GPIO_PIN_6
#define PE7			GPIOE, GPIO_PIN_7

#define PF0			GPIOF, GPIO_PIN_0
#define PF1			GPIOF, GPIO_PIN_1
#define PF2			GPIOF, GPIO_PIN_2
#define PF3			GPIOF, GPIO_PIN_3
#define PF4			GPIOF, GPIO_PIN_4
#define PF5			GPIOF, GPIO_PIN_5
#define PF6			GPIOF, GPIO_PIN_6
#define PF7			GPIOF, GPIO_PIN_7

void PIN_ON(GPIO_TypeDef * GPIOx, uint8_t PINx);
void PIN_OFF(GPIO_TypeDef * GPIOx, uint8_t PINx);
uint8_t PIN_SYG(GPIO_TypeDef * GPIOx, uint8_t PINx);
void PIN_IN (GPIO_TypeDef * GPIOx, uint8_t PINx);
void PIN_OUT_PP (GPIO_TypeDef * GPIOx, uint8_t PINx);
void PIN_INV(GPIO_TypeDef * GPIOx, uint8_t PINx);
void PIN_IN_PU (GPIO_TypeDef * GPIOx,uint8_t PINx);

#endif
