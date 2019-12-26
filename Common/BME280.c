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

#include "BME280.h"

/* Registers deginition */
#define BME280_HUM_LSB				0XFE
#define BME280_HUM_MSB				0XFD
#define BME280_TEMP_XLSB			0XFC
#define BME280_TEMP_LSB				0XFB
#define BME280_TEMP_MSB				0XFA
#define BME280_PRESS_XLSB			0XF9
#define BME280_PRESS_LSB 			0XF8
#define BME280_PRESS_MSB 			0XF7
#define BME280_CONFIG				0XF5
#define BME280_CTRL_MEAS			0XF4
#define BME280_STATUS				0XF3
#define BME280_CTRL_HUM				0XF2
#define BME280_RESET				0XE0
#define BME280_ID					0XD0

static enum chip_type bme280_check_id(struct bme280_t *dev)
{
	uint8_t id;

	if (dev->read_reg(BME280_ID, &id, sizeof(id)))
		return ID_WRONG;

	switch (id)
	{
	case 0x58: return BMP280;
	case 0x60: return BME280;
	default: return ID_WRONG;
	}
}

static uint8_t check_busy(struct bme280_t *dev)
{
	uint8_t ret;

	dev->read_reg(BME280_STATUS, &ret, sizeof(ret));

	return ret & (1 << 3);
}

static int32_t t_fine;
static int32_t compensate_t_int32(int32_t adc_t, struct bme280_t *dev)
{
	int32_t var1, var2;

	var1 = ((((adc_t >> 3) - ((int32_t)dev->calibration.t1 << 1))) *
			((int32_t)dev->calibration.t2)) >> 11;

	var2 = (((((adc_t >> 4) - ((int32_t)dev->calibration.t1)) *
			((adc_t >> 4) - ((int32_t)dev->calibration.t1))) >> 12) *
			((int32_t)dev->calibration.t3)) >> 14;
	
	t_fine = var1 + var2;

	return (t_fine * 5 + 128) >> 8;
}

static uint32_t compensate_p_int64(int32_t adc_p, struct bme280_t *dev)
{
	int64_t var1, var2, p;

	var1 = ((int64_t)t_fine) - 128000;

	var2 = var1 * var1 * (int64_t)dev->calibration.p6;

	var2 = var2 + ((var1*(int64_t)dev->calibration.p5) << 17);

	var2 = var2 + (((int64_t)dev->calibration.p4) << 35);

	var1 = ((var1 * var1 * (int64_t)dev->calibration.p3) >> 8) +
			((var1 * (int64_t)dev->calibration.p2) << 12);

	var1 = (((((int64_t)1) << 47) + var1)) *
			((int64_t)dev->calibration.p1) >> 33;

	if (!var1)
		return 0; /* avoid exception caused by division by zero */

	p = 1048576 - adc_p;

	p = (((p << 31) - var2) * 3125) / var1;

	var1 = (((int64_t)dev->calibration.p9) * (p >> 13) * (p >> 13)) >> 25;

	var2 = (((int64_t)dev->calibration.p8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calibration.p7) << 4);

	return (uint32_t)p;
}

static uint32_t compensate_h_int32(int32_t adc_h, struct bme280_t *dev)
{
	int32_t hum;
	
	hum = (t_fine - ((int32_t)76800));

	hum = (((((adc_h << 14) - (((int32_t)dev->calibration.h4) << 20) -
			(((int32_t)dev->calibration.h5) * hum)) +
			((int32_t)16384)) >> 15) * (((((((hum *
			((int32_t)dev->calibration.h6)) >> 10) *
			(((hum * ((int32_t)dev->calibration.h3)) >> 11) +
			((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
			((int32_t)dev->calibration.h2) + 8192) >> 14));

	hum = (hum - (((((hum >> 15) * (hum >> 15)) >> 7) *
			((int32_t)dev->calibration.h1)) >> 4));

	hum = (hum < 0 ? 0 : hum);
	hum = (hum > 419430400 ? 419430400 : hum);

	return (uint32_t)(hum >> 12);
}

uint8_t bme280_init(struct bme280_t *dev)
{
	uint8_t ret;
	uint8_t buf[4];

	dev->chip = bme280_check_id(dev);
	if (dev->chip == ID_WRONG)
		return (uint8_t)ID_WRONG;

	dev->sea_level_pressure = 101325; /* Pressure at sea level (Pa) */

	/* T1..T3, P1..P9 -> 0x88..0x9F */
	ret = dev->read_reg(0x88, (uint8_t*)&dev->calibration, 24);
	if (ret)
		return ret;

	/* H1 -> 0xA1 */
	dev->read_reg(0xA1, (uint8_t*)&dev->calibration.h1, 1);

	/* H2 -> 0xE1..0xE2 */
	dev->read_reg(0xE1, (uint8_t*)&dev->calibration.h2, 2);

	/* H3 -> 0xE3 */
	dev->read_reg(0xE3, (uint8_t*)&dev->calibration.h3, 1);

	/* H6 -> 0xE7 */
	dev->read_reg(0xE7, (uint8_t*)&dev->calibration.h6, 1);

	/* H6 -> 0xE7 */
	dev->read_reg(0xE4, buf, 1);

	dev->calibration.h4 = (buf[1] & 0x07) | (buf[0] << 4);
	dev->calibration.h5 = (buf[2] & 0x07) | (buf[3] << 4);

	dev->write_reg(BME280_CTRL_MEAS, dev->temperature_oversampling << 5 |
		dev->pressure_oversampling << 2 | 3);

	dev->write_reg(BME280_CTRL_HUM, dev->humidity_oversampling);

	return ret;
}

uint8_t bme280_get_result(struct bme280_t *dev)
{
	uint8_t ret;
	uint8_t buf[8];

	ret = check_busy(dev);
	if (ret)
		return BME280_BUSY;

	dev->read_reg(BME280_PRESS_MSB, buf, sizeof(buf));

	dev->temperature = (double)compensate_t_int32((int32_t)(buf[5] |
			buf[4] << 8 | buf[3] << 16) >> 4, dev) / 100;

	dev->pressure = (uint32_t)(compensate_p_int64((int32_t)(buf[2] |
			buf[1] << 8 | buf[0] << 16) >> 4, dev) >> 8);

	if (dev->chip == BME280)
		dev->humidity = (double)(compensate_h_int32((int32_t)(buf[7] |
			buf[6] << 8), dev)) / 1024;

	return ret;
}

uint8_t bme280_calibrate_sea_level(struct bme280_t *dev)
{
	uint8_t ret;

	ret = bme280_get_result(dev);
	if (ret)
		return ret;

	dev->sea_level_pressure = dev->pressure;

	return ret;
}

#include <math.h>

double bme280_altitude(struct bme280_t *dev)
{
	return (double)44330 * (1 - pow(((double)dev->pressure /
			dev->sea_level_pressure), 0.190295));
}

uint16_t bme280_mmHg(struct bme280_t *dev)
{
	return (uint16_t)((uint32_t)dev->pressure * 760) /
			dev->sea_level_pressure;
}
