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

#ifndef STM32F10X_GPIO_H
#define STM32F10X_GPIO_H

#include "interfaces.h"
#include "devices.h"
#include <stm32f10x.h>

/**
 * @brief GPIO peripheral configuration base class.
 */
class GPIO_Config
{
public:
	explicit GPIO_Config() = default;
	virtual ~GPIO_Config() = default;
	friend class GPIO_Device;
protected:
	uint32_t cnf_mode = 0;
};

/**
 * @brief GPIO peripheral configuration class for input mode.
 */
class GPIO_Input : public GPIO_Config
{
public:
	virtual ~GPIO_Input() = default;

	/**
	 * @brief GPIO input mode configuration.
	 * @note To enable pull-up or pull-down resistor in 'PullUpDown' mode, use GPIO_Device::Set() method.
	 */
	enum class InputMode : uint8_t {
		Analog = 0,
		Floating = 1,
		PullUpDown = 2
	};

	/**
	 * @brief GPIO input mode configuration constructor.
	 * @param mode Input mode configuration.
	 */
	constexpr GPIO_Input(InputMode func = InputMode::Floating) {
		cnf_mode = static_cast<uint32_t>(func) << 2;
	}
};

/**
 * @brief GPIO peripheral configuration class for output mode.
 */
class GPIO_Output : public GPIO_Config
{
public:
	virtual ~GPIO_Output() = default;

	/**
	 * @brief GPIO output mode configuration.
	 */
	enum class OutputMode : uint8_t {
		PushPull = 0,
		OpenDrain = 1,
		AlternateFunctionPushPull = 2,
		AlternateFunctionOpenDrain = 3
	};

	/**
	 * @brief GPIO output speed configuration.
	 */
	enum class OutputSpeed : uint8_t {
		Speed10MHz = 1,
		Speed2MHz = 2,
		Speed50MHz = 3
	};

	/**
	 * @brief GPIO output mode configuration constructor.
	 *
	 * @param speed Output speed configuration.
	 * @param func Output function configuration.
	 */
	constexpr GPIO_Output(OutputMode func = OutputMode::PushPull, OutputSpeed speed = OutputSpeed::Speed2MHz) {
		cnf_mode = (static_cast<uint32_t>(func) << 2) | static_cast<uint32_t>(speed);
	}
};

class GPIO_Device: public GPIO_PinBase {
	friend class GPIO_Port;
public:
	/** Default constructor */
	explicit GPIO_Device() = delete;
	virtual ~GPIO_Device() = default;

	/**
	 * Constructor
	 *
	 * @param port GPIO port
	 * @param pin GPIO pin number
	 * @param dir config for GPIO pin direction and mode
	 */
	explicit constexpr GPIO_Device(GPIO_TypeDef *port,
								   uint16_t pin,
								   GPIO_Config &&config) :
		port_(port), pin_(pin), crl_crh(config.cnf_mode) {}

	/**
	 * Initialize GPIO pin
	 *
	 * @return Reference to the GPIO device
	 */
	GPIO_Device& Init();

	/**
	 * Set pin state
	 *
	 * @param state Pin state
	 */
	GPIO_Device& Set(bool state) override;

	/**
	 * Get pin state
	 *
	 * @return Pin state
	 */
	bool Get() override;
private:
	GPIO_TypeDef *port_;
	uint16_t pin_;
	uint32_t crl_crh;

	constexpr static uint32_t rcc_enable(GPIO_TypeDef *port) {
		if (port == GPIOA)
			return RCC_APB2ENR_IOPAEN;
		else if (port == GPIOB)
			return RCC_APB2ENR_IOPBEN;
		else if (port == GPIOC)
			return RCC_APB2ENR_IOPCEN;
		else if (port == GPIOD)
			return RCC_APB2ENR_IOPDEN;
		else if (port == GPIOE)
			return RCC_APB2ENR_IOPEEN;
#if defined(STM32F10X_HD) || defined(STM32F10X_XL)
		else if constexpr (port == GPIOF) return RCC_APB2ENR_IOPFEN;
		else if constexpr (port == GPIOG) return RCC_APB2ENR_IOPGEN;
#endif
		return 0;
	}
};

/**
 * @brief GPIO Port Class. Allows access to entire port.
 */
class GPIO_Port : public GPIO_PortBase<uint16_t>
{
public:
	/** Default constructor */
	explicit GPIO_Port() = delete;
	virtual ~GPIO_Port() = default;

	/**
	 * Constructor
	 *
	 * @param port GPIO port number
	 */
	explicit constexpr
	GPIO_Port(GPIO_TypeDef *port) : port_(port) {}

	/**
	 * Read port bitmask.
	 *
	 * @return Port pins state.
	 */
	uint16_t Read() override {
		return port_->IDR;
	}

	/**
	 * Write bitmask to GPIO port.
	 *
	 * @param bitmask Bitmask to write.
	 * @return Reference to itself.
	 */
	GPIO_PortBase &Write(uint16_t bitmask) override {
		port_->ODR = bitmask;
		return *this;
	}
private:
	GPIO_TypeDef *port_;
};

#endif /* STM32F10X_GPIO_H */
