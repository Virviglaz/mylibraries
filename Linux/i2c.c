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
#include <linux/i2c.h>
#include <errno.h>
#include <string.h>
#include "i2c.h"

int i2c_wr(const char *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
	uint8_t buf[size + sizeof(reg)];
	struct i2c_msg msg = {
		.addr = addr,
		.flags = 0,
		.len = size + sizeof(reg),
		.buf = buf
	};
	struct i2c_rdwr_ioctl_data msgset = {
		.msgs = &msg,
		.nmsgs = 1
	};
	int fd, ret = 0;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return errno;

	buf[0] = reg;
	memcpy(buf + sizeof(reg), data, size);

	if (ioctl(fd, I2C_RDWR, &msgset))
		ret = errno;

	close(fd);

	return ret;
}

int i2c_rd(const char *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
	struct i2c_msg msg[] = {{
		.addr = addr,
		.flags = 0,
		.len = sizeof(reg),
		.buf = &reg }, {
		.addr = addr,
		.flags = I2C_M_RD, I2C_M_NOSTART,
		.buf = data,
		.len = size
	}};
	struct i2c_rdwr_ioctl_data msgset = {
		.msgs = msg,
		.nmsgs = 2
	};
	int fd, ret = 0;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return errno;

	if (ioctl(fd, I2C_RDWR, &msgset))
		ret = errno;

	close(fd);

	return ret;
}
