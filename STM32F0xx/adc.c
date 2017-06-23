#include "adc.h"

uint32_t ADCx_GetCalibrationFactor (ADC_TypeDef * ADCx)
{
	ADCx->CR |= ADC_CR_ADCAL;
	while(ADCx->CR & ADC_CR_ADCAL);
	
	return ADCx->DR;
}

float GetIntTemp (void)
{
	float res;
	
	ADC1->CHSELR = ADC_CHSELR_CHSEL16;
	ADC1->SMPR = ADC_SMPR_SMP;
	ADC->CCR |= ADC_CCR_TSEN;
	ADC1->CR = ADC_CR_ADSTART | ADC_CR_ADEN;
	while (!(ADC1->ISR & ADC_ISR_EOC));
	
	res = (110.0 - 30.0) * (float)((int16_t)ADC1->DR - TS_CAL1) / (float)(TS_CAL2 - TS_CAL1) + 30.0;
	
	ADC->CCR &= ~ADC_CCR_TSEN;
	ADC1->CR = 0;
	
	return res;
}

float GetIntVDD (void)
{
	float res;
	
	ADC1->CHSELR = ADC_CHSELR_CHSEL17;
	ADC1->SMPR = ADC_SMPR_SMP;
	ADC->CCR |= ADC_CCR_VREFEN;
	ADC1->CR = ADC_CR_ADSTART | ADC_CR_ADEN;
	while (!(ADC1->ISR & ADC_ISR_EOC));
	
	res = 3.3 * (float)VREFINT_CAL / (float)ADC1->DR;
	
	ADC->CCR &= ~ADC_CCR_VREFEN;
	ADC1->CR = 0;
	return res;
}

uint16_t ADC_GenericRead (ADC_TypeDef * ADCx, uint32_t ADC_Channel)
{
	ADCx->SMPR = 7;
	ADCx->CHSELR = ADC_Channel;
	ADCx->CR = ADC_CR_ADEN | ADC_CR_ADSTART;
	while(!(ADCx->ISR & ADC_ISR_EOC));
	return ADCx->DR;
}
