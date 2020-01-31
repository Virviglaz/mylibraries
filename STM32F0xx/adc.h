#ifndef ADC_H
#define ADC_H

#include "stm32f0xx.h"                  // Device header

uint32_t ADCx_GetCalibrationFactor (ADC_TypeDef * ADCx);
double GetIntTemp (void);
double GetIntVDD (void);
uint16_t ADC_GenericRead (ADC_TypeDef * ADCx, uint32_t ADC_Channel);
#endif
