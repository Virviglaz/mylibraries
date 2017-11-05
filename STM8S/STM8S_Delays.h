#ifndef STM8S_DELAYS_H
#define STM8S_DELAYS_H

#define _STDINT
#include "stm8s.h"

typedef enum
{
  TIM4_PRSC_1  = ((uint8_t)0x00),
  TIM4_PRSC_2    = ((uint8_t)0x01),
  TIM4_PRSC_4    = ((uint8_t)0x02),
  TIM4_PRSC_8     = ((uint8_t)0x03),
  TIM4_PRSC_16   = ((uint8_t)0x04),
  TIM4_PRSC_32     = ((uint8_t)0x05),
  TIM4_PRSC_64    = ((uint8_t)0x06),
  TIM4_PRSC_128   = ((uint8_t)0x07)
} TIM4_PRSC_TypeDef;

void delays_init (TIM4_PRSC_TypeDef TIM4_PRSC);
void delay_us (unsigned short us);
void delay_ms (unsigned short ms);
#endif
