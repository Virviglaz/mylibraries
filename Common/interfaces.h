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
 * Abstract interface for ebmedded systems peripherals.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef INTERFACES_H
#define INTERFACES_H

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

#include <stdint.h>

/**
 * Simple Interface Base Class
 *
 * This class provides basic read and write methods for simple interfaces.
 * For example, it can be used as a base class for interfaces that only need to
 * send and receive single bytes of data.
 */
class SimpleInterfaceBase
{
public:
	explicit SimpleInterfaceBase() = default;
	virtual ~SimpleInterfaceBase() = default;

	/**
	 * Write a single byte of data
	 *
	 * @param data Byte to write
	 *
	 * @return Reference to the interface object
	 */
	virtual SimpleInterfaceBase& WriteByte(uint8_t data)
	{
		(void)data;
		return *this;
	}

	/**
	 * Read a single byte of data
	 *
	 * @return Read byte
	 */
	virtual uint8_t ReadByte()
	{
		return 0;
	}

	/**
	 * Write a single word of data
	 *
	 * @param data Word to write
	 *
	 * @return Reference to the interface object
	 */
	virtual SimpleInterfaceBase& WriteWord(uint16_t data)
	{
		(void)data;
		return *this;
	}

	/**
	 * Read a single word of data
	 *
	 * @return Read word
	 */
	virtual uint16_t ReadWord()
	{
		return 0;
	}
};

/**
 * GPIO Interface Base Class
 */
class GPIO_InterfaceBase
{
public:
	explicit GPIO_InterfaceBase() = default;
	virtual ~GPIO_InterfaceBase() = default;

	/**
	 * Read pin state
	 *
	 * @param pin GPIO pin number
	 *
	 * @return Pin state
	 */
	virtual int Read(uint16_t port, uint16_t pin) = 0;

	/**
	 * Write pin state
	 *
	 * @param pin GPIO pin number
	 * @param state Pin state
	 */
	virtual void Write(uint16_t port, uint16_t pin, int state) = 0;
};

/**
 * I2C Interface Base Class
 */
class I2C_InterfaceBase
{
public:
	explicit I2C_InterfaceBase() = delete;
	virtual ~I2C_InterfaceBase() = default;

	/**
	 * Constructor
	 *
	 * @param bus_number I2C bus number
	 */
	explicit
	I2C_InterfaceBase(uint8_t bus_number) : bus_number_(bus_number) {}

	/**
	 * Write data to I2C device (optional)
	 *
	 * @param device_addr I2C device address
	 * @param reg_addr Register address to write to
	 * @param data Data to write
	 * @param length Length of data to write
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int Write(uint8_t device_addr, uint8_t reg_addr,
					  const uint8_t *data, uint32_t length) { return 0; };

	/**
	 * Read data from I2C device
	 *
	 * @param device_addr I2C device address
	 * @param reg_addr Register address to read from
	 * @param data Buffer to store read data
	 * @param length Length of data to read
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int Read(uint8_t device_addr, uint8_t reg_addr,
					 uint8_t *data, uint32_t length) = 0;
protected:
	uint8_t bus_number_;
};

/**
 * SPI Interface Base Class
 */
class SPI_InterfaceBase : public SimpleInterfaceBase
{
public:
	explicit SPI_InterfaceBase() = delete;
	virtual ~SPI_InterfaceBase() = default;

	/**
	 * Constructor
	 *
	 * @param bus_number SPI bus number
	 */
	explicit
	SPI_InterfaceBase(uint8_t bus_number) : bus_number_(bus_number) {}
	/**
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int Transfer(const uint8_t *tx_data,
						 uint8_t *rx_data,
						 uint32_t length) = 0;

	/**
	 * Write a single byte of data
	 *
	 * @param data Byte to write
	 * @return Reference to the interface object
	 */
	virtual SimpleInterfaceBase& WriteByte(uint8_t data) override
	{
		Transfer(&data, nullptr, 1);
		return *this;
	}

	/**
	 * Write a single word of data
	 *
	 * @param data Word to write
	 * @return Reference to the interface object
	 */
	virtual SimpleInterfaceBase& WriteWord(uint16_t data) override
	{
		uint8_t tx_data[2];
		tx_data[0] = (data >> 8) & 0xFF;
		tx_data[1] = (data >> 0) & 0xFF;
		Transfer(tx_data, nullptr, 2);
		return *this;
	}

	/**
	 * Read a single byte of data
	 *
	 * @return Read byte
	 */
	virtual uint8_t ReadByte() override
	{
		uint8_t data = 0;
		Transfer(nullptr, &data, 1);
		return data;
	}

	/**
	 * Read a single word of data
	 *
	 * @return Read word
	 */
	virtual uint16_t ReadWord() override
	{
		uint8_t rx_data[2] = {0};
		Transfer(nullptr, rx_data, 2);
		return (static_cast<uint16_t>(rx_data[0]) << 8) |
			   (static_cast<uint16_t>(rx_data[1]) << 0);
	}
protected:
	uint8_t bus_number_;
};

/**
 * UART Interface Base Class
 */
class UART_InterfaceBase : public SimpleInterfaceBase
{
public:
	explicit UART_InterfaceBase() = delete;
	virtual ~UART_InterfaceBase() = default;

	/**
	 * Constructor
	 *
	 * @param port_number UART port number
	 */
	explicit
	UART_InterfaceBase(uint8_t port_number) : port_number_(port_number) {}

	/**
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int SendReceive(const uint8_t *tx_data, uint8_t *rx_data, uint32_t length) = 0;
protected:
	uint8_t port_number_;
};

/**
 * One-Wire Interface Base Class
 */
class OneWire_InterfaceBase
{
public:
	explicit OneWire_InterfaceBase() = default;
	virtual ~OneWire_InterfaceBase() = default;

	/**
	 * Result of the Reset operation
	 */
	enum Result {
		Success = 0,
		NoDevice = 1,
		CrcMissmatch = 2,
		NotReady = 3,
		NotAllDevicesFound = 4
	};

	/**
	 * Perform a reset on the One-Wire bus (Optional)
	 *
	 * @return Result of the reset operation
	 */
	virtual Result Reset() { return Success; };

	/**
	 * Write a byte to the One-Wire bus
	 *
	 * @param value Byte to write
	 */
	virtual void Write(uint8_t value) = 0;

	/**
	 * Read a byte from the One-Wire bus
	 *
	 * @return Read byte
	 */
	virtual uint8_t Read() = 0;

	/**
	 * Write a single bit to the One-Wire bus (optional)
	 *
	 * @param bit Bit value to write (true = 1, false = 0)
	 */
	virtual void WriteBit(uint8_t bit) { }

	/**
	 * Read a single bit from the One-Wire bus (optional) 
	 *
	 * @return Read bit value (true = 1, false = 0)
	 */
	virtual uint8_t ReadBit() { return 0; }
};

#endif // INTERFACES_H
