#ifndef SPI_H
#define SPI_H

#include <stdio.h>
#include <stdint.h>
#include "gpio.h"

typedef enum 
{
	SPIx_Unidirection = 0x00,
	SPIx_Bidirection =  SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE
}SPIx_DirectionTypeDef;

typedef enum
{
	SPIx_Div2   = 0x00,
	SPIx_Div4   = 0x08,
	SPIx_Div8   = 0x10,
	SPIx_Div16  = 0x18,
	SPIx_Div32  = 0x20,
	SPIx_Div64  = 0x28,
	SPIx_Div128 = 0x30,
	SPIx_Div256 = 0x38
}SPIx_BaudRateTypeDef;

typedef enum
{
	SPIx_ClockLow = 0,
	SPIx_ClockHigh = SPI_CR1_CPHA | SPI_CR1_CPOL
}SPIx_ClockTypeDef;

typedef struct
{
	ErrorStatus (* Init)	(SPI_TypeDef * SPIx, SPIx_DirectionTypeDef Dir, SPIx_BaudRateTypeDef DB, SPIx_ClockTypeDef Clk);
	void (* ClockConfig)	(SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk);
	uint8_t (* WriteReg)	(SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
	uint8_t (* ReadReg)		(SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
	uint8_t (* ReadRegInc)(SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size, uint8_t inc);
	uint8_t (* RW_Reg)		(SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
	uint8_t (* RW)				(SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t * buf, uint16_t size);
	
}SPI_ClassTypeDef;

ErrorStatus SPIx_Init (SPI_TypeDef * SPIx, SPIx_DirectionTypeDef Dir, SPIx_BaudRateTypeDef DB, SPIx_ClockTypeDef Clk);
void SPIx_ClockConfigure (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk);
uint8_t SPIx_WriteReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
uint8_t SPIx_ReadReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
uint8_t SPIx_ReadRegInc (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size, uint8_t inc);
uint8_t SPIx_RW_Reg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size);
uint8_t SPIx_RW (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t * buf, uint16_t size);

static const SPI_ClassTypeDef STM32_SPI = {SPIx_Init, SPIx_ClockConfigure, SPIx_WriteReg, SPIx_ReadReg, SPIx_ReadRegInc, SPIx_RW_Reg, SPIx_RW};
#endif
