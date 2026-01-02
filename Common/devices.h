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
 * Abstract interface for ebmedded systems peripherals.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef DEVICES_H
#define DEVICES_H

#include "interfaces.h"
#include <stddef.h>

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

/**
 * GPIO Device Base Class
 */
class GPIO_DeviceBase
{
public:
	/**
	 * GPIO pin direction
	 */
	enum dir
	{
		/**
		 * Input direction
		 */
		INPUT,
		/**
		 * Output direction
		 */
		OUTPUT
	};

	/**
	 * Constructor
	 *
	 * @param pin GPIO pin number
	 * @param dir Pin direction
	 */
	explicit GPIO_DeviceBase(uint16_t pin, dir dir) : pin_(pin) {}

	/**
	 * Set pin state (optional)
	 *
	 * @param state Pin state
	 */
	virtual GPIO_DeviceBase& Set(uint16_t state) { return *this; }

	/**
	 * Get pin state
	 *
	 * @return Pin state
	 */
	virtual int Get() = 0;

	/**
	 * Get pin number
	 *
	 * @return Pin number
	 */
	virtual uint16_t GetPin() const { return pin_; }

protected:
	uint16_t pin_;
};

/**
 * I2C Device Base Class
 */
class I2C_DeviceBase
{
public:
	/** Default constructor */
	I2C_DeviceBase();

	/**
	 * Constructor
	 *
	 * @param ifs I2C interface
	 * @param address I2C device address
	 */
	explicit
	I2C_DeviceBase(I2C_InterfaceBase &ifs, uint8_t address) : ifs_(ifs), address_(address) {}

	/**
	 * Write data to I2C device (optional)
	 *
	 * @param reg_addr Register address to write to
	 * @param dst Data to write
	 * @param size Length of data to write
	 *
	 * @return Reference to the I2C device
	 */

	virtual I2C_DeviceBase &Write(uint8_t reg_addr,
								  const uint8_t *data,
								  uint32_t length) { return *this; };
	/**
	 * Read data from I2C device
	 *
	 * @param reg_addr Register address to read from
	 * @param dst Buffer to store read data
	 * @param size Length of data to read
	 *
	 * @return Reference to the I2C device
	 */
	virtual I2C_DeviceBase &Read(uint8_t reg_addr,
								 uint8_t *data,
								 uint32_t length) = 0;

protected:
	I2C_InterfaceBase &ifs_;
	uint8_t address_;
};

/**
 * SPI Device Base Class
 */
class SPI_DeviceBase
{
public:
	/**
	 * Constructor
	 */
	explicit SPI_DeviceBase();

	/**
	 * Constructor
	 *
	 * @param ifs SPI interface
	 * @param cs_pin Chip select GPIO pin
	 */
	SPI_DeviceBase(SPI_InterfaceBase &ifs, GPIO_InterfaceBase &cs_pin) : ifs_(ifs), cs_pin_(cs_pin) {}

	/**
	 * Transfer data over SPI
	 *
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual SPI_DeviceBase& Transfer(const uint8_t *tx_data,
						 uint8_t *rx_data,
						 uint32_t length) = 0;

protected:
	SPI_InterfaceBase &ifs_;
	GPIO_InterfaceBase &cs_pin_;
};

#endif /* DEVICES_H */
