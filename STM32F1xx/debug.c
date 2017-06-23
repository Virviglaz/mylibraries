#include <stdio.h>
#include <stm32f10x.h>

struct __FILE { int handle; /* Add whatever needed */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
#ifdef DEBUG_USART1
		while (!(USART1->SR & USART_SR_TXE));
		USART1->DR = ch;
#endif
	
#ifdef DEBUG_USART2
		while (!(USART2->SR & USART_SR_TXE));
		USART2->DR = ch;
#endif
	
#ifdef DEBUG_USART3
		while (!(USART3->SR & USART_SR_TXE));
		USART3->DR = ch;
#endif	

#ifdef DEBUG_HD44780_SPI
	#include "LCD_HD44780_SPI.h"
	extern LCD_SPI_StructTypeDef LCD_InitStruct;
	HD44780_SPI_PutChar(&LCD_InitStruct, ch);
#endif
  return(ch);
}
