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
 * Software I2C implementation using GPIO pins header file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef I2C_GPIO_H
#define I2C_GPIO_H

#include "interfaces.h"
#include "devices.h"
#include <functional>

class I2C_GPIO : public I2C_InterfaceBase
{
public:
	/**
	 * Constructor
	 *
	 * @param sda_pin GPIO pin for SDA line
	 * @param scl_pin GPIO pin for SCL line
	 */
	I2C_GPIO(GPIO_DeviceBase &sda_pin, GPIO_DeviceBase &scl_pin, std::function<void()> delay_func)
		: sda_pin_(sda_pin), scl_pin_(scl_pin), delay_func_(delay_func) {}

	/**
	 * Write data to I2C device
	 *
	 * @param device_addr I2C device address
	 * @param reg_addr Register address to write to
	 * @param data Data to write
	 * @param length Length of data to write
	 *
	 * @return 0 on success, negative value on error
	 */
	int Write(uint8_t device_addr, uint8_t reg_addr,
			  const uint8_t *data, uint32_t length) override;

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
	int Read(uint8_t device_addr, uint8_t reg_addr,
			 uint8_t *data, uint32_t length) override;

private:
	GPIO_DeviceBase &sda_pin_;
	GPIO_DeviceBase &scl_pin_;
	std::function<void()> delay_func_;

	int  StartCondition();
	void RepeatStartCondition();
	void StopCondition();
	uint16_t WriteByte(uint8_t data);
	uint16_t ReadByte(bool ack);
};

#endif /* I2C_GPIO_H */
