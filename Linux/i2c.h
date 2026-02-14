/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020-2026 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Generic Linux I2C driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>

#ifdef __cplusplus
#include "interfaces.h"

/**
 * Linux I2C Interface Implementation
 */
class I2C : public I2C_InterfaceBase
{
public:
	/**
	 * @brief Default constructor (does not initialize the interface)
	 */
	explicit I2C() = default;

	/**
	 * Constructor (initializes the interface)
	 *
	 * @param dev I2C device file (e.g. "/dev/i2c-1")
	 * @throws std::runtime_error if initialization fails
	 */
	explicit I2C(const char *dev);
	virtual ~I2C() override;

	/**
	 * @brief Initialize I2C interface
	 *
	 * @param dev I2C device file (e.g. "/dev/i2c-1")
	 * @throws std::runtime_error if initialization fails
	 */
	void Init(const char *dev);

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
					  uint16_t reg_addr_length,
					  const uint8_t *data,
					  uint32_t data_length) override;

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
					 uint16_t reg_addr_length,
					 uint8_t *data,
					 uint32_t data_length) override;

private:
	int fd;
};

#else /* C */
int i2c_wr(const char *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size);
int i2c_rd(const char *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size);
#endif /* __cplusplus */

#endif /* __I2C_H__ */
