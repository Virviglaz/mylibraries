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

#define HSI_CLOCK_FREQ		8000000UL

static uint32_t hse_freq_ = HSE_DEFAULT_CLOCK_FREQ;
static uint32_t cpu_freq_hz_ = HSI_CLOCK_FREQ;
static uint32_t ahb_freq_hz_ = HSI_CLOCK_FREQ;
static uint32_t apb1_freq_hz_ = HSI_CLOCK_FREQ;
static uint32_t apb2_freq_hz_ = HSI_CLOCK_FREQ;
static Clocks::ClockSource current_source_ = Clocks::ClockSource::HSI_CLOCK;

Clocks::Clocks()
	: cpu_freq_hz(cpu_freq_hz_),
	  ahb_freq_hz(ahb_freq_hz_),
	  apb1_freq_hz(apb1_freq_hz_),
	  apb2_freq_hz(apb2_freq_hz_)
{}

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

static inline Clocks::ClockSource get_pll_source(void)
{
	return RCC->CFGR & RCC_CFGR_PLLSRC ? Clocks::HSE_CLOCK : Clocks::HSI_CLOCK;
}

static uint8_t get_pll_mult(void)
{
	uint32_t mult = (RCC->CFGR & RCC_CFGR_PLLMULL) >> 18;

	return mult >= 2 ? mult + 2 : 1;
}

Clocks &Clocks::Update()
{
	switch (Clocks::GetClockSource()) {
	case HSI_CLOCK:
		cpu_freq_hz = HSI_CLOCK_FREQ;
		break;
	case HSE_CLOCK:
		cpu_freq_hz = hse_freq_;
		break;
	case PLL_CLOCK:
		if (get_pll_source() == HSI_CLOCK)
			cpu_freq_hz = HSI_CLOCK_FREQ / 2 * get_pll_mult();
		else
			cpu_freq_hz = hse_freq_ * get_pll_mult();
		break;
	default:
		cpu_freq_hz = 0;
		break;
	}

	ahb_freq_hz = cpu_freq_hz / get_ahb_mult();
	apb1_freq_hz = ahb_freq_hz / get_apb1_mult();
	apb2_freq_hz = ahb_freq_hz / get_apb2_mult();

	return *this;
}

Clocks::ClockSource Clocks::GetClockSource()
{
	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSI)
		return ClockSource::HSI_CLOCK;

	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSE)
		return ClockSource::HSE_CLOCK;

	if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL)
		return ClockSource::PLL_CLOCK;

	return ClockSource::INV_CLOCK;
}

void Clocks::RunFromHSI()
{
	RCC->CR |= RCC_CR_HSION;

	while (!(RCC->CR & RCC_CR_HSIRDY))
		;

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
	if (mult >= 2 && mult <= 16)
		return;	

	switch (current_source_) {
	case HSE_CLOCK:
		RCC->CFGR &= ~RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC;
		RCC->CFGR |= RCC_CFGR_PLLSRC;
		break;
	case HSE_CLOCK_DIV2:
		RCC->CFGR |= RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC;
		break;
	case HSI_CLOCK:
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
