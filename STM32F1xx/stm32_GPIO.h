#ifndef GPIO_H
#define GPIO_H

#include <stm32f10x.h>

#define PA0			GPIOA, GPIO_Pin_0
#define PA1			GPIOA, GPIO_Pin_1
#define PA2			GPIOA, GPIO_Pin_2
#define PA3			GPIOA, GPIO_Pin_3
#define PA4			GPIOA, GPIO_Pin_4
#define PA5			GPIOA, GPIO_Pin_5
#define PA6			GPIOA, GPIO_Pin_6
#define PA7			GPIOA, GPIO_Pin_7
#define PA8			GPIOA, GPIO_Pin_8
#define PA9			GPIOA, GPIO_Pin_9
#define PA10		GPIOA, GPIO_Pin_10
#define PA11		GPIOA, GPIO_Pin_11
#define PA12		GPIOA, GPIO_Pin_12
#define PA13		GPIOA, GPIO_Pin_13
#define PA14		GPIOA, GPIO_Pin_14
#define PA15		GPIOA, GPIO_Pin_15

#define PB0			GPIOB, GPIO_Pin_0
#define PB1			GPIOB, GPIO_Pin_1
#define PB2			GPIOB, GPIO_Pin_2
#define PB3			GPIOB, GPIO_Pin_3
#define PB4			GPIOB, GPIO_Pin_4
#define PB5			GPIOB, GPIO_Pin_5
#define PB6			GPIOB, GPIO_Pin_6
#define PB7			GPIOB, GPIO_Pin_7
#define PB8			GPIOB, GPIO_Pin_8
#define PB9			GPIOB, GPIO_Pin_9
#define PB10		GPIOB, GPIO_Pin_10
#define PB11		GPIOB, GPIO_Pin_11
#define PB12		GPIOB, GPIO_Pin_12
#define PB13		GPIOB, GPIO_Pin_13
#define PB14		GPIOB, GPIO_Pin_14
#define PB15		GPIOB, GPIO_Pin_15

#define PC0			GPIOC, GPIO_Pin_0
#define PC1			GPIOC, GPIO_Pin_1
#define PC2			GPIOC, GPIO_Pin_2
#define PC3			GPIOC, GPIO_Pin_3
#define PC4			GPIOC, GPIO_Pin_4
#define PC5			GPIOC, GPIO_Pin_5
#define PC6			GPIOC, GPIO_Pin_6
#define PC7			GPIOC, GPIO_Pin_7
#define PC8			GPIOC, GPIO_Pin_8
#define PC9			GPIOC, GPIO_Pin_9
#define PC10		GPIOC, GPIO_Pin_10
#define PC11		GPIOC, GPIO_Pin_11
#define PC12		GPIOC, GPIO_Pin_12
#define PC13		GPIOC, GPIO_Pin_13
#define PC14		GPIOC, GPIO_Pin_14
#define PC15		GPIOC, GPIO_Pin_15

#define PD0			GPIOD, GPIO_Pin_0
#define PD1			GPIOD, GPIO_Pin_1
#define PD2			GPIOD, GPIO_Pin_2
#define PD3			GPIOD, GPIO_Pin_3
#define PD4			GPIOD, GPIO_Pin_4
#define PD5			GPIOD, GPIO_Pin_5
#define PD6			GPIOD, GPIO_Pin_6
#define PD7			GPIOD, GPIO_Pin_7
#define PD8			GPIOD, GPIO_Pin_8
#define PD9			GPIOD, GPIO_Pin_9
#define PD10		GPIOD, GPIO_Pin_10
#define PD11		GPIOD, GPIO_Pin_11
#define PD12		GPIOD, GPIO_Pin_12
#define PD13		GPIOD, GPIO_Pin_13
#define PD14		GPIOD, GPIO_Pin_14
#define PD15		GPIOD, GPIO_Pin_15

#define PE0			GPIOE, GPIO_Pin_0
#define PE1			GPIOE, GPIO_Pin_1
#define PE2			GPIOE, GPIO_Pin_2
#define PE3			GPIOE, GPIO_Pin_3
#define PE4			GPIOE, GPIO_Pin_4
#define PE5			GPIOE, GPIO_Pin_5
#define PE6			GPIOE, GPIO_Pin_6
#define PE7			GPIOE, GPIO_Pin_7
#define PE8			GPIOE, GPIO_Pin_8
#define PE9			GPIOE, GPIO_Pin_9
#define PE10		GPIOE, GPIO_Pin_10
#define PE11		GPIOE, GPIO_Pin_11
#define PE12		GPIOE, GPIO_Pin_12
#define PE13		GPIOE, GPIO_Pin_13
#define PE14		GPIOE, GPIO_Pin_14
#define PE15		GPIOE, GPIO_Pin_15

#define PF0			GPIOF, GPIO_Pin_0
#define PF1			GPIOF, GPIO_Pin_1
#define PF2			GPIOF, GPIO_Pin_2
#define PF3			GPIOF, GPIO_Pin_3
#define PF4			GPIOF, GPIO_Pin_4
#define PF5			GPIOF, GPIO_Pin_5
#define PF6			GPIOF, GPIO_Pin_6
#define PF7			GPIOF, GPIO_Pin_7
#define PF8			GPIOF, GPIO_Pin_8
#define PF9			GPIOF, GPIO_Pin_9
#define PF10		GPIOF, GPIO_Pin_10
#define PF11		GPIOF, GPIO_Pin_11
#define PF12		GPIOF, GPIO_Pin_12
#define PF13		GPIOF, GPIO_Pin_13
#define PF14		GPIOF, GPIO_Pin_14
#define PF15		GPIOF, GPIO_Pin_15

typedef struct
{
	GPIO_TypeDef * GPIOx;
	u16 PINx;
}PIN_TypeDef;

void PIN_ON(GPIO_TypeDef * GPIOx,u16 PINx);
void PIN_OFF(GPIO_TypeDef * GPIOx,u16 PINx);
u16 PIN_SYG(GPIO_TypeDef * GPIOx, u16 PINx);
void PIN_SET(GPIO_TypeDef * GPIOx, u16 PINx, u8 State);
void PIN_IN (GPIO_TypeDef * GPIOx,u16 PINx);
void PIN_OUT_PP (GPIO_TypeDef * GPIOx,u16 PINx);
void PIN_OUT_OD (GPIO_TypeDef * GPIOx,u16 PINx);
void PIN_INV(GPIO_TypeDef * GPIOx, u16 PINx);
void PIN_IN_PU (GPIO_TypeDef * GPIOx, u16 PINx);
void PIN_IN_PD (GPIO_TypeDef * GPIOx, u16 PINx);
void PIN_OUT_AF_PP (GPIO_TypeDef * GPIOx,u16 PINx);
void PIN_OUT_AF_OD (GPIO_TypeDef * GPIOx,u16 PINx);
void gpio_init (GPIO_TypeDef * GPIOx, u16 PINx, GPIOSpeed_TypeDef GPIO_Speed, GPIOMode_TypeDef GPIO_Mode);

#endif
