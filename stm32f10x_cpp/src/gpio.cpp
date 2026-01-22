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
 * STM32F10x GPIO peripheral interface.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "gpio.h"
#include <stm32f10x.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef BIT
#define BIT(x) (1U << (x))
#endif

#if __cplusplus <= 201402L
#error "This file requires at least a C++17 compliant compiler"
#endif

int GPIO_Interface::Read(uint16_t port, uint16_t pin)
{
	GPIO_TypeDef* gpio = GPIO_Device::getGPIOPort(port);
	if (gpio == nullptr)
		return -1;

	return (gpio->IDR & pin) ? 1 : 0;
}

void GPIO_Interface::Write(uint16_t port, uint16_t pin, int state)
{
	GPIO_TypeDef* gpio = GPIO_Device::getGPIOPort(port);
	if (gpio == nullptr)
		return;

	if (state)
		gpio->BSRR = pin;
	else
		gpio->BRR = pin;
}

GPIO_Device &GPIO_Device::Init()
{
	RCC->APB2ENR |= config_.rcc_mask;
	GPIO_TypeDef *gpio = config_.gpio;
	if (gpio == nullptr)
		return *this;

	gpio->CRL &= config_.crl_clear;
	gpio->CRL |= config_.crl_set;
	gpio->CRH &= config_.crh_clear;
	gpio->CRH |= config_.crh_set;
	if (config_.direction == PULL_UP)
		Set(1);
	else if (config_.direction == PULL_DOWN)
		Set(0);

	return *this;
}

GPIO_Device &GPIO_Device::Set(uint16_t state)
{
	GPIO_TypeDef* gpio = getGPIOPort(port_);
	if (gpio == nullptr)
		return *this;

	if (state)
		gpio->BSRR = BIT(pin_);
	else
		gpio->BRR = BIT(pin_);

	return *this;
}

int GPIO_Device::Get()
{
	GPIO_TypeDef* gpio = getGPIOPort(port_);
	if (gpio == nullptr)
		return -1;

	return (gpio->IDR & BIT(pin_)) ? 1 : 0;
}
