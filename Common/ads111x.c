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

#include "ads111x.h"

#define CONV_REG	0
#define CONF_REG	1
#define LO_TRES		2
#define HI_TRES		3

static uint8_t check_ready(struct ads111x_t *dev)
{
	uint8_t conf[2];

	dev->rd(dev->i2c_addr, CONF_REG,
		conf, sizeof(conf));

	return conf[1] & 0x80;
}

uint8_t ads111x_init(struct ads111x_t *dev)
{
	dev->config = dev->gain << 9
		| dev->mode << 8
		| dev->rate << 5
		| dev->comp << 4
		| dev->cpol << 3
		| dev->comp_latch << 2
		| dev->comp_mode;

	return dev->wr(dev->i2c_addr, CONF_REG,
			(uint8_t*)&dev->config, sizeof(dev->config));
}

uint8_t ads111x_start(struct ads111x_t *dev,
	enum ads111x_imux channel)
{
	dev->config &= ~0x7000;
	dev->config |= channel << 12 | 1 << 15;

	return dev->wr(dev->i2c_addr, CONF_REG,
		(uint8_t*)&dev->config, sizeof(dev->config));
}

uint8_t ads111x_read(struct ads111x_t *dev, int16_t *res)
{
	if (!check_ready(dev))
		return ADS111X_CONV_IN_PROGRESS;

	return dev->rd(dev->i2c_addr, CONV_REG,
		(uint8_t*)res, sizeof(*res));
}
