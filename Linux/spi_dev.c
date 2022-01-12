/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020 Pavel Nadein
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
 * Generic Linux SPI driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include "spi_dev.h"

#define DEFAULT_SPI_DEV			"/dev/spidev0.0"

static struct spi_dev *local_driver;

/* Optional debug output function */
static int debug(const char *format, ...)
{	/* TODO: works incorrect */
	if (local_driver->dbg) {
		va_list arg;
		va_start(arg, format);
		local_driver->dbg(format, arg);
		va_end(arg);
	}
	return 0;
}

static uint8_t send_receive(uint8_t *data, uint16_t size, bool send)
{
	struct spi_ioc_transfer xfer;

	memset(&xfer, 0, sizeof(xfer));

	if (send)
		xfer.tx_buf = (unsigned long)data;
	else
		xfer.rx_buf = (unsigned long)data;

	xfer.len = size;

	if (ioctl(local_driver->fd, SPI_IOC_MESSAGE(1), &xfer) < 0)
		debug("%s: Error sending data, errno: %s\n",
			local_driver->name, strerror(errno));

	return data[0];
}

static uint8_t send_receive_reg(uint8_t reg, uint8_t *data, uint16_t size, bool send)
{
	uint8_t res;
	struct spi_ioc_transfer xfer[2];

	memset(xfer, 0, sizeof(xfer));

	xfer[0].rx_buf = (unsigned long)&res;
	xfer[0].tx_buf = (unsigned long)&reg;
	xfer[0].len = sizeof(reg);

	xfer[1].tx_buf = send ? (unsigned long)data : 0;
	xfer[1].rx_buf = send ? 0 : (unsigned long)data;
	xfer[1].len = size;

	if (ioctl(local_driver->fd, SPI_IOC_MESSAGE(size ? 2 : 1), xfer) < 0)
		debug("%s: Error sending reg data, errno: %s\n",
			local_driver->name, strerror(errno));

	return res;
}

static uint8_t send(uint8_t *data, uint16_t size)
{
	return send_receive(data, size, true);
}

static uint8_t recv(uint8_t *data, uint16_t size)
{
	return send_receive(data, size, false);
}

static uint8_t send_reg(uint8_t reg, uint8_t *data, uint16_t size)
{
	return send_receive_reg(reg, data, size, true);
}

static uint8_t recv_reg(uint8_t reg, uint8_t *data, uint16_t size)
{
	return send_receive_reg(reg, data, size, false);
}

struct spi_dev *spi_init(struct spi_dev *driver, const char *name)
{
	/* If NULL we allocate the driver by ourselfs */
	if (!driver) {
		driver = malloc(sizeof(struct spi_dev));
		if (!driver) {
			debug("%s: Error! No memory\n", name);
			return NULL;
		}
		memset(driver, 0, sizeof(struct spi_dev));
		driver->is_allocated = true;
	} else
		driver->is_allocated = false;

	local_driver = driver;
	local_driver->send = send;
	local_driver->recv = recv;
	local_driver->send_reg = send_reg;
	local_driver->recv_reg = recv_reg;
	local_driver->name = name ? name : DEFAULT_SPI_DEV;

	/* Open the device */
	local_driver->fd = open(local_driver->name, O_RDWR);

	if (local_driver->fd < 0) {
		debug("%s: Fail to open\n", name);
		return NULL;
	}

	if (ioctl(local_driver->fd, SPI_IOC_WR_MODE, &local_driver->mode))
		debug("IOCTL SPI_IOC_WR_MODE failed at line %u\n", __LINE__);

	if (ioctl(local_driver->fd, SPI_IOC_WR_BITS_PER_WORD, 
		&local_driver->bits_per_word))
		debug("IOCTL SPI_IOC_WR_BITS_PER_WORD failed at line %u\n", __LINE__);

	if (ioctl(local_driver->fd, SPI_IOC_WR_MAX_SPEED_HZ,
		&local_driver->freq))
		debug("IOCTL SPI_IOC_WR_MAX_SPEED_HZ failed at line %u\n", __LINE__);

	return local_driver;
}

void spi_deinit(void)
{
	close(local_driver->fd);

	if (local_driver->is_allocated)
		free(local_driver);
}
