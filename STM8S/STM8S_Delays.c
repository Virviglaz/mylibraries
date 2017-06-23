#include "STM8S_Delays.h"

void delays_init (TIM4_PRSC_TypeDef TIM4_PRSC)
{
  CLK->PCKENR1 |= CLK_PCKENR1_TIM4;
  TIM4->PSCR = TIM4_PRSC;
  TIM4->EGR |= TIM4_EGR_UG;
  TIM4->CR1 = TIM4_CR1_CEN;
}

void delay_us (unsigned short us)
{
  while (us > 100)
  {
    TIM4->CNTR = 0;
    while (TIM4->CNTR < 100);
    us -= 100;
  }
  TIM4->CNTR = 0;
  while (TIM4->CNTR < us);  
}

void delay_ms (unsigned short ms)
{
  ms <<= 2;
  while (ms--) 
	  delay_us(250);
}


