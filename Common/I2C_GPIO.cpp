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
 * Software I2C implementation using GPIO pins source file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "I2C_GPIO.h"
#include <errno.h>

int I2C_GPIO::StartCondition()
{
	sda_pin_.Set(1);
	scl_pin_.Set(1);
	delay_func_();

	/* Check bus is busy */
	if (sda_pin_.Get() == 0 || scl_pin_.Get() == 0)
		return -EBUSY;

	sda_pin_.Set(0);
	delay_func_();
	scl_pin_.Set(0);
	delay_func_();
	return 0;
}

void I2C_GPIO::RepeatStartCondition()
{
	sda_pin_.Set(1);
	scl_pin_.Set(1);
	delay_func_();
	sda_pin_.Set(0);
	delay_func_();
	scl_pin_.Set(0);
	delay_func_();
}

void I2C_GPIO::StopCondition()
{
	delay_func_();
	sda_pin_.Set(0);
	scl_pin_.Set(1);
	delay_func_();
	sda_pin_.Set(1);
	delay_func_();
}

uint16_t I2C_GPIO::WriteByte(uint8_t data)
{
	uint8_t mask = 0x80;
	uint16_t ack;

	while (mask)
	{
		if (data & mask)
			sda_pin_.Set(1);
		else
			sda_pin_.Set(0);
		scl_pin_.Set(1);
		delay_func_();
		scl_pin_.Set(0);
		delay_func_();
		mask >>= 1;
	}

	sda_pin_.Set(1); // Release SDA for ACK bit
	delay_func_();
	scl_pin_.Set(1);
	delay_func_();
	ack = sda_pin_.Get();
	scl_pin_.Set(0);
	delay_func_();

	return ack;
}

uint16_t I2C_GPIO::ReadByte(bool ack)
{
	uint8_t data = 0, mask = 0x80;

	sda_pin_.Set(1); // Release SDA for reading

	while (mask)
	{
		scl_pin_.Set(1);
		delay_func_();
		if (sda_pin_.Get())
			data |= mask;
		scl_pin_.Set(0);
		delay_func_();
		mask >>= 1;
	}

	// Send ACK/NACK bit
	if (ack)
		sda_pin_.Set(0);
	else
		sda_pin_.Set(1);
	delay_func_();
	scl_pin_.Set(1);
	delay_func_();
	scl_pin_.Set(0);
	delay_func_();
	sda_pin_.Set(1); // Release SDA

	return data;
}

int I2C_GPIO::Write(uint8_t device_addr, uint8_t reg_addr,
					  const uint8_t *data, uint32_t length)
{
	if (StartCondition() != 0)
		return -EBUSY;

	// Send device address with write flag
	if (WriteByte((device_addr << 1) | 0) != 0)
	{
		StopCondition();
		return -EIO;
	}

	// Send register address
	if (WriteByte(reg_addr) != 0)
	{
		StopCondition();
		return -EIO;
	}

	// Send data bytes
	for (uint32_t i = 0; i < length; i++)
	{
		if (WriteByte(data[i]) != 0)
		{
			StopCondition();
			return -EIO;
		}
	}

	StopCondition();
	return 0;
}

int I2C_GPIO::Read(uint8_t device_addr, uint8_t reg_addr,
					 uint8_t *data, uint32_t length)
{
	if (StartCondition() != 0)
		return -EBUSY;

	// Send device address with write flag
	if (WriteByte((device_addr << 1) | 0) != 0)
	{
		StopCondition();
		return -EIO;
	}

	// Send register address
	if (WriteByte(reg_addr) != 0)
	{
		StopCondition();
		return -EIO;
	}

	// Repeat start condition
	RepeatStartCondition();

	// Send device address with read flag
	if (WriteByte((device_addr << 1) | 1) != 0)
	{
		StopCondition();
		return -EIO;
	}

	// Read data bytes
	for (uint32_t i = 0; i < length; i++)
	{
		bool ack = (i < (length - 1));
		data[i] = ReadByte(ack);
	}

	StopCondition();
	return 0;
}
