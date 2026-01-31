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

#include "clock.h"
#include "timer.h"

Timer_DeviceBase &Timer_Device::Init(uint32_t period)
{
	// Enable timer clock
	*clock_enable_reg_ |= clock_enable_bit_;

	tim_base_->ARR = period ? period : UINT16_MAX;
	tim_base_->EGR = TIM_EGR_UG;

	return *this;
}

Timer_DeviceBase &Timer_Device::InitAt(uint32_t freq_hz)
{
	Init();

	if (clock_enable_reg_ == &RCC->APB1ENR)
	{
		tim_base_->PSC = (Clocks().apb1_freq_hz / freq_hz) - 1;
	}
	else
	{
		tim_base_->PSC = (Clocks().apb2_freq_hz / freq_hz) - 1;
	}

	return *this;
}

Timer_DeviceBase &Timer_Device::Enable()
{
	tim_base_->CR1 |= TIM_CR1_CEN;
	return *this;
}

Timer_DeviceBase &Timer_Device::Disable()
{
	tim_base_->CR1 &= ~TIM_CR1_CEN;
	return *this;
}

Timer_DeviceBase &Timer_Device::Reset()
{
	tim_base_->CNT = 0;
	return *this;
}

Timer_DeviceBase &Timer_Device::Wait(uint32_t timer_ticks)
{
	tim_base_->CNT = 0;

	while (tim_base_->CNT < timer_ticks)
	{
		__NOP();
	}

	return *this;
}
