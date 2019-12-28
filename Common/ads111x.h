/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2019 Pavel Nadein
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
 * ADS111x x Ultra-Small, Low-Power, I2C-Compatible, 860-SPS, 16-Bit ADCs
 * with Internal Reference, Oscillator, and Programmable Comparator
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>

#define ADS111X_CONV_IN_PROGRESS	0x80
#define ADS111X_I2C_ADDR_GND		0x48
#define ADS111X_I2C_ADDR_VDD		0x49
#define ADS111X_I2C_ADDR_SDA		0x4A
#define ADS111X_I2C_ADDR_SCL		0x4B

enum ads111x_imux {
	IN0_IN1,
	IN0_IN3,
	IN1_IN3,
	IN2_IN3,
	IN0_GND,
	IN1_GND,
	IN2_GND,
	IN3_GND,
};

enum ads111x_gain {
	FSR_6_144V,
	FSR_4_096V,
	FSR_2_048V,
	FSR_1_024V,
	FSR_0_512V,
	FSR_0_256V,
};

enum ads111x_mode {
	CONTINUOUS,
	SINGLE_SHOT,
};

enum ads111x_rate {
	RATE_8_HZ,
	RATE_16_HZ,
	RATE_32_HZ,
	RATE_64_HZ,
	RATE_128_HZ,
	RATE_250_HZ,
	RATE_475_HZ,
	RATE_860_HZ,
};

enum ads111x_comp {
	TRADITIONAL,
	WINDOW,
};

enum ads111x_cpol {
	COMP_ACTIVE_LOW,
	COMP_ACTIVE_HIGH,
};

enum ads111x_comp_latch {
	COMP_LATCH_DISABLE,
	COMP_LATCH_ENABLE,
};

enum ads111x_comp_mode {
	COMP_ASSERT_AFTER_1_CONVERSION,
	COMP_ASSERT_AFTER_2_CONVERSION,
	COMP_ASSERT_AFTER_4_CONVERSION,
	COMP_DISABLED,
};

struct ads111x_t {
	/* Interface functions */
	uint8_t i2c_addr;
	uint8_t(*wr)(uint8_t i2c_addr, uint8_t reg, uint8_t *buf, uint16_t size);
	uint8_t(*rd)(uint8_t i2c_addr, uint8_t reg, uint8_t *buf, uint16_t size);

	enum ads111x_gain gain;
	enum ads111x_mode mode;
	enum ads111x_rate rate;
	enum ads111x_comp comp;
	enum ads111x_cpol cpol;
	enum ads111x_comp_latch comp_latch;
	enum ads111x_comp_mode comp_mode;
	uint16_t config;
};

uint8_t ads111x_init(struct ads111x_t *dev);
uint8_t ads111x_start(struct ads111x_t *dev,
	enum ads111x_imux channel);
uint8_t ads111x_read(struct ads111x_t *dev, int16_t *res);

#endif /* ADS1115_H */
