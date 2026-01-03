/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2025 Pavel Nadein
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
 * Streaming interface for ebmedded devices. Prints trace to standart output.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef DEVICES_OSTREAM_H
#define DEVICES_OSTREAM_H

#include "devices.h"
#include <string>

/**
 * GPIO Device streaming to standart output
 */
class GPIO_DeviceOStream : public GPIO_DeviceBase
{
public:
	GPIO_DeviceOStream() = delete;

	/**
	 * Constructor for GPIO device.
	 *
	 * @param name Device name for logging
	 * @param pin GPIO pin number
	 * @param dir GPIO direction
	 *
	 * @param read_value Value to return on Get() calls (only for INPUT direction)
	 */
	GPIO_DeviceOStream(const std::string &name,
					   uint16_t pin,
					   GPIO_DeviceBase::dir dir,
					   int read_value = 0) :
					   GPIO_DeviceBase(pin, dir), name_(name), dir_(dir), read_value_(read_value) {}

	/**
	 * Set value to output pin.
	 *
	 * @param state State to set (only for OUTPUT direction)
	 * @throws std::invalid_argument if GPIO is not configured as OUTPUT
	 */
	GPIO_DeviceOStream& Set(uint16_t state);

	/**
	 * Read value from input pin.
	 *
	 * @return Current GPIO state (only for INPUT direction)
	 * @throws std::invalid_argument if GPIO is not configured as INPUT
	 */
    int Get();
private:
    std::string name_;
    GPIO_DeviceBase::dir dir_;
    int read_value_;
};

/**
 * I2C Device streaming to standart output
 */
class I2C_DeviceOStream : public I2C_DeviceBase
{
public:
	I2C_DeviceOStream() = delete;

	/**
	 * Constructor for I2C Device.
	 *
	 * @param name Device name for logging
	 */
	I2C_DeviceOStream(const std::string &name) :
		I2C_DeviceBase(dummy_ifs_, 0), name_(name) {}

	/**
	 * Write data to I2C Device.
	 *
	 * @param reg_addr Register address to write to
	 * @param data Data to write
	 * @param length Length of data to write
	 *
	 * @return Reference to the I2C Device
	 * @throws std::invalid_argument if data is null or length is zero
	 */
	I2C_DeviceOStream &Write(uint8_t reg_addr,
							 const uint8_t *data,
							 uint32_t length) override;

	/**
	 * Read data from I2C Device.
	 *
	 * @param reg_addr Register address to read from
	 * @param data Buffer to store read data
	 * @param length Length of data to read
	 *
	 * @return Reference to the I2C Device
	 * @throws std::invalid_argument if data is null or length is zero
	 */
	I2C_DeviceOStream &Read(uint8_t reg_addr,
							uint8_t *data,
							uint32_t length) override;

private:
	/**
	 * Dummy I2C Interface for base class construction
	 *
	 * This interface does nothing and is only used to satisfy the base class constructor
	 */
	class I2C_InterfaceDummy : public I2C_InterfaceBase
	{
	public:
		int Read(uint8_t device_addr, uint8_t reg_addr,
				 uint8_t *data, uint32_t length) override
		{
			return 0;
		}
	} dummy_ifs_;

	std::string name_;
};

class SPI_DeviceOStream : public SPI_DeviceBase
{
public:
	/**
	 * Constructor for SPI Device.
	 *
	 * @param name Device name for logging
	 */
	SPI_DeviceOStream(const std::string &name) :
		SPI_DeviceBase(dummy_spi_ifs_, dummy_gpio_ifs_),
		name_(name) {}

	/**
	 * Transfer data (read + write)
	 *
	 * @param tx_data Data to write
	 * @param rx_data Data to read
	 * @param length Size of data buffer
	 *
	 * @return 0 on success
	 */
	SPI_DeviceOStream &Transfer(const uint8_t *tx_data,
								uint8_t *rx_data,
								uint32_t length);

private:
	/**
	 * Dummy SPI Interface for base class construction
	 *
	 * This interface does nothing and is only used to satisfy the base class constructor
	 */
	class SPI_InterfaceDummy : public SPI_InterfaceBase
	{
	public:
		int Transfer(const uint8_t *tx_data,
					 uint8_t *rx_data,
					 uint32_t length) override
		{
			return 0;
		}
	} dummy_spi_ifs_;

	/**
	 * Dummy GPIO Interface for base class construction
	 *
	 * This interface does nothing and is only used to satisfy the base class constructor
	 */
	class GPIO_InterfaceDummy : public GPIO_InterfaceBase
	{
	public:
		void Write(uint16_t pin, int state) override { }
		int Read(uint16_t pin) override { return 0; }
	} dummy_gpio_ifs_;

	std::string name_;
};

#endif /* DEVICES_OSTREAM_H */
