#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc1_init (uint8_t channel);
uint16_t ADC_Get_Result (uint8_t channel);

#endif
