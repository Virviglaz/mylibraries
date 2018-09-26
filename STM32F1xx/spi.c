#include "spi.h"
#include <stdio.h>

static void Init_RCC (SPI_TypeDef * SPIx);
static void Init_GPIO(SPI_TypeDef * SPIx);

void SPIxInitAll (SPI_TypeDef * SPIx, uint16_t Prescaler, bool isIdleCLK_High)
{
	Init_RCC(SPIx);
	Init_GPIO(SPIx);
	Init_SPI(SPIx, Prescaler, isIdleCLK_High);
}

static void Init_RCC (SPI_TypeDef * SPIx)
{
	if (SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	}
	
	if (SPIx == SPI2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	}
	
	if (SPIx == SPI3) //not supported yet
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	}
}

static void Init_GPIO(SPI_TypeDef * SPIx)
{
	GPIO_InitTypeDef GPIO_InitStruct = {.GPIO_Mode = GPIO_Mode_AF_PP, .GPIO_Speed = GPIO_Speed_50MHz };
	
	if (SPIx == SPI1)	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;		 //alternative func for SPI1

	if (SPIx == SPI2)	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	 //alternative func for SPI2

	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Init_SPI	(SPI_TypeDef * SPIx, uint16_t Prescaler, bool isIdleCLK_High)
{
	SPI_InitTypeDef SPI_InitStruct = 
	{	
		.SPI_Direction = SPI_Direction_2Lines_FullDuplex,
		.SPI_Mode = SPI_Mode_Master,
		.SPI_DataSize = SPI_DataSize_8b,
		.SPI_CPOL = isIdleCLK_High ? SPI_CPOL_High :SPI_CPOL_Low,
		.SPI_CPHA = isIdleCLK_High ? SPI_CPHA_2Edge : SPI_CPHA_1Edge,
		.SPI_NSS = SPI_NSS_Soft,
		.SPI_BaudRatePrescaler = Prescaler,
		.SPI_FirstBit = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial = 7,
	};
	
	SPI_Init(SPIx, &SPI_InitStruct);
	SPI_Cmd(SPIx, ENABLE);
}


__inline static u8 SPI_ReadByte(SPI_TypeDef * SPIx, u8 byte)
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

__inline static void SPI_Select (GPIO_TypeDef * GPIOx, u16 PINx)
{
	PIN_OFF(GPIOx, PINx);

	#ifdef DEBUG_SPI
		printf("SPI: CS low\n");
	#endif	
}

__inline static void SPI_Deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx,u16 PINx)
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
