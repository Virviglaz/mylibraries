#include "spi.h"
#include <stdio.h>

#ifdef FreeRTOS
		#include "FreeRTOS.h"
		#include "semphr.h"
		SemaphoreHandle_t xSPISemaphore;
#endif

/* Local functions */
void SPIx_Select (GPIO_TypeDef * GPIOx, u16 PINx);
void SPIx_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx);
u8 SPIx_ReadByte(SPI_TypeDef * SPIx, u8 byte);

ErrorStatus SPIx_Init (SPI_TypeDef * SPIx, SPIx_DirectionTypeDef Dir, SPIx_BaudRateTypeDef DB, SPIx_ClockTypeDef Clk)
{
	#ifdef FreeRTOS
		xSPISemaphore = xSemaphoreCreateMutex(); 
		if (xSPISemaphore == NULL) return ERROR;
	#endif
	
	SPIx->CR1 = Dir | DB | SPI_CR1_MSTR | SPI_CR1_SPE | Clk;
	
	/* Check that perepherial clock enabled before init */
	if (SPIx->CR1 == 0) return ERROR;
	return SUCCESS;
}

void SPIx_Select (GPIO_TypeDef * GPIOx, u16 PINx)
{
	#ifdef DEBUG_SPI
		printf("SPI: CS low\n");
	#endif
	
	PIN_OFF(GPIOx, PINx);
}

void SPIx_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx)
{
	while (SPIx->SR & SPI_SR_BSY);
	PIN_ON(GPIOx, PINx);
	
	#ifdef DEBUG_SPI
		printf("SPI: CS high\n\n");
	#endif
}

u8 SPIx_ReadByte(SPI_TypeDef * SPIx, u8 byte)
{
	u8 res;
	
	if (SPIx->SR & SPI_SR_OVR)
		SPIx->DR;
	
    /* Loop while DR register in not empty */
  while (!(SPIx->SR & SPI_SR_TXE));

  /* Send byte through the SPI peripheral */
  SPIx->DR = byte;

  /* Wait to receive a byte */
	while (!(SPIx->SR & SPI_SR_RXNE));
		
	res = (u8)SPIx->DR;
		
	#ifdef DEBUG_SPI
		printf("SPI: WR 0x%.2X, RD 0x%.2X\n", byte, res);
	#endif
	
  /* Return the byte read from the SPI bus */
  return res;
}

u8 SPIx_WriteReg (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;
	
	#ifdef FreeRTOS
		xSemaphoreTake(xSPISemaphore, 1000);
	#endif
	
	#ifdef DEBUG_SPI
		printf("SPI: Write REG 0x%.2X size 0x%.4X\n", reg, size);
	#endif
	
	/* Configure clock idle mode */
	SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_SPE);
	SPIx->CR1 |= Clk  | SPI_CR1_SPE;
	
	SPIx_Select(GPIOx, PINx);
	
	result = SPIx_ReadByte(SPIx, reg);
	while(size--)
		SPIx_ReadByte(SPIx, * buf++);
	
	SPIx_Deselect(SPIx, GPIOx, PINx);
	
	#ifdef FreeRTOS	
		xSemaphoreGive(xSPISemaphore);
	#endif
	return result;
}

u8 SPIx_ReadReg (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;
	
	#ifdef FreeRTOS
		xSemaphoreTake(xSPISemaphore, 1000);
	#endif
	
	#ifdef DEBUG_SPI
		printf("SPI: Read REG 0x%.2X size 0x%.4X\n", reg, size);
	#endif

	/* Configure clock idle mode */
	SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_SPE);
	SPIx->CR1 |= Clk  | SPI_CR1_SPE;
		
	SPIx_Select(GPIOx, PINx);	
	result = SPIx_ReadByte(SPIx, reg);
	
	while(size--)
		* buf++ = SPIx_ReadByte(SPIx, ++reg);
	
	SPIx_Deselect(SPIx, GPIOx, PINx);

	#ifdef FreeRTOS	
		xSemaphoreGive(xSPISemaphore);
	#endif
	
	return result;
}

u8 SPIx_ReadRegInc (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size, u8 inc)
{
	u8 result;

	#ifdef FreeRTOS
		xSemaphoreTake(xSPISemaphore, 1000);
	#endif
	
	#ifdef DEBUG_SPI
		printf("SPI: Read REG with auto increase 0x%.2X size 0x%.4X\n", reg, size);
	#endif
	
	/* Configure clock idle mode */
	SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_SPE);
	SPIx->CR1 |= Clk  | SPI_CR1_SPE;
	
	SPIx_Select(GPIOx, PINx);	
	result = SPIx_ReadByte(SPIx, reg);
	
	while(size--)
	{
		reg += inc;
		* buf++ = SPIx_ReadByte(SPIx, reg);
	}
	
	SPIx_Deselect(SPIx, GPIOx, PINx);

	#ifdef FreeRTOS	
		xSemaphoreGive(xSPISemaphore);
	#endif
	
	return result;
}

u8 SPIx_RW_Reg (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result, tmp;

	#ifdef FreeRTOS
		xSemaphoreTake(xSPISemaphore, 1000);
	#endif
	
	/* Configure clock idle mode */
	SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_SPE);
	SPIx->CR1 |= Clk  | SPI_CR1_SPE;

	SPIx_Select(GPIOx, PINx);
	result = SPIx_ReadByte(SPIx, reg);
	
	while(size--)
	{
		tmp = SPIx_ReadByte(SPIx, * buf);
		* buf++ = tmp;
	}
	
	SPIx_Deselect(SPIx, GPIOx, PINx);

	#ifdef FreeRTOS	
		xSemaphoreGive(xSPISemaphore);
	#endif
	
	return result;	
}

u8 SPIx_RW (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk, GPIO_TypeDef * GPIOx, u16 PINx, u8 * buf, u16 size)
{
	u8 res, tmp;

	#ifdef FreeRTOS
		xSemaphoreTake(xSPISemaphore, 1000);
	#endif
	
	#ifdef DEBUG_SPI
		printf("SPI: Read and Write array size 0x%.4X\n", size);
	#endif
	
	/* Configure clock idle mode */
	SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_SPE);
	SPIx->CR1 |= Clk  | SPI_CR1_SPE;
	
	SPIx_Select(GPIOx, PINx);
	res = SPIx_ReadByte(SPIx, * buf);

	while(size--)
	{
		tmp = SPIx_ReadByte(SPIx, * buf);
		* buf++ = tmp;
	}

	SPIx_Deselect(SPIx, GPIOx, PINx);

	#ifdef FreeRTOS	
		xSemaphoreGive(xSPISemaphore);
	#endif
	
	return res;
}
