#include "spi.h"
#include <stdio.h>

u8 SPI_ReadByte(SPI_TypeDef* SPIx, u8 byte);	
void SPI_Select (GPIO_TypeDef * GPIOx,u16 PINx);
void SPI_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx);

void SPI_Init_All (SPI_TypeDef * SPIx, u8 SPI_CLK_HIGH)
{
	Init_RCC(SPIx);
	Init_GPIO(SPIx);
	if (SPI_CLK_HIGH)
		Init_SPI_CLKH(SPIx);
	else
		Init_SPI_CLKL(SPIx);
}

/* Support old versions */
void SPI_init_all (SPI_TypeDef * SPIx)
{
	Init_RCC(SPIx);
	Init_GPIO(SPIx);
	Init_SPI_CLKL(SPIx);
}

void Init_RCC (SPI_TypeDef* SPIx)
{
	if (SPIx == SPI1)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		}
	if (SPIx == SPI2)
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		}
	if (SPIx == SPI3){RCC_APB2PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);}
}

void Init_GPIO(SPI_TypeDef * SPIx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if (SPIx == SPI1)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;		 //alternative func for SPI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	if (SPIx == SPI2)
	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	 //alternative func for SPI
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void Init_SPI_CLKL	(SPI_TypeDef * SPIx)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStruct);

	SPI_Cmd(SPIx, ENABLE);
}


void Init_SPI_CLKH	(SPI_TypeDef * SPIx)
{
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStruct);

	SPI_Cmd(SPIx, ENABLE);
}

u8 SPI_ReadByte(SPI_TypeDef * SPIx, u8 byte)
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

void SPI_Select (GPIO_TypeDef * GPIOx, u16 PINx)
{
	#ifdef DEBUG_SPI
		printf("SPI: CS low\n");
	#endif
	PIN_OFF(GPIOx, PINx);
}

void SPI_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx,u16 PINx)
{
	while (SPIx->SR & SPI_SR_BSY);
	PIN_ON(GPIOx, PINx);
	#ifdef DEBUG_SPI
		printf("SPI: CS high\n\n");
	#endif
}

u8 SPI_WriteReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;
	SPI_Select(GPIOx, PINx);
	
	result = SPI_ReadByte(SPIx, reg);
	while(size--)
		SPI_ReadByte(SPIx, * buf++);
	
	SPI_Deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_ReadReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;
	
	SPI_Select(GPIOx, PINx);	
	result = SPI_ReadByte(SPIx, reg);
	
	while(size--)
		* buf++ = SPI_ReadByte(SPIx, ++reg);
	
	SPI_Deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_ReadRegInc (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size, u8 inc)
{
	u8 result;
	
	SPI_Select(GPIOx, PINx);	
	result = SPI_ReadByte(SPIx, reg);
	
	while(size--)
	{
		reg += inc;
		* buf++ = SPI_ReadByte(SPIx, reg);
	}
	
	SPI_Deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_RW_Reg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result, tmp;
	SPI_Select(GPIOx, PINx);
	result = SPI_ReadByte(SPIx, reg);
	
	while(size--)
	{
		tmp = SPI_ReadByte(SPIx, * buf);
		* buf++ = tmp;
	}
	
	SPI_Deselect(SPIx, GPIOx, PINx);
	return result;	
}

u8 SPI_RW (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 * buf, u16 size)
{
	u8 res, tmp; 
	SPI_Select(GPIOx, PINx);
	res = SPI_ReadByte(SPIx, * buf);

	while(size--)
	{
		tmp = SPI_ReadByte(SPIx, * buf);
		* buf++ = tmp;
	}
	//buf = SPI_ReadByte(SPIx, 0);
	SPI_Deselect(SPIx, GPIOx, PINx);
	return res;
}
