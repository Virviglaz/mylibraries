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

/* Example of use */
/*
	To access the pin the handle will be assigned:
	int pin4;
	struct gpio_t gpio0;

	Opening device:
	if (!gpio_open(&gpio0, "/dev/gpiochip0")) {

		Obtain the handle:
		pin4 = gpio_request(&gpio0, GPIOA, PIN_4, GPIO_OUTPUT, "pin4 output");

		Use pin:
		gpio_write(pin4, true);

		Close pin handle:
		gpio_release(pin4);

		Close device handle:
		gpio_close(&gpio0);
	}
*/

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

enum gpio {
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
};

enum pin {
	PIN_0,
	PIN_1,
	PIN_2,
	PIN_3,
	PIN_4,
	PIN_5,
	PIN_6,
	PIN_7,
	PIN_8,
	PIN_9,
	PIN_10,
	PIN_11,
	PIN_12,
	PIN_13,
	PIN_14,
	PIN_15,
	PIN_16,
	PIN_17,
	PIN_18,
	PIN_19,
	PIN_20,
	PIN_21,
	PIN_22,
	PIN_23,
	PIN_24,
	PIN_25,
	PIN_26,
	PIN_27,
	PIN_28,
	PIN_29,
	PIN_30,
	PIN_31,
};

enum gpio_mode {
	GPIO_INPUT			= (1UL << 0),
	GPIO_OUTPUT			= (1UL << 1),
	GPIO_ACTIVE_LOW		= (1UL << 2),
	GPIO_OPEN_DRAIN		= (1UL << 3),
	GPIO_OPEN_SOURCE	= (1UL << 4),
	GPIO_PULL_UP		= (1UL << 5),
	GPIO_PULL_DOWN		= (1UL << 6),
	GPIO_BIAS_DISABLE	= (1UL << 7),
};

struct gpio_t {
	const char *name;
	int fd;
};

int gpio_open(struct gpio_t *dev, const char *name);
int gpio_request(struct gpio_t *dev, enum gpio gpio,
	enum pin pin, enum gpio_mode mode, const char *name);
void gpio_release(int fd);
int gpio_write(int fd, bool state);
bool gpio_read(int fd);
int gpio_get_name(int fd, char *name);
void gpio_close(struct gpio_t *dev);

#endif /* GPIO_H */