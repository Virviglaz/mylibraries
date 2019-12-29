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
 * INA3221 Triple-Channel, High-Side Measurement, Shunt and Bus Voltage
 * Monitor with I2C- and SMBUS-Compatible Interface
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef INA3221_H
#define INA3221_H

#include <stdint.h>
#include <stdbool.h>

#define INA3221_I2C_ADDRESS_GND			0x40
#define INA3221_I2C_ADDRESS_VDD			0x41
#define INA3221_I2C_ADDRESS_SDA			0x42
#define INA3221_I2C_ADDRESS_SCL			0x43

#define INA3221_NO_DATA_READY			0x80
#define INA3221_WRONG_ID				0x60

enum ina3221_channel {
	CH1,
	CH2,
	CH3
};

enum ina3221_average {
	AVG_0001,
	AVG_0004,
	AVG_0016,
	AVG_0064,
	AVG_0128,
	AVG_0256,
	AVG_0512,
	AVG_1024,
};

enum ina3221_conv_time {
	CONV_TIME_140US,
	CONV_TIME_204US,
	CONV_TIME_332US,
	CONV_TIME_588US,
	CONV_TIME_1100US,
	CONV_TIME_2116US,
	CONV_TIME_4156US,
	CONV_TIME_8244US,
};

enum ina3221_mode {
	POWER_DOWN,
	SHUNT_VOLTAGE_SINGLE_SHOT,
	BUS_VOLTAGE_SINGLE_SHOT,
	SHUNT_AND_BUS_SINGLE_SHOT,
	POWER_DOWN2,
	SHUNT_VOLTAGE_CONTIUOUS,
	BUS_VOLTAGE_CONTIUOUS,
	SHUNT_AND_BUS_CONTIUOUS,
};

struct ina3221_t {
	/* Interface */
	uint8_t i2c_addr;
	uint8_t(*wr)(uint8_t i2c_adrs, uint8_t reg, uint8_t *buf, uint16_t len);
	uint8_t(*rd)(uint8_t i2c_adrs, uint8_t reg, uint8_t *buf, uint16_t len);

	struct {
		enum ina3221_mode  mode : 3;
		enum ina3221_conv_time shunt_voltage_conv_time : 3;
		enum ina3221_conv_time bus_voltage_conv_time : 3;
		enum ina3221_average average : 3;
		bool ch3_enable : 1;
		bool ch2_enable : 1;
		bool ch1_enable : 1;
		bool reset : 1;
	} config;

	/* Shunt resistors values */
	double shunt[3];
};

/* Init */
uint8_t ina3221_init(struct ina3221_t *dev);

/* Read voltage */
double ina3221_read_bus_voltage(struct ina3221_t *dev,
	enum ina3221_channel ch);

/* Read current */
double ina3221_read_shunt_current(struct ina3221_t *dev,
	enum ina3221_channel ch);

/* Set critical current alarm */
uint8_t ina3221_set_critical_current(struct ina3221_t *dev,
	double current, enum ina3221_channel ch);

/* Set warning current alarm */
uint8_t ina3221_set_warning_current(struct ina3221_t *dev,
	double current, enum ina3221_channel ch);

/* Set power limit alarm */
uint8_t ina3221_set_power_limits(struct ina3221_t *dev,
	double high, double low);

#endif /* INA3221_H */
