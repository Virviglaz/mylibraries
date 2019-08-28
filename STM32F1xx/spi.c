#include "spi.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#ifdef DEBUG_SPI
#include <stdio.h>
#endif

#define PIN_OFF 		GPIO_ResetBits
#define PIN_ON			GPIO_SetBits

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
	} else if (SPIx == SPI2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	} else if (SPIx == SPI3) //not supported yet
	{
		RCC_APB2PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
	}
}

static void Init_GPIO(SPI_TypeDef * SPIx)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	if (SPIx == SPI1)	{
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;		 //alternative func for SPI1
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	else if (SPIx == SPI2) {
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	 //alternative func for SPI2
		GPIO_Init(GPIOB, &GPIO_InitStruct);
	}
}

void Init_SPI	(SPI_TypeDef * SPIx, uint16_t Prescaler, bool isIdleCLK_High)
{
	SPI_InitTypeDef SPI_InitStruct;

	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_BaudRatePrescaler = Prescaler;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = Prescaler;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_InitStruct.SPI_CPOL = isIdleCLK_High ? SPI_CPOL_High : SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = isIdleCLK_High ? SPI_CPHA_2Edge : SPI_CPHA_1Edge;

	SPI_Init(SPIx, &SPI_InitStruct);
	SPI_Cmd(SPIx, ENABLE);
}

__inline static u8 _spi_readByte(SPI_TypeDef * SPIx, u8 byte)
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

__inline static void _spi_select (GPIO_TypeDef * GPIOx, u16 PINx)
{
	PIN_OFF(GPIOx, PINx);

	#ifdef DEBUG_SPI
		printf("SPI: CS low\n");
	#endif
}

__inline static void _spi_deselect (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx,u16 PINx)
{
	while (SPIx->SR & SPI_SR_BSY);
	PIN_ON(GPIOx, PINx);

	#ifdef DEBUG_SPI
		printf("SPI: CS high\n\n");
	#endif
}

u8 SPI_ReadByte(SPI_TypeDef * SPIx, u8 byte)
{
	return _spi_readByte(SPIx, byte);
}

u8 SPI_WriteReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;
	_spi_select(GPIOx, PINx);

	result = _spi_readByte(SPIx, reg);
	while(size--)
		_spi_readByte(SPIx, * buf++);

	_spi_deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_ReadReg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result;

	_spi_select(GPIOx, PINx);
	result = _spi_readByte(SPIx, reg);

	while(size--)
		* buf++ = _spi_readByte(SPIx, ++reg);

	_spi_deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_ReadRegInc (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size, u8 inc)
{
	u8 result;

	_spi_select(GPIOx, PINx);
	result = _spi_readByte(SPIx, reg);

	while(size--)
	{
		reg += inc;
		* buf++ = _spi_readByte(SPIx, reg);
	}

	_spi_deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_RW_Reg (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 reg, u8 * buf, u16 size)
{
	u8 result, tmp;
	_spi_select(GPIOx, PINx);
	result = _spi_readByte(SPIx, reg);

	while(size--)
	{
		tmp = _spi_readByte(SPIx, * buf);
		* buf++ = tmp;
	}

	_spi_deselect(SPIx, GPIOx, PINx);
	return result;
}

u8 SPI_RW (SPI_TypeDef * SPIx, GPIO_TypeDef * GPIOx, u16 PINx, u8 * buf, u16 size)
{
	u8 res, tmp;
	_spi_select(GPIOx, PINx);
	res = _spi_readByte(SPIx, * buf);

	while(size--)
	{
		tmp = _spi_readByte(SPIx, * buf);
		* buf++ = tmp;
	}
	//buf = SPI_ReadByte(SPIx, 0);
	_spi_deselect(SPIx, GPIOx, PINx);
	return res;
}
