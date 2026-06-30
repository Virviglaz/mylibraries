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
 * Abstract interface for embedded systems peripherals.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef DEVICES_H
#define DEVICES_H

#include "interfaces.h"
#include <cstddef>

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

/**
 * @brief GPIO Pin Base Class. Access to individual pin.
 */
class GPIO_PinBase
{
public:
	explicit GPIO_PinBase() = default;
	virtual ~GPIO_PinBase() = default;

	/**
	 * @brief GPIO pin direction
	 */
	enum class Direction
	{
		/**
		 * Input direction
		 */
		INPUT,
		/**
		 * Output direction
		 */
		OUTPUT,
		/**
		 * Open-drain output direction
		 */
		OPEN_DRAIN,

		/**
		 * Input with pull-up resistor
		 */
		PULL_UP,

		/**
		 * Input with pull-down resistor
		 */
		PULL_DOWN
	};

	/**
	 * @brief Set pin state (optional)
	 *
	 * @param state Pin state
	 */
	virtual GPIO_PinBase &Set(bool) { return *this; };

	/**
	 * @brief Get pin state
	 *
	 * @return Pin state
	 */
	virtual bool Get() = 0;
};

/**
 * @brief GPIO Port Base Class. Allows access to entire port.
 * 
 * @tparam T 
 */
template <class T>
class GPIO_PortBase
{
public:
	explicit GPIO_PortBase() = default;
	virtual ~GPIO_PortBase() = default;

	/**
	 * @brief Read port bitmask.
	 * 
	 * @return T Port pins state.
	 */
	virtual T Read() = 0;

	/**
	 * @brief Write bitmask to GPIO port.
	 * 
	 * @param bitmask Bitmask to write.
	 * @return GPIO_PortBase& reference to itself.
	 */
	virtual GPIO_PortBase &Write(T bitmask) = 0;
};

/**
 * @brief I2C Device Base Class
 */
class I2C_DeviceBase
{
public:
	explicit I2C_DeviceBase() = delete;
	virtual ~I2C_DeviceBase() = default;

	/**
	 * @brief Constructor
	 *
	 * @param ifs I2C interface
	 * @param address I2C device address
	 */
	explicit
	I2C_DeviceBase(I2C_InterfaceBase &ifs, uint8_t address) :
		ifs_(ifs), address_(address) {}

	/**
	 * @brief Write data to I2C device
	 *
	 * @param reg_addr Register address to write to
	 * @param reg_addr_size Size of register address in bytes
	 * @param data Data to write
	 * @param length Length of data to write
	 *
	 * @return Reference to the I2C device
	 */
	virtual I2C_DeviceBase &Write(const uint8_t *reg_addr,
								  size_t reg_addr_size,
								  const uint8_t *data,
								  size_t length)
	{
		ifs_.Write(address_, reg_addr, reg_addr_size, data, length);
		return *this;
	};

	/**
	 * @brief Convenience template method to write data of any type
	 *
	 * @param reg_addr Register address to write to
	 * @param data Data to write
	 * @return Reference to the I2C device
	 */
	virtual I2C_DeviceBase &Write(uint8_t reg_addr,
								  const uint8_t *data,
								  size_t length)
	{
		return Write(reinterpret_cast<const uint8_t *>(&reg_addr), sizeof(reg_addr), data, length);
	}

	/**
	 * @brief Convenience template method to write data of any type
	 *
	 * @param reg_addr Register address to write to
	 * @param data Data to write
	 */
	virtual I2C_DeviceBase &Write(uint8_t reg_addr, uint8_t data) {
		return Write(reinterpret_cast<const uint8_t*>(&reg_addr), sizeof(reg_addr), reinterpret_cast<const uint8_t*>(&data), sizeof(data));
	}

	/**
	 * @brief Read data from I2C device
	 *
	 * @param reg_addr Register address to read from
	 * @param reg_addr_size Size of register address in bytes
	 * @param data Buffer to store read data
	 * @param length Length of data to read
	 *
	 * @return Reference to the I2C device
	 */
	virtual I2C_DeviceBase &Read(const uint8_t *reg_addr,
								 size_t reg_addr_size,
								 uint8_t *data,
								 size_t length)
	{
		ifs_.Read(address_, reg_addr, reg_addr_size, data, length);
		return *this;
	}

	/**
	 * @brief Read data from I2C device (optional)
	 *
	 * @param reg_addr Register address to read from
	 * @param data Buffer to store read data
	 * @param length Length of data to read
	 * @return Reference to the I2C device
	 */
	virtual I2C_DeviceBase &Read(uint8_t reg_addr,
								 uint8_t *data,
								 size_t length)
	{
		return Read(reinterpret_cast<const uint8_t *>(&reg_addr), sizeof(reg_addr), data, length);
	}

	/**
	 * @brief Read data from I2C device (optional)
	 *
	 * @param reg_addr Register address to read from
	 * @param data Buffer to store read data
	 * @return Reference to the I2C device
	 */
	virtual I2C_DeviceBase &Read(uint8_t reg_addr, uint8_t &data) {
		return Read(reinterpret_cast<const uint8_t*>(&reg_addr), sizeof(reg_addr), reinterpret_cast<uint8_t*>(&data), sizeof(data));
	}

protected:
	I2C_InterfaceBase &ifs_;
	uint8_t address_;
};

/**
 * @brief SPI Device Base Class
 */
class SPI_DeviceBase
{
public:
	explicit SPI_DeviceBase() = delete;
	virtual ~SPI_DeviceBase() = default;

	/**
	 * @brief Constructor
	 *
	 * @param ifs SPI interface
	 * @param cs_pin Chip select GPIO pin
	 */
	explicit
	SPI_DeviceBase(SPI_InterfaceBase &ifs) : ifs_(ifs) {}

	/**
	 * @brief Transfer data over SPI
	 *
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual SPI_DeviceBase &Transfer(const uint8_t *tx_data,
									 uint8_t *rx_data,
									 size_t length)
	{
		ifs_.Transfer(tx_data, rx_data, length);
		return *this;
	};

protected:
	SPI_InterfaceBase &ifs_;
};

/**
 * @brief Timer Device Base Class
 */
class Timer_DeviceBase
{
public:
	explicit Timer_DeviceBase() = delete;
	virtual ~Timer_DeviceBase() = default;

	/**
	 * @brief Constructor
	 *
	 * @param tim Timer number
	 */
	explicit constexpr
	Timer_DeviceBase(uint8_t tim) : tim_(tim) {}

	virtual Timer_DeviceBase &Init(uint32_t period = 0) = 0;

	virtual Timer_DeviceBase &Enable() = 0;
	virtual Timer_DeviceBase &Disable() = 0;
	virtual Timer_DeviceBase &Reset() = 0;
	virtual Timer_DeviceBase &Wait(uint32_t timer_ticks) = 0;
#if 0
	class Timer_InterruptHandlerBase
	{
	public:
		explicit Timer_InterruptHandlerBase() = default;
		virtual ~Timer_InterruptHandlerBase() = default;

		/**
		 * Timer interrupt handler
		 */
		virtual void HandleInterrupt() = 0;
	};

	virtual void EnableInterrupt(Timer_InterruptHandlerBase &handler) = 0;
#endif
protected:
	uint8_t tim_;
};

#endif /* DEVICES_H */
