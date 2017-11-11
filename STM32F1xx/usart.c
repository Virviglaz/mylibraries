#include <stdio.h>
#include "stm32f10x_usart.h"

ErrorStatus USART_STD_Init (USART_TypeDef* USARTx, uint32_t USART_BaudRate)
{
	USART_InitTypeDef USART_InitStruct;
	
	/* Perepherial clock enabled */
	if (USARTx == USART1)
		RCC->APB2ENR |= RCC_APB2Periph_USART1;
	else if (USARTx == USART2)
		RCC->APB1ENR |= RCC_APB1Periph_USART2;
	else if (USARTx == USART3)
		RCC->APB1ENR |= RCC_APB1Periph_USART2;
	else
		return ERROR;
	
	/*switch (USARTx)
	{
		case USART1: RCC->APB2ENR |= RCC_APB2Periph_USART1; break;
		case USART2: RCC->APB2ENR |= RCC_APB1Periph_USART2; break;
		case USART3: RCC->APB2ENR |= RCC_APB1Periph_USART3; break;
	}*/
	
	
	/* Configure USART with standart settings */
	USART_InitStruct.USART_BaudRate = USART_BaudRate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USARTx, &USART_InitStruct);
	
	/* Enable USART */
	USARTx->CR1 |= USART_CR1_UE;
	
	return SUCCESS;
}

void USART_STD_PrintArrayValueHEX (USART_TypeDef* USARTx, char * buf, unsigned short size)
{
	unsigned short cnt = 0;
	printf("Array size: %u\n", size);
	while(size--)
		printf("Value[%.3u]: 0x%.2X\n", cnt++, *buf++);
}

void USART_STD_PrintRegArrayValueHEX (USART_TypeDef* USARTx, char reg, char * buf, unsigned short size)
{
	unsigned short cnt = 0;
	printf("Array size: %u in REG 0x%.2X\n", size, reg);
	while(size--)
		printf("Value[%.3u]: 0x%.2X\n", cnt++, *buf++);
}
