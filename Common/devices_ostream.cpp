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

#include "devices_ostream.h"
#include <iostream>
#include <stdexcept>

static void print_data(const uint8_t *data, uint32_t length)
{
	std::cout << "Data: ";
	for (uint32_t i = 0; i < length; ++i)
	{
		std::cout << std::hex << static_cast<int>(data[i]) << " ";
	}
	std::cout << std::dec << std::endl;
}

void GPIO_DeviceOStream::Set(uint16_t state)
{
	if (dir_ != GPIO_DeviceBase::dir::OUTPUT)
	{
		throw std::invalid_argument("GPIO configured as intput");
	}

	std::cout << "GPIO Device " << name_ << ": " << (state ? "1" : "0") << std::endl;
}

int GPIO_DeviceOStream::Get()
{
	if (dir_ != GPIO_DeviceBase::dir::INPUT)
	{
		throw std::invalid_argument("GPIO configured as output");
	}

	std::cout << "GPIO Device " << name_ << " reading: " << read_value_ << std::endl;

	return read_value_;
}

int I2C_DeviceOStream::Write(uint8_t reg_addr,
							 const uint8_t *data,
							 uint32_t length)
{
	if (!data || length == 0)
	{
		throw std::invalid_argument("No data to write");
	}

	std::cout << "I2C Device " << name_ << ": write to reg "
			  << static_cast<int>(reg_addr)
			  << " length " << length << std::endl;
	print_data(data, length);
	return 0;
}

int I2C_DeviceOStream::Read(uint8_t reg_addr,
							uint8_t *data,
							uint32_t length)
{
	if (!data || length == 0)
	{
		throw std::invalid_argument("No data to read");
	}

	std::cout << "I2C Device " << name_ << ": read from reg "
			  << static_cast<int>(reg_addr)
			  << " length " << length << std::endl;
	return 0;
}

int SPI_DeviceOStream::Transfer(const uint8_t *tx_data,
								uint8_t *rx_data,
								uint32_t length)
{
	if (length == 0)
	{
		throw std::invalid_argument("No data to transfer");
	}

	std::cout << "SPI Device " << name_ << ": transfer "
			  << static_cast<int>(length) << " bytes." << std::endl;

	if (tx_data)
		print_data(tx_data, length);

	return 0;
}
