/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2019 Pavel Nadein
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * BME280 Combined humidity and pressure sensor
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef BME280_H
#define BME280_H

#include <stdint.h>

#define BME280_DEFAULT_I2C_ADDRESS		0x76
#define BME280_BUSY						0x10

enum oversampling {
	BME280_OVS_SKIPPED,
	BME280_OVS_X1,
	BME280_OVS_X2,
	BME280_OVS_X4,
	BME280_OVS_X8,
	BME280_OVS_X16,
};

enum chip_type {
	ID_WRONG = 1,
	BMP280,
	BME280,
};

struct bme280_const {
	uint16_t t1;
	int16_t t2;
	int16_t t3;
	uint16_t p1;
	int16_t p2;
	int16_t p3;
	int16_t p4;
	int16_t p5;
	int16_t p6;
	int16_t p7;
	int16_t p8;
	int16_t p9;
	uint8_t h1;
	int16_t h2;
	uint8_t h3;
	int16_t h4;
	int16_t h5;
	int8_t h6;
};

struct bme280_t {
	/* Result */
	double humidity;
	uint32_t pressure, sea_level_pressure;
	double temperature;

	/* Interface functions */
	uint8_t(*write_reg)(uint8_t reg, uint8_t value);
	uint8_t(*read_reg) (uint8_t reg, uint8_t *buf, uint16_t size);

	/* Settings */
	enum oversampling humidity_oversampling;
	enum oversampling pressure_oversampling;
	enum oversampling temperature_oversampling;

	struct bme280_const calibration;
	enum chip_type chip;
};

uint8_t bme280_init(struct bme280_t *dev);
uint8_t bme280_get_result(struct bme280_t *dev);
uint8_t bme280_calibrate_sea_level(struct bme280_t *dev);
double bme280_altitude(struct bme280_t *dev);
uint16_t bme280_mmHg(struct bme280_t *dev);

#endif /* BME280_H */