#ifndef STM8_SPI_H
#define STM8_SPI_H

#include "STM8_GPIO.h"
#include "stm8s.h"

void SPI_Init_ClockLOW (SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler);
void SPI_Init_ClockHigh (SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler);
uint8_t SPI_ReadByte   (uint8_t Data);
void SPI_Select   (GPIO_TypeDef * GPIOx, uint8_t PINx);
void SPI_Deselect (GPIO_TypeDef * GPIOx, uint8_t PINx);
uint8_t SPI_WriteReg (GPIO_TypeDef * GPIOx, uint8_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
uint8_t SPI_ReadReg  (GPIO_TypeDef * GPIOx, uint8_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
uint8_t SPI_RW_Reg (GPIO_TypeDef * GPIOx, uint8_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
uint8_t SPI_ReadRegInc (GPIO_TypeDef * GPIOx, uint8_t PINx, uint8_t reg, uint8_t * buf, uint16_t size, s8 inc);

#endif
