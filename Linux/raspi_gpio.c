/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2021 Pavel Nadein
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
 * RaspberryPi Linux GPIO driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "raspi_gpio.h"

#ifndef GPIO_DEV_NAME
#define GPIO_DEV_NAME	"/dev/gpiomem"
#endif

static raspi_gpio_t *gpio = 0;
static int fd;

static int init(void)
{
	void *mem;

	if (gpio)
		return 0;

	fd = open(GPIO_DEV_NAME, O_RDWR | O_SYNC);
	if (fd < 0)
		return fd;

	mem = mmap(0, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED,
		fd, GPIO_ADDR_START);
	if (mem == MAP_FAILED) {
		int res = errno;
		gpio = (raspi_gpio_t *)0;
		close(fd);
		return res;
	}

	gpio = (raspi_gpio_t *)mem;

	return 0;
}

void gpio_close(void)
{
	munmap((void *)gpio, sizeof(raspi_gpio_t));
	gpio = (raspi_gpio_t *)0;

	close(fd);
}

int gpio_init(int gpio_num, enum gpio_func func)
{
	uint32_t *reg = 0;
	int res = init();
	if (res)
		return res;

	if (gpio_num <= 9)
		reg = &gpio->GPFSEL[0];
	else if (gpio_num <= 19)
		reg = &gpio->GPFSEL[1];
	else if (gpio_num <= 29)
		reg = &gpio->GPFSEL[2];
	else if (gpio_num <= 39)
		reg = &gpio->GPFSEL[3];
	else if (gpio_num <= 49)
		reg = &gpio->GPFSEL[4];
	else if (gpio_num <= 53)
		reg = &gpio->GPFSEL[5];

	if (!reg)
		return EINVAL;

	gpio_num %= 10;
	gpio_num *= 3;
	*reg &= (7 << gpio_num);
	*reg |= ((uint32_t)func << gpio_num);

	return 0;
}

void gpio_set(int gpio_num)
{
	if (gpio_num <= 31)
		gpio->GPSET[0] = 1 << gpio_num;
	else
		gpio->GPSET[1] = 1 << (gpio_num - 31);
}

void gpio_clr(int gpio_num)
{
	if (gpio_num <= 31)
		gpio->GPCLR[0] = 1 << gpio_num;
	else
		gpio->GPCLR[1] = 1 << (gpio_num - 31);
}

int gpio_read(int gpio_num)
{
	if (gpio_num <= 31)
		return gpio->GPLEV[0] & (1 << gpio_num) ? 1 : 0;

	gpio_num -= 31;
	return gpio->GPLEV[1] & (1 << gpio_num) ? 1 : 0;
}
