/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2022 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
 * ESP32 LTC6804-2 battery monitor/ballancer driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "ltc6804.h"
#include <string.h>
#include <errno.h>

#define SWAP16(x)			(((x) >> 8) | ((x) << 8))
#define ADC_TO_REAL(x)			((double)(x) / 10000.0)
#define REAL_TO_ADC(x)			(uint16_t)((x) * 10000.0)
#define TEMP_TO_REAL(x)			(ADC_TO_REAL(x) / 0.0075 - 273.0)
#define TO_BOOL(x)			((x) ? true : false)

#define WRCFG				0x0001
#define RDCFG				0x0002
#define RDCVA				0x0004
#define RDCVB				0x0006
#define RDCVC				0x0008
#define RDCVD				0x000A
#define RDAUXA				0x000C
#define RDAUXB				0x000E
#define RDSTATA				0x0010
#define RDSTATB				0x0012
#define ADCV				0x0260
#define ADOW				0x0228
#define CVST				0x0207
#define ADAX				0x0460
#define AXST				0x0407
#define ADSTAT				0x0468
#define STATST				0x040F
#define ADCVAX				0x046F
#define CLRCELL				0x0711
#define CLRAUX				0x0712
#define CLRSTAT				0x0713
#define PLADC				0x0714
#define DIAGN				0x0715
#define WRCOMM				0x0721
#define RDCOMM				0x0722
#define STCOMM				0x0723

static io_func_t io;
static delay_ms_func delay;

static uint16_t pec15_calc(uint8_t *data, uint8_t len)
{
	const uint16_t crc15Table[256] = {
		0x0000, 0xC599, 0xCEAB, 0x0B32, 0xD8CF, 0x1D56, 0x1664, 0xD3FD,
		0xF407, 0x319E, 0x3AAC, 0xFF35, 0x2CC8, 0xE951, 0xE263, 0x27FA,
		0xAD97, 0x680E, 0x633C, 0xA6A5, 0x7558, 0xB0C1, 0xBBF3, 0x7E6A,
		0x5990, 0x9C09, 0x973B, 0x52A2, 0x815F, 0x44C6, 0x4FF4, 0x8A6D,
		0x5B2E, 0x9EB7, 0x9585, 0x501C, 0x83E1, 0x4678, 0x4D4A, 0x88D3,
		0xAF29, 0x6AB0, 0x6182, 0xA41B, 0x77E6, 0xB27F, 0xB94D, 0x7CD4,
		0xF6B9, 0x3320, 0x3812, 0xFD8B, 0x2E76, 0xEBEF, 0xE0DD, 0x2544,
		0x02BE, 0xC727, 0xCC15, 0x098C, 0xDA71, 0x1FE8, 0x14DA, 0xD143,
		0xF3C5, 0x365C, 0x3D6E, 0xF8F7, 0x2B0A, 0xEE93, 0xE5A1, 0x2038,
		0x07C2, 0xC25B, 0xC969, 0x0CF0, 0xDF0D, 0x1A94, 0x11A6, 0xD43F,
		0x5E52, 0x9BCB, 0x90F9, 0x5560, 0x869D, 0x4304, 0x4836, 0x8DAF,
		0xAA55, 0x6FCC, 0x64FE, 0xA167, 0x729A, 0xB703, 0xBC31, 0x79A8,
		0xA8EB, 0x6D72, 0x6640, 0xA3D9, 0x7024, 0xB5BD, 0xBE8F, 0x7B16,
		0x5CEC, 0x9975, 0x9247, 0x57DE, 0x8423, 0x41BA, 0x4A88, 0x8F11,
		0x057C, 0xC0E5, 0xCBD7, 0x0E4E, 0xDDB3, 0x182A, 0x1318, 0xD681,
		0xF17B, 0x34E2, 0x3FD0, 0xFA49, 0x29B4, 0xEC2D, 0xE71F, 0x2286,
		0xA213, 0x678A, 0x6CB8, 0xA921, 0x7ADC, 0xBF45, 0xB477, 0x71EE,
		0x5614, 0x938D, 0x98BF, 0x5D26, 0x8EDB, 0x4B42, 0x4070, 0x85E9,
		0x0F84, 0xCA1D, 0xC12F, 0x04B6, 0xD74B, 0x12D2, 0x19E0, 0xDC79,
		0xFB83, 0x3E1A, 0x3528, 0xF0B1, 0x234C, 0xE6D5, 0xEDE7, 0x287E,
		0xF93D, 0x3CA4, 0x3796, 0xF20F, 0x21F2, 0xE46B, 0xEF59, 0x2AC0,
		0x0D3A, 0xC8A3, 0xC391, 0x0608, 0xD5F5, 0x106C, 0x1B5E, 0xDEC7,
		0x54AA, 0x9133, 0x9A01, 0x5F98, 0x8C65, 0x49FC, 0x42CE, 0x8757,
		0xA0AD, 0x6534, 0x6E06, 0xAB9F, 0x7862, 0xBDFB, 0xB6C9, 0x7350,
		0x51D6, 0x944F, 0x9F7D, 0x5AE4, 0x8919, 0x4C80, 0x47B2, 0x822B,
		0xA5D1, 0x6048, 0x6B7A, 0xAEE3, 0x7D1E, 0xB887, 0xB3B5, 0x762C,
		0xFC41, 0x39D8, 0x32EA, 0xF773, 0x248E, 0xE117, 0xEA25, 0x2FBC,
		0x0846, 0xCDDF, 0xC6ED, 0x0374, 0xD089, 0x1510, 0x1E22, 0xDBBB,
		0x0AF8, 0xCF61, 0xC453, 0x01CA, 0xD237, 0x17AE, 0x1C9C, 0xD905,
		0xFEFF, 0x3B66, 0x3054, 0xF5CD, 0x2630, 0xE3A9, 0xE89B, 0x2D02,
		0xA76F, 0x62F6, 0x69C4, 0xAC5D, 0x7FA0, 0xBA39, 0xB10B, 0x7492,
		0x5368, 0x96F1, 0x9DC3, 0x585A, 0x8BA7, 0x4E3E, 0x450C, 0x8095,
	};

	uint16_t remainder = 16; // initialize the PEC
	uint16_t addr;

	/* loops for each byte in data array */
	for(uint8_t i = 0; i != len; i++) {
		/* calculate PEC table address */
		addr = ((remainder >> 7) ^ data[i]) & 0xff;
		remainder = (remainder << 8) ^ crc15Table[addr];
	}
	return (remainder * 2);
}

