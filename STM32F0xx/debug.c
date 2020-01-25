#include <stdio.h>
#include "stm32f0xx.h"

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

#ifdef FreeRTOS
	#include "FreeRTOS.h"
	#include "semphr.h"
	static SemaphoreHandle_t xDebugSemaphore = NULL;
#endif

int fputc(int ch, FILE *f)
{
#ifdef FreeRTOS
	#include "FreeRTOS.h"
	#include "semphr.h"
	if (!xDebugSemaphore)
		xDebugSemaphore = xSemaphoreCreateMutex();
	xSemaphoreTake(xDebugSemaphore, 1000);
#endif
	
#ifdef DEBUG_USART1
	while (!(USART1->ISR & USART_ISR_TXE));
	USART1->TDR = ch;
#endif
	
#ifdef DEBUG_USART2
	while (!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = ch;
#endif
		
#ifdef DEBUG_USART3
	while (!(USART3->ISR & USART_ISR_TXE));
	USART3->TDR = ch;
#endif	

#ifdef FreeRTOS
	xSemaphoreGive(xDebugSemaphore);
#endif

#ifdef DEBUG_HD44780_SPI
#include "LCD_HD44780_SPI.h"
	extern LCD_SPI_StructTypeDef LCD_InitStruct;
	HD44780_SPI_PutChar(&LCD_InitStruct, ch);
#endif

#ifdef DEBUG_LCD1602
	#include "LCD.h"
	LCD_PutChar (ch);
#endif
  return(ch);
}
