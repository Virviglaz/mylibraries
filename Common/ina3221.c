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
 * INA3221 Triple-ch, High-Side Measurement, Shunt and Bus Voltage
 * Monitor with I2C- and SMBUS-Compatible Interface
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "ina3221.h"

#ifndef be16toword
#define be16toword(a)	((((a) >> 8) & 0xff) | (((a) << 8) & 0xff00))
#endif

#define INA3221_CH1							0x00
#define INA3221_CH2							0x01
#define INA3221_CH3							0x02

#define INA3221_CONFIG_REG					0x00
#define INA3221_CH1_SHUNT_I					0x01
#define INA3221_CH1_BUS_V					0x02
#define INA3221_CH2_SHUNT_I					0x03
#define INA3221_CH2_BUS_V					0x04
#define INA3221_CH3_SHUNT_I					0x05
#define INA3221_CH3_BUS_V					0x06
#define INA3221_CH1_CRIT_I					0x07
#define INA3221_CH1_WARN_I					0x08
#define INA3221_CH2_CRIT_I					0x09
#define INA3221_CH2_WARN_I					0x0A
#define INA3221_CH3_CRIT_I					0x0B
#define INA3221_CH3_WARN_I					0x0C
#define INA3221_SHUNT_I_SUM					0x0D
#define INA3221_SHUNT_I_SUM_LIM				0x0E
#define INA3221_MASK_ENABLE					0x0F
#define INA3221_POWER_UPPER_LIM				0x10
#define INA3221_POWER_LOWER_LIM				0x11
#define INA3221_MANUFACTURE_ID				0xFE
#define INA3221_DIE_ID						0xFF

#define INA3221_MANUFACTURE_ID_VALUE		0x5449

static int16_t read_reg(struct ina3221_t *dev, uint8_t reg)
{
	int16_t ret;

	dev->rd(dev->i2c_addr, reg, (uint8_t*)&ret, sizeof(ret));

	return be16toword(ret);
}

static uint8_t write_reg(struct ina3221_t *dev,
	uint8_t reg, uint16_t value)
{
	value = be16toword(value);
	return dev->wr(dev->i2c_addr, reg, (uint8_t*)&value, sizeof(value));
}

static uint8_t check_id(struct ina3221_t *dev)
{
	uint8_t ret;
	uint16_t id;

	ret = dev->rd(dev->i2c_addr, INA3221_MANUFACTURE_ID,
		(uint8_t*)&id, sizeof(id));

	id = be16toword(id);

	return ret ? ret : \
		id == INA3221_MANUFACTURE_ID_VALUE ? 0 : INA3221_WRONG_ID;
}

uint8_t ina3221_init(struct ina3221_t *dev)
{
	uint8_t ret = check_id(dev);
	if (ret)
		return ret;

	return write_reg(dev, INA3221_CONFIG_REG,
		*(uint16_t *)&dev->config);
}

double ina3221_read_bus_voltage(struct ina3221_t *dev,
	enum ina3221_channel ch)
{
	const uint8_t ch_regs[] = {
		INA3221_CH1_BUS_V,
		INA3221_CH2_BUS_V,
		INA3221_CH3_BUS_V,
	};
	int16_t raw_value;

	raw_value = read_reg(dev, ch_regs[(uint8_t)ch]);

	return (double)raw_value / 1000; /* mV -> V */
}

double ina3221_read_shunt_current(struct ina3221_t *dev,
	enum ina3221_channel ch)
{
	const uint8_t ch_regs[] = {
		INA3221_CH1_SHUNT_I,
		INA3221_CH2_SHUNT_I,
		INA3221_CH3_SHUNT_I,
	};
	int16_t raw_value;

	raw_value = read_reg(dev, ch_regs[(uint8_t)ch]);

	return (double)raw_value / 8 * 40 / 1000000 / dev->shunt[ch];
}

uint8_t ina3221_set_critical_current(struct ina3221_t *dev,
	double current, enum ina3221_channel ch)
{
	const uint8_t ch_regs[] = {
		INA3221_CH1_CRIT_I,
		INA3221_CH2_CRIT_I,
		INA3221_CH3_CRIT_I,
	};
	int16_t raw_value;

	raw_value = (double)(current / 40 / 8 * 1000000 * \
		dev->shunt[(uint8_t)ch]);

	return write_reg(dev, ch_regs[(uint8_t)ch], raw_value);
}

uint8_t ina3221_set_warning_current(struct ina3221_t *dev,
	double current, enum ina3221_channel ch)
{
	const uint8_t ch_regs[] = {
		INA3221_CH1_WARN_I,
		INA3221_CH2_WARN_I,
		INA3221_CH3_WARN_I
	};
	int16_t raw_value;

	raw_value = (double)current / 40 / 8 * 1000000 * \
		dev->shunt[(uint8_t)ch];

	return write_reg(dev, ch_regs[(uint8_t)ch], raw_value);
}

uint8_t ina3221_set_power_limits(struct ina3221_t *dev,
	double high, double low)
{
	write_reg(dev, INA3221_POWER_UPPER_LIM, (int16_t)(high * 1000));
	
	return write_reg(dev, INA3221_POWER_LOWER_LIM,
		(int16_t)(low * 1000));
}
