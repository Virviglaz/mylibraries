/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2025-2026 Pavel Nadein
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

#ifndef INTERFACES_H
#define INTERFACES_H

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

#include <cstdint>
#include <cstddef>

/**
 * I2C Interface Base Class
 */
class I2C_InterfaceBase
{
public:
	explicit I2C_InterfaceBase() = default;
	virtual ~I2C_InterfaceBase() = default;

	/**
	 * @brief Write data to I2C device
	 *
	 * @param device_addr I2C device address
	 * @param reg_addr Pointer to register address to write to
	 * @param reg_addr_length Length of the register address (1, 2 or 4 bytes)
	 * @param data Pointer to data to write
	 * @param data_length Length of data to write
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int Write(uint8_t device_addr,
					  const uint8_t *reg_addr,
					  size_t reg_addr_length,
					  const uint8_t *data,
					  size_t data_length) = 0;

	/**
	 * @brief Read data from I2C device
	 *
	 * @param device_addr I2C device address
	 * @param reg_addr Pointer to register address to read from
	 * @param reg_addr_length Length of the register address (1, 2 or 4 bytes)
	 * @param data Buffer to store read data
	 * @param data_length Length of data to read
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int Read(uint8_t device_addr,
					 const uint8_t *reg_addr,
					 size_t reg_addr_length,
					 uint8_t *data,
					 size_t data_length) = 0;
};

/**
 * SPI Interface Base Class
 */
class SPI_InterfaceBase
{
public:
	explicit SPI_InterfaceBase() = default;
	virtual ~SPI_InterfaceBase() = default;

	/**
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int Transfer(const uint8_t *tx_data,
						 uint8_t *rx_data,
						 size_t length) = 0;
};

/**
 * UART Interface Base Class
 */
class UART_InterfaceBase 
{
public:
	explicit UART_InterfaceBase() = default;
	virtual ~UART_InterfaceBase() = default;

	/**
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int SendReceive(const uint8_t *tx_data, uint8_t *rx_data, size_t length) = 0;
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
	enum class Result {
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
	virtual Result Reset() { return Result::Success; };

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

class Time_InterfaceBase
{
public:
	explicit Time_InterfaceBase() = default;
	virtual ~Time_InterfaceBase() = default;

	/**
	 * Get current time in microseconds since the last reset.
	 *
	 * @return Time in microseconds
	 */
	virtual uint64_t GetTimeUs() = 0;

	/**
	 * Reset the time counter.
	 */
	virtual void ResetTime() = 0;
};

#endif // INTERFACES_H
