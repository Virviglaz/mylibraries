#ifndef ADC_H
#define ADC_H

#include "stm32f0xx.h"                  // Device header
#include <stdint.h>

#define TS_CAL1									*(__IO int16_t*)0x1FFFF7B8
#define TS_CAL2									*(__IO int16_t*)0x1FFFF7C2
#define VREFINT_CAL							*(__IO int16_t*)0x1FFFF7BA

uint32_t ADCx_GetCalibrationFactor (ADC_TypeDef * ADCx);
float GetIntTemp (void);
float GetIntVDD (void);
uint16_t ADC_GenericRead (ADC_TypeDef * ADCx, uint32_t ADC_Channel);
#endif