static void write_data(uint16_t cmd, uint8_t *data, uint8_t size)
{
	uint16_t pec;

	struct {
		uint8_t cmd[2];
		uint8_t cmd_pec[2];
		uint8_t data[size];
		uint8_t data_pec[2];
	} __attribute__((packed)) payload;

	payload.cmd[0] = cmd >> 8;
	payload.cmd[1] = cmd;
	pec = pec15_calc((uint8_t *)payload.cmd, sizeof(payload.cmd));
	payload.cmd_pec[0] = pec >> 8;
	payload.cmd_pec[1] = pec;

	if (size) {
		memcpy(payload.data, data, size);
		pec = pec15_calc(payload.data, size);
		payload.data_pec[0] = pec >> 8;
		payload.data_pec[1] = pec;
		size += 6;
		io((uint8_t *)&payload, NULL, size);
	} else { /* only send command, no payload */
		io((uint8_t *)&payload, NULL, 4);
	}
}

static int read_data(uint16_t cmd, uint8_t *data, uint8_t size, bool poll)
{
	uint16_t pec, rx_pec;
	uint8_t txrx_size = size + 6;

	struct {
		uint8_t cmd[2];
		uint8_t cmd_pec[2];
		uint8_t dummy[size + sizeof(uint16_t)];
	} __attribute__((packed)) tx_payload;

	struct {
		uint8_t dummy[4];
		uint8_t data[size];
		uint8_t data_pec[2];
	} __attribute__((packed)) rx_payload;

	memset(&tx_payload.dummy, 0xFF, size + sizeof(uint16_t));

	/* TX: CMD[0]..CMD[1]..PEC[0]..PEC[1] */
	tx_payload.cmd[0] = cmd >> 8;
	tx_payload.cmd[1] = cmd;
	pec = pec15_calc((uint8_t *)tx_payload.cmd, sizeof(tx_payload.cmd));
	tx_payload.cmd_pec[0] = pec >> 8;
	tx_payload.cmd_pec[1] = pec;

	io((uint8_t *)&tx_payload, (uint8_t *)&rx_payload, txrx_size);

	pec = rx_payload.data_pec[0] << 8 | rx_payload.data_pec[1];
	rx_pec = pec15_calc(rx_payload.data, size);
	if (!poll && rx_pec != pec)
		return EFAULT;

	memcpy(data, rx_payload.data, size);

	return 0;
}

