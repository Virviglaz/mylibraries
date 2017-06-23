#include "STM8L_SPI.h"
#include "STM8_GPIO.h"
u8 debug;
void SPI_Init_ClockLOW (void)
{
  CLK->PCKENR |= CLK_PCKENR_SPI;
  SPI->CR2 = SPI_CR2_SSI|SPI_CR2_SSM;
  SPI->CR1 = SPI_CR1_SPE|SPI_CR1_MSTR;
}

void SPI_Init_ClockHigh (void)
{
  CLK->PCKENR |= CLK_PCKENR_SPI;
  SPI->CR2 = SPI_CR2_SSI|SPI_CR2_SSM;
  SPI->CR1 = SPI_CR1_SPE|SPI_CR1_MSTR|SPI_CR1_CPOL|SPI_CR1_CPHA;
}

u8 SPI_ReadByte (u8 Data)
{
  /* Loop while DR register in not emplty */ 
  while (!(SPI->SR&SPI_SR_TXE));
  
  /* Send byte through the SPI peripheral */
  SPI->DR = Data; 
  
  /* Wait to receive a byte */
  while (!(SPI->SR&SPI_SR_RXNE));
  
  /* Return the byte read from the SPI bus */
  return SPI->DR;
}

void SPI_SendByte (u8 Data)
{
  SPI_ReadByte(Data);
}

void SPI_Select (GPIO_TypeDef * GPIOx,u8 PINx)
{
	PIN_OFF(GPIOx, PINx);
}

void SPI_Deselect (GPIO_TypeDef * GPIOx,u8 PINx)
{
	while (SPI->SR & SPI_SR_BSY);
	PIN_ON(GPIOx, PINx);
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