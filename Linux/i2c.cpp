/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2026 Pavel Nadein
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

#include <sys/ioctl.h>
#include "i2c.h"
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

I2C::I2C(const char *dev)
{
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		throw std::runtime_error("Failed to open I2C device: " +
			std::string(dev) + ", error: " + std::strerror(errno));
	}
}

int I2C::Write(uint8_t device_addr,
			   const uint8_t *reg_addr,
			   uint16_t reg_addr_length,
			   const uint8_t *data,
			   uint32_t data_length)
{
	struct i2c_msg msg[2] = {
		{ device_addr,	0,				reg_addr_length,					const_cast<uint8_t *>(reg_addr) },
		{ device_addr,	I2C_M_NOSTART,	static_cast<uint16_t>(data_length),	const_cast<uint8_t *>(data) }
	};

	struct i2c_rdwr_ioctl_data msgset = { msg, 2 };

	if (ioctl(fd, I2C_RDWR, &msgset))
		throw std::runtime_error("Failed to write to I2C device, error: " +
			std::string(std::strerror(errno)));

	return 0;
}

int I2C::Read(uint8_t device_addr,
			  const uint8_t *reg_addr,
			  uint16_t reg_addr_length,
			  uint8_t *data,
			  uint32_t data_length)
{
	struct i2c_msg msg[2] = {
		{ device_addr,	0,				reg_addr_length,					const_cast<uint8_t *>(reg_addr) },
		{ device_addr,	I2C_M_RD,		static_cast<uint16_t>(data_length),	data }
	};

	struct i2c_rdwr_ioctl_data msgset = { msg, 2 };

	if (ioctl(fd, I2C_RDWR, &msgset))
		throw std::runtime_error("Failed to read from I2C device, error: " +
			std::string(std::strerror(errno)));

	return 0;
}

I2C::~I2C()
{
	if (fd >= 0)
		close(fd);
}
