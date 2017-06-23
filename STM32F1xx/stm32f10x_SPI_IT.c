#include "stm32f10x_SPI_IT.h"
#include "stm32_GPIO.h"	

SPI_IT_InitStructTypeDef * USE_SPI_IT_Struct;
void SPI_IT_RW_Handler (void);

void SPI_IT_Init (SPI_IT_InitStructTypeDef * SPI_IT_InitStruct)
{
	/* Base configuration */
	SPI_IT_InitStruct->SPIx->CR1 = SPI_CR1_MSTR | SPI_IT_InitStruct->SPI_BaudRate | SPI_IT_InitStruct->SPI_CLK_Idle;
	if	(SPI_IT_InitStruct->SPI_Enable)
		SPI_IT_InitStruct->SPIx->CR1 |= SPI_CR1_SPE;
	
	/* Interrupt init */
	NVIC_EnableIRQ(SPI_IT_InitStruct->InterruptVector);
}

char SPI_IT_RW (SPI_IT_InitStructTypeDef * SPI_IT_InitStruct, GPIO_TypeDef * GPIOx, u16 PINx, char reg, char * buffer, unsigned short size)
{
	PIN_OFF(GPIOx, PINx);
	if (size)
	{
		SPI_IT_InitStruct->i = 0;
		SPI_IT_InitStruct->size = size;
		SPI_IT_InitStruct->buffer = buffer;
		SPI_IT_InitStruct->SPI_IT_Handler = SPI_IT_RW_Handler;
		USE_SPI_IT_Struct = SPI_IT_InitStruct;
		SPI_IT_InitStruct->SPIx->CR2 = SPI_CR2_TXEIE | SPI_CR2_RXNEIE;
		SPI_IT_InitStruct->SPIx->DR = reg;
		while (SPI_IT_InitStruct->TimeOut && SPI_IT_InitStruct->SPIx->SR & SPI_SR_BSY);
	}
	else
	{
		SPI_IT_InitStruct->SPIx->DR = reg;
		while (SPI_IT_InitStruct->SPIx->SR & SPI_SR_BSY);
		SPI_IT_InitStruct->result = SPI_IT_InitStruct->SPIx->DR;
	}
	//SPI_IT_InitStruct->SPIx->CR2 = 0;
	PIN_ON(GPIOx, PINx);
	return SPI_IT_InitStruct->result;
}

void SPI_IT_RW_Handler (void)
{
	unsigned short Status = USE_SPI_IT_Struct->SPIx->SR;
	if (Status & SPI_SR_TXE)
	{
		if (USE_SPI_IT_Struct->i < USE_SPI_IT_Struct->size)		
			USE_SPI_IT_Struct->SPIx->DR = USE_SPI_IT_Struct->buffer[USE_SPI_IT_Struct->i];			
		else		
			USE_SPI_IT_Struct->SPIx->CR2 &= SPI_CR2_TXEIE;		
	}
	
	if (Status & SPI_SR_RXNE)
	{
		if (USE_SPI_IT_Struct->i == 0) //status register read	
		{
			USE_SPI_IT_Struct->result = USE_SPI_IT_Struct->SPIx->DR;
			USE_SPI_IT_Struct->i = 1;
		}
		else
		{
			if (USE_SPI_IT_Struct->buffer && USE_SPI_IT_Struct->i < USE_SPI_IT_Struct->size)	//read sequence
				USE_SPI_IT_Struct->buffer[USE_SPI_IT_Struct->i++ - 1] = USE_SPI_IT_Struct->SPIx->DR;
			else														//one byte read only
				USE_SPI_IT_Struct->SPIx->CR2 &= SPI_CR2_RXNEIE;
		}
	}	
}


