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
 * Generic Linux GPIO driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include "gpio.h"

int gpio_open(struct gpio_t *dev, const char *name)
{
	dev->name = name ? name : "/dev/gpiochip0";
	dev->fd = open(dev->name, O_RDONLY);
	return dev->fd < 0 ? -ENODEV : 0;
}

int gpio_request(struct gpio_t *dev, enum gpio gpio,
	enum pin pin, enum gpio_mode mode, const char *name)
{
	int ret;
	struct gpiohandle_request req = { 0 };

	req.default_values[gpio] = 0;
	req.lineoffsets[gpio] = pin;
	req.flags = mode;
	req.lines = 1;
	if (name)
		strcpy(req.consumer_label, name);

	ret = ioctl(dev->fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
	if (ret < 0)
		return ret;

	return req.fd;
}

void gpio_release(int fd)
{
	close(fd);
}

int gpio_write(int fd, bool state)
{
	struct gpiohandle_data data = {
		.values[0] = state,
	};

	return ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
}

bool gpio_read(int fd)
{
	struct gpiohandle_data data;

	ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
	return data.values[0];
}

int gpio_get_name(int fd, char *name)
{
	int ret;
	struct gpioline_info info;

	ret = ioctl(fd, GPIO_GET_LINEINFO_IOCTL, &info);
	if (ret > 0)
		strcpy(name, info.consumer);

	return ret;
}

void gpio_close(struct gpio_t *dev)
{
	close(dev->fd);
}