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
 * I2C over GPIO driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "i2c_gpio.h"

#define ACK		0
#define NACK		1
#define HIGH		1
#define LOW		0

#ifndef I2C_GPIO_NO_DELAYS
#define DELAY()		if (dev->delay_func) \
				dev->delay_func(dev->delay)
#else
#define DELAY()
#endif

const char *errors[] = {
	"SUCCESS",
	"TIMEOUT",
	"ERROR",
	"BUS BUSY",
	"ACK OK",
	"ACK NOT OK",
	"ADD NOT EXIST",
	"VERIFY ERROR",
	"INTERFACE ERROR"
};

static enum i2c_gpio_res_t start(const struct i2c_gpio_t *dev)
{
	dev->sda_wr(HIGH);
	dev->scl_wr(HIGH);
	DELAY();

	/*
	 * Check, if any BUS signal is low that meas that BUS is busy
	 * by other device or H/W issue
	 */
	if (!dev->sda_rd())
		return I2C_BUS_BUSY;

	dev->sda_wr(LOW);
	DELAY();
	dev->scl_wr(LOW);
	DELAY();

	return I2C_SUCCESS;
}

static void restart(const struct i2c_gpio_t *dev)
{
	dev->sda_wr(HIGH);
	dev->scl_wr(HIGH);
	DELAY();
	dev->sda_wr(LOW);
	DELAY();
	dev->scl_wr(LOW);
}

static void stop(const struct i2c_gpio_t *dev)
{
	DELAY();
	dev->sda_wr(LOW);
	dev->scl_wr(HIGH);
	DELAY();
	dev->sda_wr(HIGH);
	DELAY();
}

static uint8_t clock(const struct i2c_gpio_t *dev)
{
	uint8_t res;

	dev->scl_wr(HIGH);
	DELAY();
	res = dev->sda_rd();
	dev->scl_wr(LOW);
	DELAY();

	return res;
}

static uint8_t write(const struct i2c_gpio_t *dev, uint8_t data)
{
	uint8_t mask = 0x80;

	while (mask) {
		(data & mask) ? dev->sda_wr(HIGH) : dev->sda_wr(LOW);
		clock(dev);
		mask >>= 1;
	}

	dev->sda_wr(HIGH);
	return clock(dev);
}

static uint8_t read(const struct i2c_gpio_t *dev, uint8_t ack)
{
	uint8_t data = 0, mask = 0x80;

	while(mask) {
		if (clock(dev))
			data |= mask;
  		mask >>= 1;
	}

	if (ack) {
		dev->sda_wr(LOW);
		clock(dev);
		dev->sda_wr(HIGH);
	} else {
		dev->sda_wr(HIGH);
		clock(dev);
	}

	return data;
}

enum i2c_gpio_res_t i2c_write(const struct i2c_gpio_t *dev, uint8_t i2c_addr,
			      uint8_t *addr, uint8_t addr_len,
			      uint8_t *buf, uint16_t size)
{
	enum i2c_gpio_res_t res = I2C_ADD_NOT_EXIST;

	/* Check BUS */
	if (start(dev) != I2C_SUCCESS)
		return I2C_BUS_BUSY;

	/* Send device address for write */
	if (write(dev, i2c_addr << 1) == ACK) {
		res = I2C_SUCCESS;

		while (addr_len--)
			res = (enum i2c_gpio_res_t)write(dev, *addr++);

		while(size-- && res == I2C_SUCCESS) //send buffer
			res = (enum i2c_gpio_res_t)write(dev, *buf++);
	}

	stop(dev);
	return res;
}

enum i2c_gpio_res_t i2c_read(const struct i2c_gpio_t *dev, uint8_t i2c_addr,
			      uint8_t *addr, uint8_t addr_len,
			      uint8_t *buf, uint16_t size)
{
	enum i2c_gpio_res_t res = I2C_ADD_NOT_EXIST;

	/* Check BUS */
	if (start(dev) != I2C_SUCCESS)
		return I2C_BUS_BUSY;

	/* Send device address for write */
	if (write(dev, i2c_addr << 1) == ACK) {
		res = (enum i2c_gpio_res_t)ACK;

		while(addr_len--)
			res = (enum i2c_gpio_res_t)write(dev, *addr++);

		restart(dev);

		if (write(dev, (i2c_addr << 1) | 0x01) == I2C_SUCCESS) {
			while (size--)
				*buf++ = read(dev, size ? 1 : 0);
			res = I2C_SUCCESS;
		}
	}

	stop(dev);
	return res;
}


