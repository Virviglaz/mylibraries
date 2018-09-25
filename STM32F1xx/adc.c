#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "adc.h"

void adc1_init (uint8_t channel)
{
	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = channel;
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_Cmd(ADC1, ENABLE);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
}

uint16_t ADC_Get_Result (uint8_t channel)
{
  	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
  	// Start the conversion
  	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  	// Wait until conversion completion
  	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  	// Get the conversion value
  	return ADC_GetConversionValue(ADC1);
}
