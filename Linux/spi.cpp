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
 * Linux SPI interface driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "spi.h"
#include <fcntl.h>
#include <string>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

void SPI::Init(const char *dev)
{
	fd_ = open(dev, O_RDWR);
	if (fd_ < 0) {
		throw std::runtime_error("Failed to open SPI device: " + std::string(dev) + \
			", error: " + strerror(errno));
	}
};

SPI::~SPI()
{
	if (fd_ >= 0) {
		close(fd_);
	}
}

int SPI::Transfer(const uint8_t *tx_data,
	uint8_t *rx_data,
	uint32_t length)
{
	struct spi_ioc_transfer xfer = {
		reinterpret_cast<uint64_t>(tx_data),
		reinterpret_cast<uint64_t>(rx_data),
		length,
		0, // speed_hz (use default)
		0, // delay_usecs
		0, // bits_per_word (use default)
		0, // cs_change
		0, // tx_nbits
		0, // rx_nbits
		0, // word_delay_usecs
		0, // pad
	};

	if (ioctl(fd_, SPI_IOC_MESSAGE(1), &xfer) < 0) {
		throw std::runtime_error("SPI transfer failed: " + std::string(strerror(errno)));
		return errno;
	}

	return 0;
}
