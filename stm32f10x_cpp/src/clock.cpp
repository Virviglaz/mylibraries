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
 * STM32F10x Clock configuration interface.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "clock.h"
#include <stm32f10x.h>

#define HSI_CLOCK_FREQ		8000000UL

static uint32_t hse_freq_ = HSE_DEFAULT_CLOCK_FREQ;
static uint32_t cpu_freq_hz_ = HSI_CLOCK_FREQ;
static uint32_t ahb_freq_hz_ = HSI_CLOCK_FREQ;
static uint32_t apb1_freq_hz_ = HSI_CLOCK_FREQ;
static uint32_t apb2_freq_hz_ = HSI_CLOCK_FREQ;
static Clocks::ClockSource current_source_ = Clocks::ClockSource::HSI_CLOCK;

static uint32_t get_ahb_mult(void)
{
	uint32_t mult = (RCC->CFGR & RCC_CFGR_HPRE) >> 4;

	return mult < 8 ? 1 : (1 << (mult - 7));
}

static uint32_t get_apb1_mult(void)
{
	uint32_t mult = (RCC->CFGR & RCC_CFGR_PPRE1) >> 8;

	return mult < 4 ? 1 : (1 << (mult - 3));
}

static uint32_t get_apb2_mult(void)
{
	uint32_t mult = (RCC->CFGR & RCC_CFGR_PPRE2) >> 11;

	return mult < 4 ? 1 : (1 << (mult - 3));
}

/**
 * Per the STM32F10x reference manual: if the corresponding APBx prescaler
 * is 1, the timers on that bus are clocked at apb_freq_hz; otherwise they
 * are clocked at 2 * apb_freq_hz.
 */
static inline uint32_t get_timer_freq_hz(uint32_t apb_freq_hz, uint32_t apb_mult)
{
	return apb_mult == 1 ? apb_freq_hz : apb_freq_hz * 2;
}

static inline Clocks::ClockSource get_pll_source(void)
{
	return RCC->CFGR & RCC_CFGR_PLLSRC ? Clocks::ClockSource::HSE_CLOCK : Clocks::ClockSource::HSI_CLOCK;
}

static uint8_t get_pll_mult(void)
{
	uint32_t mult = (RCC->CFGR & RCC_CFGR_PLLMULL) >> 18;

	return mult >= 2 ? mult + 2 : 1;
}

void Clocks::Update()
{
	switch (Clocks::GetClockSource()) {
	case Clocks::ClockSource::HSI_CLOCK:
		cpu_freq_hz_ = HSI_CLOCK_FREQ;
		break;
	case Clocks::ClockSource::HSE_CLOCK:
		cpu_freq_hz_ = hse_freq_;
		break;
	case Clocks::ClockSource::PLL_CLOCK:
		if (get_pll_source() == Clocks::ClockSource::HSI_CLOCK)
			cpu_freq_hz_ = HSI_CLOCK_FREQ / 2 * get_pll_mult();
		else
			cpu_freq_hz_ = hse_freq_ * get_pll_mult();
		break;
	default:
		cpu_freq_hz_ = 0;
		break;
	}

	ahb_freq_hz_ = cpu_freq_hz_ / get_ahb_mult();
	apb1_freq_hz_ = ahb_freq_hz_ / get_apb1_mult();
	apb2_freq_hz_ = ahb_freq_hz_ / get_apb2_mult();
}

Clocks::ClockSource Clocks::GetClockSource()
{
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSI)
		return Clocks::ClockSource::HSI_CLOCK;

	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSE)
		return Clocks::ClockSource::HSE_CLOCK;

	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)
		return Clocks::ClockSource::PLL_CLOCK;

	return Clocks::ClockSource::INV_CLOCK;
}

void Clocks::RunFromHSI()
{
	RCC->CR |= RCC_CR_HSION;

	while (!(RCC->CR & RCC_CR_HSIRDY)) {}

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;

	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

	current_source_ = Clocks::ClockSource::HSI_CLOCK;
}

void Clocks::RunFromHSE(uint32_t crystal_freq_hz)
{
	hse_freq_ = crystal_freq_hz;

	RCC->CR |= RCC_CR_CSSON | RCC_CR_HSEON;

	while (!(RCC->CR & RCC_CR_HSERDY)) {}

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSE;

	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}

	current_source_ = Clocks::ClockSource::HSE_CLOCK;
}

void Clocks::EnablePLL(uint8_t mult)
{
	if (mult < 2 || mult > 16)
		return;	

	switch (current_source_) {
	case Clocks::ClockSource::HSE_CLOCK:
		RCC->CFGR &= ~RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC;
		RCC->CFGR |= RCC_CFGR_PLLSRC;
		break;
	case Clocks::ClockSource::HSE_CLOCK_DIV2:
		RCC->CFGR |= RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC;
		break;
	case Clocks::ClockSource::HSI_CLOCK:
		RCC->CFGR &= ~RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC;
		break;
	default:
		return;
	}

	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR |= (uint32_t)(mult - 2) << 18;
	RCC->CR |=  RCC_CR_CSSON | RCC_CR_PLLON;

	while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

	current_source_ = Clocks::ClockSource::PLL_CLOCK;
	cpu_freq_hz_ = hse_freq_ * mult;
	ahb_freq_hz_ = cpu_freq_hz_ / get_ahb_mult();
	apb1_freq_hz_ = ahb_freq_hz_ / get_apb1_mult();
	apb2_freq_hz_ = ahb_freq_hz_ / get_apb2_mult();
}

uint32_t Clocks::GetFreqHz(BusClock source)
{
	switch (source) {
	case Clocks::BusClock::HSE_FREQ:
		return hse_freq_;
	case Clocks::BusClock::CPU_FREQ:
		return cpu_freq_hz_;
	case Clocks::BusClock::AHB_FREQ:
		return ahb_freq_hz_;
	case Clocks::BusClock::APB1_FREQ:
		return apb1_freq_hz_;
	case Clocks::BusClock::APB2_FREQ:
		return apb2_freq_hz_;
	}
	return 0;
}

static uint64_t time_us = 0;

extern "C"
{
	void SysTick_Handler(void) {
		time_us += SysTick->LOAD;
	}
}

void SystemClock::Configure()
{
	SysTick->LOAD = 0xFFFFFF;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	__enable_irq();
}

uint64_t SystemClock::GetTimeStampUs()
{
	uint64_t t1;
	uint32_t val;
	uint64_t t2;

	do {
		t1 = time_us;
		val = SysTick->VAL;
		t2 = time_us;
	} while (t1 != t2);

	uint64_t elapsed_in_period = SysTick->LOAD - val;

	return (t1 + elapsed_in_period) * 8000000 / ahb_freq_hz_;
}