int ltc6804_init(ltc6804_init_conf *cfg, io_func_t io_func, delay_ms_func wait)
{
	if (!io_func)
		return EINVAL;

	io = io_func;
	delay = wait;

	if (cfg) {
		uint16_t uv = REAL_TO_ADC(cfg->under_voltage);
		uint16_t ov = REAL_TO_ADC(cfg->over_voltage);
		uint8_t cfgr[6] = {
			[0] = 0xF8 | (cfg->refon ? 4 : 0) | \
				(cfg->fast_adc ? 1 : 0),
			[1] = uv & 0x00FF,
			[2] = ((uv >> 8) & 0xF) | ((ov & 0xF) << 4),
			[3] = ov >> 4,
			[4] = 0, /* discharge disable */
			[5] = (cfg->timeout & 0xF) << 4,
		};
		uint8_t cfgr_r[6];
		int res;

		write_data(WRCFG, cfgr, 6);

		/* diagnostic read */
		res = read_data(RDCFG, cfgr_r, sizeof(cfgr_r), false);
		if (res)
			return res;
		/* Compare read back data except GPIO and DCTO value */
		return memcmp(cfgr + 1, cfgr_r + 1,
			sizeof(cfgr_r) - 2) ? ENOLINK : 0;
	}

	return 0;
}

void ltc6804_start_cell_adc_conv(enum adc_mode mode, bool discharge_en)
{
	/* start conversion for all cells */
	uint16_t adcv = ADCV | ((uint16_t)mode << 7) | \
		((discharge_en ? 1 : 0) << 4);
	write_data(adcv, NULL, 0);
}

void ltc6804_start_gpio_adc_conv(enum adc_mode mode)
{
	/* start conversion for all gpios */
	uint16_t adax = ADAX | (uint16_t)mode << 7;
	write_data(adax, NULL, 0);
}

void ltc6804_start_combined_adc_conv(enum adc_mode mode, bool discharge_en)
{
	uint16_t adcax = ADCVAX | (uint16_t)mode << 7 | \
		(discharge_en ? 1 : 0) << 4;
	write_data(adcax, NULL, 0);
}

void ltc6804_start_int_params_adc_conv(enum adc_mode mode)
{
	uint16_t adstat = ADSTAT | (uint16_t)mode << 7;
	write_data(adstat, NULL, 0);
}

bool ltc6804_check_conversion_done(void)
{
	uint8_t response = 0;
	read_data(PLADC, &response, sizeof(response), true);
	return response ? true : false;
}

bool ltc6804_wait_conversion_done(uint32_t poll_interval_ms)
{
	uint8_t atmp = 10;
	do {
		if (ltc6804_check_conversion_done())
			return true;
		delay(poll_interval_ms);
	} while (atmp--);
	return false;
}

void ltc6804_clear_cell_voltages(void)
{
	write_data(CLRCELL, NULL, 0);
}

void ltc6804_clear_gpio_voltages(void)
{
	write_data(CLRAUX, NULL, 0);
}

void ltc6804_clear_status_register(void)
{
	write_data(CLRSTAT, NULL, 0);
}

void ltc6804_clear_all(void)
{
	ltc6804_clear_cell_voltages();
	ltc6804_clear_gpio_voltages();
	ltc6804_clear_status_register();
}

int ltc6804_read_cells(cell_meas *c)
{
	if (!c)
		return EINVAL;

	/* CVAR0..5 CVBR0..5 CVCR0..5 CVDR0..5 Cell Voltage Register Groups */
	struct {
		uint16_t voltage[3];
	}  __attribute__((packed)) cell_group[4];

	/* read CVAR0..5 CVBR0..5 CVCR0..5 CVDR0..5 */
	const uint16_t rdva_x[4] = { RDCVA, RDCVB, RDCVC, RDCVD };
	for (int i = 0; i != 4; i++) {
		int res = read_data(rdva_x[i],
			(uint8_t *)&cell_group[i], 6, false);
			if (res)
				return res;
	}

	/* convert cell voltages */
	int cell = 0;
	for (int i = 0; i != 4; i++)
		for (int j = 0; j != 3; j++)
			c->cell[cell++] = ADC_TO_REAL(cell_group[i].voltage[j]);
	return 0;
}

