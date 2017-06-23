#include <stdio.h>

size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
   size_t nChars = 0;
  
   while(Bufsize--)
   {
      /* Цикл, пока не завершится передача */
      #ifdef DEBUG_USART
        while (!(USART->SR & USART_SR_TXE));
        USART->DR = *Buf++;
      #endif
        
      #ifdef DEBUG_HD44780_SPI
	#include "LCD_HD44780_SPI.h"
	extern LCD_SPI_StructTypeDef LCD_InitStruct;
	HD44780_SPI_PutChar(&LCD_InitStruct, *Buf++);  
      #endif
      ++nChars;
   }
   return nChars;
}