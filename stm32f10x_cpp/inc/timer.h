/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2026 Pavel Nadein
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * STM32F10x Timer peripheral interface.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef STM32F10X_TIMER_H
#define STM32F10X_TIMER_H

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

#include "devices.h"
#include <stm32f10x.h>
#include <assert.h>

class Timer_Device : public Timer_DeviceBase
{
public:
	/** Default constructor */
	explicit Timer_Device() = delete;
	virtual ~Timer_Device() = default;

	/**
	 * Constructor
	 *
	 * @param tim Timer number
	 */
	explicit constexpr Timer_Device(uint8_t tim) : Timer_DeviceBase(tim)
	{
		switch (tim)
		{
		case 1:
			tim_base_ = TIM1;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM1EN;
#if defined(STM32F10X_LD) || defined(STM32F10X_MD) || defined(STM32F10X_HD) || \
	defined(STM32F10X_CL)
			irq_number_ = TIM1_UP_IRQn;
#endif /* STM32F10X_LD STM32F10X_MD STM32F10X_HD STM32F10X_CL */
			break;
		case 2:
			tim_base_ = TIM2;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM2EN;
			irq_number_ = TIM2_IRQn;
			break;
		case 3:
			tim_base_ = TIM3;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM3EN;
			irq_number_ = TIM3_IRQn;
			break;
#if !defined (STM32F10X_LD) && !defined (STM32F10X_LD_VL)
		case 4:
			tim_base_ = TIM4;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM4EN;
			irq_number_ = TIM4_IRQn;
			break;
#endif /* !STM32F10X_LD !STM32F10X_LD_VL */
#if defined (STM32F10X_HD) || defined  (STM32F10X_CL)
		case 5:
			tim_base_ = TIM5;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM5EN;
			irq_number_ = TIM5_IRQn;
			break;
		case 6:
			tim_base_ = TIM6;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM6EN;
			irq_number_ = TIM6_IRQn;
			break;
		case 7:
			tim_base_ = TIM7;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM7EN;
			irq_number_ = TIM7_IRQn;
			break;
#endif /* STM32F10X_HD STM32F10X_CL */
#if defined (STM32F10X_HD) || defined (STM32F10X_XL)
		case 8:
			tim_base_ = TIM8;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM8EN;
			irq_number_ = TIM8_UP_TIM13_IRQn;
			break;
#endif /* STM32F10X_HD STM32F10X_XL */
#ifdef STM32F10X_XL
		case 9:
			tim_base_ = TIM9;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM9EN;
			irq_number_ = TIM1_BRK_TIM9_IRQn;
			break;
		case 10:
			tim_base_ = TIM10;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM10EN;
			irq_number_ = TIM1_UP_TIM10_IRQn;
			break;
		case 11:
			tim_base_ = TIM11;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM11EN;
			irq_number_ = TIM1_TRG_COM_TIM11_IRQn;
			break;
#endif /* STM32F10X_XL */
#ifdef STM32F10X_HD_VL
		case 12:
			tim_base_ = TIM12;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM12EN;
			irq_number_ = TIM8_BRK_TIM12_IRQn;
			break;
		case 13:
			tim_base_ = TIM13;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM13EN;
			irq_number_ = TIM8_UP_TIM13_IRQn;
			break;
		case 14:
			tim_base_ = TIM14;
			clock_enable_reg_ = &RCC->APB1ENR;
			clock_enable_bit_ = RCC_APB1ENR_TIM14EN;
			irq_number_ = TIM8_TRG_COM_TIM14_IRQn;
			break;
#endif /* STM32F10X_HD_VL */
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || \
	defined (STM32F10X_HD_VL)
		case 15:
			tim_base_ = TIM15;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM15EN;
			irq_number_ = TIM1_BRK_TIM15_IRQn;
			break;
		case 16:
			tim_base_ = TIM16;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM16EN;
			irq_number_ = TIM1_UP_TIM16_IRQn;
			break;
		case 17:
			tim_base_ = TIM17;
			clock_enable_reg_ = &RCC->APB2ENR;
			clock_enable_bit_ = RCC_APB2ENR_TIM17EN;
			irq_number_ = TIM1_TRG_COM_TIM17_IRQn;
			break;
#endif /* STM32F10X_LD_VL STM32F10X_MD_VL STM32F10X_HD_VL */
		default:
			assert(false);
		}
	}

	/**
	 * Initialize the timer peripheral
	 *
	 * @param period Timer period
	 *
	 * @return Reference to the Timer device
	 */
	Timer_DeviceBase &Init(uint32_t period = 0) override;

	/**
	 * Initialize the timer to run at specified frequency
	 *
	 * @param freq_hz Timer frequency in Hz
	 *
	 * @return Reference to the Timer device
	 */
	Timer_DeviceBase &InitAt(uint32_t freq_hz = 1000000UL);

	/**
	 * Enable the timer
	 *
	 * @return Reference to the Timer device
	 */
	Timer_DeviceBase &Enable() override;

	/**
	 * Disable the timer
	 *
	 * @return Reference to the Timer device
	 */
	Timer_DeviceBase &Disable() override;

	/**
	 * Reset the timer counter
	 *
	 * @return Reference to the Timer device
	 */
	Timer_DeviceBase &Reset() override;

	/**
	 * Wait for the specified number of timer ticks
	 *
	 * @param timer_ticks Number of timer ticks to wait
	 *
	 * @return Reference to the Timer device
	 */
	Timer_DeviceBase &Wait(uint32_t timer_ticks) override;

private:
	TIM_TypeDef *tim_base_ = nullptr;
	volatile uint32_t *clock_enable_reg_ = nullptr;
	uint32_t clock_enable_bit_ = 0;
	IRQn_Type irq_number_ = NonMaskableInt_IRQn;
};

#endif /* STM32F10X_TIMER_H */
