#ifndef SPI_H
#define SPI_H

#include "stm32f10x.h"
#include <stdbool.h>

void SPIxInitAll (SPI_TypeDef * SPIx, uint16_t Prescaler, bool isIdleCLK_High);
void Init_SPI	(SPI_TypeDef * SPIx, uint16_t Prescaler, bool isIdleCLK_High);
u8 SPI_ReadByte(SPI_TypeDef * SPIx, u8 byte);
u8 SPI_WriteReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadReg  (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_ReadRegInc (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size, u8 inc);
u8 SPI_RW_Reg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size);
u8 SPI_RW (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 * buf, u16 size);

#endif
