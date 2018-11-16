#ifndef ALLWINNER_H3_H_
#define ALLWINNER_H3_H_

#include <stdint.h>

/* GPIO */
typedef struct
{
	volatile uint32_t CFG0;
	volatile uint32_t CFG1;
	volatile uint32_t CFG2;
	volatile uint32_t CFG3;
	volatile uint32_t DAT;
	volatile uint32_t DRV0;
	volatile uint32_t DRV1;
	volatile uint32_t PUL0;
	volatile uint32_t PUL1;
}GPIO_TypeDef;

typedef enum
{
	eINPUT = 0,
	eOUTPUT = 1,
	eFUCN1 = 2,
	eFUNC2 = 3,
	eRES1 = 4,
	eRES2 = 5,
	eINT = 6,
	eDISABLED = 7
}GPIO_FuncTypeDef;

#define GPIO_PERIPH_BASE	0x01C20800
#define GPIO_PERIPH_LEN		0x24
#define GPIOA               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 0)
#define GPIOC               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 1)
#define GPIOD               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 2)
#define GPIOE               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 3)
#define GPIOF               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 4)
#define GPIOG               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 5)
#define GPIOL               ((GPIO_TypeDef *) GPIO_PERIPH_BASE + GPIO_PERIPH_LEN * 6)

#define GPIO_Pin0			0x00000000
#define GPIO_Pin1			0x00000001
#define GPIO_Pin2			0x00000002
#define GPIO_Pin3			0x00000004
#define GPIO_Pin4			0x00000008
#define GPIO_Pin5			0x00000010
#define GPIO_Pin6			0x00000020
#define GPIO_Pin7			0x00000040
#define GPIO_Pin8			0x00000080
#define GPIO_Pin9			0x00000100
#define GPIO_Pin10			0x00000200
#define GPIO_Pin11			0x00000400
#define GPIO_Pin12			0x00000800
#define GPIO_Pin13			0x00001000
#define GPIO_Pin14			0x00002000
#define GPIO_Pin15			0x00004000
#define GPIO_Pin16			0x00008000
#define GPIO_Pin17			0x00010000
#define GPIO_Pin18			0x00020000
#define GPIO_Pin19			0x00040000
#define GPIO_Pin20			0x00080000
#define GPIO_Pin21			0x00100000
#define GPIO_Pin22			0x00200000
#define GPIO_PIN_MAX		23


#endif /* ALLWINNER_H3_H_ */
