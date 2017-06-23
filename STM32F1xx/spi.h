#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32_GPIO.h"	
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void SPI_Init_All (SPI_TypeDef * SPIx, u8 SPI_CLK_HIGH);
void SPI_init_all (SPI_TypeDef * SPIx);
void Init_RCC (SPI_TypeDef * SPIx);
void Init_GPIO(SPI_TypeDef * SPIx);
void Init_SPI_CLKL	(SPI_TypeDef * SPIx);
void Init_SPI_CLKH	(SPI_TypeDef * SPIx);
u8 SPI_WriteReg (SPI_TypeDef* SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadRegInc (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size, u8 inc);
u8 SPI_RW_Reg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_RW (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 * buf, u16 size);
