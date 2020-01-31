#include "bootflash.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_syscfg.h"

void JumpToMemory(uint32_t offset)
{
	typedef void(*pFunction)(void);
	pFunction Jump_To_Application;
	uint32_t JumpAddress;

	JumpAddress = *(__IO uint32_t*) (offset + 4);
	Jump_To_Application = (pFunction)JumpAddress;
	__set_MSP(*(__IO uint32_t*) offset);
	Jump_To_Application();
}

void Remap_Table(uint32_t FW_Address)
{
	// Copy interrupt vector table to the RAM.
	volatile uint32_t *VectorTable = (volatile uint32_t *)0x20000000;
	uint32_t ui32_VectorIndex = 0;
	for (ui32_VectorIndex = 0; ui32_VectorIndex < 48; ui32_VectorIndex++) {
		VectorTable[ui32_VectorIndex] = *(__IO uint32_t*)(FW_Address + \
			(ui32_VectorIndex << 2));
	}
	//  Enable SYSCFG peripheral clock
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// Remap RAM into 0x0000 0000
	SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
}

void DisableInterrupts(void)
{
	// Disable all peripheral interrupts
	NVIC_DisableIRQ(SysTick_IRQn);
	NVIC_DisableIRQ(USART2_IRQn);

	NVIC_DisableIRQ(WWDG_IRQn);
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_DisableIRQ(FLASH_IRQn);
	NVIC_DisableIRQ(RCC_IRQn);
	NVIC_DisableIRQ(EXTI0_1_IRQn);
	NVIC_DisableIRQ(EXTI2_3_IRQn);
	NVIC_DisableIRQ(EXTI4_15_IRQn);
	NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	NVIC_DisableIRQ(DMA1_Channel4_5_IRQn);
	NVIC_DisableIRQ(ADC1_IRQn);
	NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
	NVIC_DisableIRQ(TIM1_CC_IRQn);
	NVIC_DisableIRQ(TIM3_IRQn);
	NVIC_DisableIRQ(TIM6_IRQn);
	NVIC_DisableIRQ(TIM7_IRQn);
	NVIC_DisableIRQ(TIM14_IRQn);
	NVIC_DisableIRQ(TIM15_IRQn);
	NVIC_DisableIRQ(TIM16_IRQn);
	NVIC_DisableIRQ(TIM17_IRQn);
	NVIC_DisableIRQ(I2C1_IRQn);
	NVIC_DisableIRQ(I2C2_IRQn);
	NVIC_DisableIRQ(SPI1_IRQn);
	NVIC_DisableIRQ(SPI2_IRQn);
	NVIC_DisableIRQ(USART1_IRQn);
	NVIC_DisableIRQ(USART2_IRQn);
	NVIC_DisableIRQ(USART3_6_IRQn);
}
