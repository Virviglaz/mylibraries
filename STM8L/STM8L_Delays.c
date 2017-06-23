#include "STM8L_Delays.h"

void delays_init (u8 PRESCALER)
{
  CLK->PCKENR |= CLK_PCKENR_TIM4;
  TIM4->PSCR = PRESCALER&TIM4_PSCR_PSC;
  TIM4->EGR |= TIM4_EGR_UG;
  TIM4->CR1 = TIM4_CR1_CEN;
}

void delay_us (u8 us)
{
  TIM4->CNTR = 0;
  while (TIM4->CNTR < us);  
}

void delay_ms (u16 ms)
{
  ms = ms << 2;
  while (ms--) delay_us(250);
}

