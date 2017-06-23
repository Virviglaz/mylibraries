#include "spi.h"

#define SPIx_DR_8b          	(*(__IO uint8_t *)((uint32_t)SPIx + 0x0C))


/* Local functions */
void SPIx_Select (GPIO_TypeDef * GPIOx, uint16_t PINx);
void SPIx_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx);
uint8_t SPIx_ReadByte(SPI_TypeDef * SPIx, uint8_t byte);

ErrorStatus SPIx_Init (SPI_TypeDef * SPIx, SPIx_DirectionTypeDef Dir, SPIx_BaudRateTypeDef DB, SPIx_ClockTypeDef Clk)
{
	SPIx->CR1 &= ~SPI_CR1_SPE;
	SPIx->CR1 = Dir | DB | SPI_CR1_MSTR | Clk | SPI_CR1_SSI | SPI_CR1_SSM;
	SPIx->CR2 = 0x700;   //  8 bit
	SPIx->CR2 |= SPI_CR2_FRXTH;
	SPIx->CR1 |= SPI_CR1_SPE;
	
	/* Check that perepherial clock enabled before init */
	if (SPIx->CR1 == 0) return ERROR;
	return SUCCESS;
}

void SPIx_ClockConfigure (SPI_TypeDef * SPIx, SPIx_ClockTypeDef Clk)
{	
	/* Configure clock idle mode */
	SPIx->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_SPE);
	SPIx->CR1 |= Clk  | SPI_CR1_SPE;	
}

void SPIx_Select (GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	#ifdef DEBUG_SPI
		printf("SPI: CS low\n");
	#endif
	
	PIN_OFF(GPIOx, PINx);
}

void SPIx_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx)
{
	while (SPIx->SR & SPI_SR_BSY);
	PIN_ON(GPIOx, PINx);
	
	#ifdef DEBUG_SPI
		printf("SPI: CS high\n\n");
	#endif
}

uint8_t SPIx_ReadByte(SPI_TypeDef * SPIx, uint8_t byte)
{
	uint8_t res;
	
    /* Loop while DR register in not empty */
  while (!(SPIx->SR & SPI_SR_TXE));

  /* Send byte through the SPI peripheral */
	SPIx_DR_8b = byte;
	
  /* Wait to receive a byte */
	while (!(SPIx->SR & SPI_SR_RXNE));
		
	res = SPIx_DR_8b;
	
	#ifdef DEBUG_SPI
		printf("SPI: WR 0x%.2X, RD 0x%.2X\n", byte, res);
	#endif
	
  /* Return the byte read from the SPI bus */
  return res;
}

uint8_t SPIx_WriteReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size)
{
	uint8_t result;
	
	#ifdef DEBUG_SPI
		printf("SPI: Write REG 0x%.2X size 0x%.4X\n", reg, size);
	#endif
	
	SPIx_Select(GPIOx, PINx);
	
	result = SPIx_ReadByte(SPIx, reg);
	while(size--)
		SPIx_ReadByte(SPIx, * buf++);
	
	SPIx_Deselect(SPIx, GPIOx, PINx);
	
	return result;
}

uint8_t SPIx_ReadReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size)
{
	uint8_t result;
	
	#ifdef DEBUG_SPI
		printf("SPI: Read REG 0x%.2X size 0x%.4X\n", reg, size);
	#endif

	SPIx_Select(GPIOx, PINx);	
	result = SPIx_ReadByte(SPIx, reg);
	
	while(size--)
		* buf++ = SPIx_ReadByte(SPIx, 0x00);
	
	SPIx_Deselect(SPIx, GPIOx, PINx);

	return result;
}

uint8_t SPIx_ReadRegInc (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size, uint8_t inc)
{
	uint8_t result;

	#ifdef DEBUG_SPI
		printf("SPI: Read REG with auto increase 0x%.2X size 0x%.4X\n", reg, size);
	#endif

	SPIx_Select(GPIOx, PINx);	
	result = SPIx_ReadByte(SPIx, reg);
	
	while(size--)
	{
		reg += inc;
		* buf++ = SPIx_ReadByte(SPIx, reg);
	}
	
	SPIx_Deselect(SPIx, GPIOx, PINx);
	
	return result;
}

uint8_t SPIx_RW_Reg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t reg, uint8_t * buf, uint16_t size)
{
	uint8_t result, tmp;

	SPIx_Select(GPIOx, PINx);
	result = SPIx_ReadByte(SPIx, reg);
	
	while(size--)
	{
		tmp = SPIx_ReadByte(SPIx, * buf);
		* buf++ = tmp;
	}
	
	SPIx_Deselect(SPIx, GPIOx, PINx);

	return result;	
}

uint8_t SPIx_RW (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, uint16_t PINx, uint8_t * buf, uint16_t size)
{
	uint8_t res, tmp;

	#ifdef DEBUG_SPI
		printf("SPI: Read and Write array size 0x%.4X\n", size);
	#endif

	SPIx_Select(GPIOx, PINx);
	res = SPIx_ReadByte(SPIx, * buf);

	while(size--)
	{
		tmp = SPIx_ReadByte(SPIx, * buf);
		* buf++ = tmp;
	}

	SPIx_Deselect(SPIx, GPIOx, PINx);

	return res;
}