int ltc6804_read_comb(comb_meas *c)
{
	if (!c)
		return EINVAL;

	/* CVAR0..5 CVBR0..5 CVCR0..5 CVDR0..5 Cell Voltage Register Groups */
	struct {
		uint16_t voltage[3];
	}  __attribute__((packed)) cell_group[4];

	/* AVAR0..5 Auxiliary Register Group A */
	struct {
		uint16_t gpio[3];
	}  __attribute__((packed)) avar;

	/* read CVAR0..5 CVBR0..5 CVCR0..5 CVDR0..5 */
	const uint16_t rdva_x[4] = { RDCVA, RDCVB, RDCVC, RDCVD };
	for (int i = 0; i != 4; i++) {
		int res = read_data(rdva_x[i],
			(uint8_t *)&cell_group[i], 6, false);
			if (res)
				return res;
	}

	/* read AVAR0..5 */
	int res = read_data(RDAUXA, (uint8_t *)&avar, 6, false);
	if (res)
		return res;

	/* convert cell voltages */
	int cell = 0;
	for (int i = 0; i != 4; i++)
		for (int j = 0; j != 3; j++)
			c->cell[cell++] = ADC_TO_REAL(cell_group[i].voltage[j]);

	/* convert GPIO0..2 voltages */
	for (int i = 0; i != 2; i++)
		c->gpio[i] = ADC_TO_REAL(avar.gpio[i]);

	return 0;	
}

int ltc6804_read_aux(aux_meas *c)
{
	int res;

	if (!c)
		return EINVAL;

	/* AVAR0..5 Auxiliary Register Group A */
	struct {
		uint16_t gpio[3];
	}  __attribute__((packed)) avar;

	/* AVBR0..5 Auxiliary Register Group B */
	struct {
		uint16_t gpio[2];
		uint16_t ref;
	}  __attribute__((packed)) avbr;

	/* read AVAR0..5 */
	res = read_data(RDAUXA, (uint8_t *)&avar, 6, false);
	if (res)
		return res;

	/* read AVBR0..5 */
	res = read_data(RDAUXB, (uint8_t *)&avbr, 6, false);
	if (res)
		return res;

	/* convert GPIO0..3 voltages */
	for (int i = 0; i != 3; i++)
		c->gpio[i] = ADC_TO_REAL(avar.gpio[i]);

	c->gpio[3] = ADC_TO_REAL(avbr.gpio[0]);
	c->gpio[4] = ADC_TO_REAL(avbr.gpio[1]);
	c->ref_voltage = ADC_TO_REAL(avbr.ref);

	return 0;
}

