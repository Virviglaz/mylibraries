#include "STM8_ADC.h"

void ADC_Init (unsigned char Channel)
{
  CLK->PCKENR2 |= CLK_PCKENR2_ADC;
  ADC1->TDRL = Channel;
  ADC1->CR1 = ADC1_PRESSEL_FCPU_D18;
  ADC1->CR2 = ADC1_ALIGN_RIGHT;
  ADC1->CR1 |= ADC1_CR1_ADON;
}

unsigned int ADC_Read (unsigned char Channel)
{
  unsigned int Result;
  ADC1->CSR = Channel;
  ADC1->CR1 |= ADC1_CR1_ADON;
  ADC1->CR1 |= ADC1_CR1_ADON;
  while (!(ADC1->CSR & ADC1_CSR_EOC));
  Result = ADC1->DRL;
  Result |= ADC1->DRH << 8;
  
  return Result;  
}