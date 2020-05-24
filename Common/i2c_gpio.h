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

#ifndef I2C_GPIO_H
#define I2C_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

struct i2c_gpio_t {
#ifndef I2C_GPIO_NO_DELAYS
	/* Delay value */
	uint16_t delay;

	/* Functions */
	void (* delay_func)(uint16_t us);
#endif
	void (* scl_wr)(uint8_t state);
	void (* sda_wr)(uint8_t state);
	uint8_t (* sda_rd)(void);
};

enum i2c_gpio_res_t {
	I2C_SUCCESS = 0,
	I2C_TIMEOUT,
	I2C_ERROR,
	I2C_BUS_BUSY,
	I2C_ACK_OK,
	I2C_ACK_NOT_OK,
	I2C_ADD_NOT_EXIST,
	I2C_VERIFY_ERROR,
	I2C_INTERFACE_ERROR,
};

extern const char *errors[];

enum i2c_gpio_res_t i2c_write(const struct i2c_gpio_t *dev, uint8_t i2c_addr,
			      uint8_t *addr, uint8_t addr_len,
			      uint8_t *buf, uint16_t size);

enum i2c_gpio_res_t i2c_read(const struct i2c_gpio_t *dev, uint8_t i2c_addr,
			      uint8_t *addr, uint8_t addr_len,
			      uint8_t *buf, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* I2C_GPIO_H */
