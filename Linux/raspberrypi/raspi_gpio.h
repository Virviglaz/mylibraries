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

#ifndef __RASPI_GPIO_H__
#define __RASPI_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

#if (!defined(BCM2835) && !defined(BCM_OTHERS))
#define BCM2835
#define GPIO_ADDR_START	0x00200000
#endif

struct __attribute__((__packed__)) bcm2835_gpio {
	uint32_t GPFSEL[6];	/* GPIO Function Select 0-5 */
	uint32_t res0;
	uint32_t GPSET[2];	/* GPIO Pin Output Set 0-1 */
	uint32_t res1;
	uint32_t GPCLR[2];	/* GPIO Pin Output Clear 0-1 */
	uint32_t res2;
	uint32_t GPLEV[2];	/* GPIO Pin Level 0-1 */
	uint32_t res3;
	uint32_t GPEDS[2];	/* GPIO Pin Event Detect Status 0-1 */
	uint32_t res4;
	uint32_t GPREN[2];	/* GPIO Pin Rising Edge Detect Enable 0-1 */
	uint32_t res5;
	uint32_t GPFEN[2];	/* GPIO Pin Falling Edge Detect Enable 0-1 */
	uint32_t res6;
	uint32_t GPHEN[2];	/* GPIO Pin High Detect Enable 0-1 */
	uint32_t res7;
	uint32_t GPLEN[2];	/* GPIO Pin Low Detect Enable 0-1 */
	uint32_t res8;
	uint32_t GPAREN[2];	/* GPIO Pin Async. Rising Edge Detect 0-1 */
	uint32_t res9;
	uint32_t GPAFEN[2];	/* GPIO Pin Async. Falling Edge Detect 0-1 */
	uint32_t res10;
	uint32_t GPPUD;		/* GPIO Pin Pull-up/down Enable */
	uint32_t GPPUDCLK[2];	/* GPIO Pin Pull-up/down Enable Clock 0-1 */
	uint32_t res11;
	uint32_t test;
};

#ifdef BCM2835
typedef struct bcm2835_gpio raspi_gpio_t;

/* NOTE: order is weird */
enum gpio_func {
	GPIO_INPUT	= 0,
	GPIO_OUTPUT	= 1,
	GPIO_ALT5	= 2,
	GPIO_ALT4	= 3,
	GPIO_ALT0	= 4,
	GPIO_ALT1	= 5,
	GPIO_ALT2	= 6,
	GPIO_ALT3	= 7,
	GPIO_ERROR,
};

#endif

int gpio_init(int gpio, enum gpio_func func);
enum gpio_func gpio_func(int gpio_num);
void gpio_set(int gpio_num);
void gpio_clr(int gpio_num);
int gpio_read(int gpio_num);
void gpio_close(void);

#ifdef __cplusplus
}
#endif

#endif /* __RASPI_GPIO_H__ */