int ltc6804_read_int_params(misc_meas *c)
{
	int res;

	/* STAR0..5: Status Register Group A */
	struct {
		uint16_t soc;
		uint16_t itmp;
		uint16_t va;
	}  __attribute__((packed)) star;

	/* STBR0..5: Status Register Group B */
	struct {
		uint16_t vd;
		uint8_t flags[3];
		uint8_t revision;
	}  __attribute__((packed)) stbr;

	/* read STAR0..5 */
	res = read_data(RDSTATA, (uint8_t *)&star, 6, false);
	if (res)
		return res;

	/* read STBR0..5 */
	res = read_data(RDSTATB, (uint8_t *)&stbr, 6, false);
	if (res)
		return res;

	/* convert Status Register Group A */
	c->sum_meas = ADC_TO_REAL(star.soc) * 20.0;
	c->die_temp = TEMP_TO_REAL(star.itmp);
	c->analog_supp_v = ADC_TO_REAL(star.va);

	/* convert Status Register Group B */
	c->digital_supp_v = ADC_TO_REAL(stbr.vd);
	c->chip_revision = stbr.revision >> 4;

	c->cell_undervoltage[0]	= TO_BOOL(stbr.flags[0] & (1 << 0));
	c->cell_overvoltage[0]	= TO_BOOL(stbr.flags[0] & (1 << 1));

	c->cell_undervoltage[1]	= TO_BOOL(stbr.flags[0] & (1 << 2));
	c->cell_overvoltage[1]	= TO_BOOL(stbr.flags[0] & (1 << 3));

	c->cell_undervoltage[2]	= TO_BOOL(stbr.flags[0] & (1 << 4));
	c->cell_overvoltage[2]	= TO_BOOL(stbr.flags[0] & (1 << 5));

	c->cell_undervoltage[3]	= TO_BOOL(stbr.flags[0] & (1 << 6));
	c->cell_overvoltage[3]	= TO_BOOL(stbr.flags[0] & (1 << 7));

	c->cell_undervoltage[4]	= TO_BOOL(stbr.flags[1] & (1 << 0));
	c->cell_overvoltage[4]	= TO_BOOL(stbr.flags[1] & (1 << 1));

	c->cell_undervoltage[5]	= TO_BOOL(stbr.flags[1] & (1 << 2));
	c->cell_overvoltage[5]	= TO_BOOL(stbr.flags[1] & (1 << 3));

	c->cell_undervoltage[6]	= TO_BOOL(stbr.flags[1] & (1 << 4));
	c->cell_overvoltage[6]	= TO_BOOL(stbr.flags[1] & (1 << 5));

	c->cell_undervoltage[7]	= TO_BOOL(stbr.flags[1] & (1 << 6));
	c->cell_overvoltage[7]	= TO_BOOL(stbr.flags[1] & (1 << 7));

	c->cell_undervoltage[8]	= TO_BOOL(stbr.flags[2] & (1 << 0));
	c->cell_overvoltage[8]	= TO_BOOL(stbr.flags[2] & (1 << 1));

	c->cell_undervoltage[9]	= TO_BOOL(stbr.flags[2] & (1 << 2));
	c->cell_overvoltage[9]	= TO_BOOL(stbr.flags[2] & (1 << 3));

	c->cell_undervoltage[10] = TO_BOOL(stbr.flags[2] & (1 << 4));
	c->cell_overvoltage[10]  = TO_BOOL(stbr.flags[2] & (1 << 5));

	c->cell_undervoltage[11] = TO_BOOL(stbr.flags[2] & (1 << 6));
	c->cell_overvoltage[11]  = TO_BOOL(stbr.flags[2] & (1 << 7));

	return 0;
}

int ltc6804_convert_cell(cell_meas *c, enum adc_mode mode, bool discharge_en,
	uint32_t poll_interval_ms)
{
	ltc6804_start_cell_adc_conv(mode, discharge_en);
	int res = ltc6804_wait_conversion_done(poll_interval_ms) ? 0 : EBUSY;
	if (res)
		return res;
	return ltc6804_read_cells(c);
}

int ltc6804_convert_comb(comb_meas *c, enum adc_mode mode, bool discharge_en,
	uint32_t poll_interval_ms)
{
	ltc6804_start_combined_adc_conv(mode, discharge_en);
	int res = ltc6804_wait_conversion_done(poll_interval_ms) ? 0 : EBUSY;
	if (res)
		return res;
	return ltc6804_read_comb(c);
}

int ltc6804_convert_aux(aux_meas *c, enum adc_mode mode,
	uint32_t poll_interval_ms)
{
	ltc6804_start_gpio_adc_conv(mode);
	int res = ltc6804_wait_conversion_done(poll_interval_ms) ? 0 : EBUSY;
	if (res)
		return res;
	return ltc6804_read_aux(c);
}

int ltc6804_convert_misc(misc_meas *c, enum adc_mode mode,
	uint32_t poll_interval_ms)
{
	ltc6804_start_int_params_adc_conv(mode);
	int res = ltc6804_wait_conversion_done(poll_interval_ms) ? 0 : EBUSY;
	if (res)
		return res;
	return ltc6804_read_int_params(c);
}

int ltc6804_discharge(uint8_t cell, bool state)
{
	uint8_t cfgr[6];

	int res = read_data(RDCFG, cfgr, sizeof(cfgr), false);
	if (res)
		return res;

	switch (cell)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		if (state)
			cfgr[4] |= 1 << cell;
		else
			cfgr[4] &= ~(1 << cell);
		break;
	case 8:
	case 9:
	case 10:
	case 11:
		cell -= 8;
		if (state)
			cfgr[5] |= 1 << cell;
		else
			cfgr[5] &= ~(1 << cell);
		break;
	default:
		break;
	}

	write_data(WRCFG, cfgr, sizeof(cfgr));
	return 0;
}
