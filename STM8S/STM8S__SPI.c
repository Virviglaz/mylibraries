#include "STM8S__SPI.h"
#include "STM8_GPIO.h"

void SPI_Init_ClockLOW (SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler)
{
  CLK->PCKENR1 |= CLK_PCKENR1_SPI;
  SPI->CR2 = SPI_CR2_SSI | SPI_CR2_SSM;
  SPI->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | SPI_BaudRatePrescaler;
}

void SPI_Init_ClockHigh (SPI_BaudRatePrescaler_TypeDef SPI_BaudRatePrescaler)
{
  CLK->PCKENR1 |= CLK_PCKENR1_SPI;
  SPI->CR2 = SPI_CR2_SSI | SPI_CR2_SSM;
  SPI->CR1 = SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_BaudRatePrescaler;
}

u8 SPI_ReadByte (u8 Data)
{
  u8 res;
	
    /* Loop while DR register in not empty */
  while (!(SPI->SR & SPI_SR_TXE));

  /* Send byte through the SPI peripheral */
  SPI->DR = Data;

  /* Wait to receive a byte */
  while (!(SPI->SR & SPI_SR_RXNE));
		
  res = (u8)SPI->DR;
		
  #ifdef DEBUG_SPI
    printf("SPI: WR 0x%.2X, RD 0x%.2X\n", byte, res);
  #endif
	
  /* Return the byte read from the SPI bus */
  return res;
}

void SPI_Select (GPIO_TypeDef * GPIOx, u8 PINx)
{
	#ifdef DEBUG_SPI
		printf("SPI: CS low\n");
	#endif
                
	PIN_OFF(GPIOx, PINx);
}

void SPI_Deselect (GPIO_TypeDef * GPIOx, u8 PINx)
{
	while (SPI->SR & SPI_SR_BSY);
	PIN_ON(GPIOx, PINx);
        
 	#ifdef DEBUG_SPI
		printf("SPI: CS high\n\n");
	#endif
}

u8 SPI_WriteReg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;
        
	SPI_Select(GPIOx, PINx);	
	result = SPI_ReadByte(reg);
        
	while(size--)
		SPI_ReadByte(* buf++);
	
	SPI_Deselect(GPIOx, PINx);
	return result;
}

u8 SPI_ReadReg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;

	SPI_Select(GPIOx, PINx);	
	result = SPI_ReadByte(reg);
	
	while(size--)
		* buf++ = SPI_ReadByte(reg);
	
	SPI_Deselect(GPIOx, PINx);
	return result;
}

u8 SPI_ReadRegInc (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size, s8 inc)
{
	u8 result;

	SPI_Select(GPIOx, PINx);	
	result = SPI_ReadByte(reg);
	
	while(size--)
		* buf++ = SPI_ReadByte(reg += inc);
	
	SPI_Deselect(GPIOx, PINx);
	return result;
}

u8 SPI_RW_Reg (GPIO_TypeDef * GPIOx, u8 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result, tmp;
	SPI_Select(GPIOx, PINx);
	result = SPI_ReadByte(reg);
	
	while(size--)
	{
		tmp = SPI_ReadByte(* buf);
		* buf++ = tmp;
	}
	
	SPI_Deselect(GPIOx, PINx);
	return result;
}
