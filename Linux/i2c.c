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
 * Generic Linux I2C driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif
#include <errno.h>
#include <string.h>
#include "i2c.h"

static struct i2c_dev *last = NULL;

static uint8_t write_reg(struct i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
	uint8_t buf[size + 1];
	struct i2c_msg msg[] = {{ .addr = addr, .flags = 0, .len = size + 1, .buf = buf}};
	struct i2c_rdwr_ioctl_data msgset[1] = {{ .msgs = msg, .nmsgs = 1 }};

	buf[0] = reg;
	memcpy(buf + 1, data, size);
	return ioctl(dev ? dev->fd : last->fd, I2C_RDWR, &msgset) < 0 ? 1 : 0;
}

static uint8_t read_reg(struct i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
	struct i2c_msg msg[] = {
		{ addr, 0, 1, &reg },
		{ addr, I2C_M_RD | I2C_M_NOSTART, size, data }};
	struct i2c_rdwr_ioctl_data msgset[1] = {{ .msgs = msg, .nmsgs = 2 }};

	return ioctl(dev ? dev->fd : last->fd, I2C_RDWR, &msgset) < 0 ? 1 : 0;
}

int i2c_init(struct i2c_dev *dev, const char *name)
{
	last = dev;
	
	dev->name = name ? name : "/dev/i2c-0";
	dev->fd = open(dev->name, O_RDWR);
	if (dev->fd < 0)
		return -ENODEV;

	dev->wr_reg = write_reg;
	dev->rd_reg = read_reg;
	
	return dev->fd < 0 ? dev->fd : 0;
}

void i2c_close(struct i2c_dev *dev)
{
	close(dev->fd);
}