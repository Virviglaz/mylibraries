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
 * APDS-9960 Digital Proximity, Ambient Light, RGB and Gesture Sensor
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "apds9960.h"

#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif
#define APDS_ENABLE_REG		0x80 /* R/W Enable states and interrupts */
#define APDS_ENABLE_GEN		BIT(6) /* Gesture Enable */
#define APDS_ENABLE_PIEN	BIT(5) /* Proximity Interrupt Enable */
#define APDS_ENABLE_AIEN	BIT(4) /* ALS Interrupt Enable */
#define APDS_ENABLE_WEN		BIT(3) /* Wait Enable */
#define APDS_ENABLE_PEN		BIT(2) /* Proximity Detect Enable */
#define APDS_ENABLE_AEN		BIT(1) /* ALS Enable */
#define APDS_ENABLE_PON		BIT(0) /* Power ON */

#define APDS_ATIME_REG		0x81 /* R/W ADC integration time */
#define APDS_WTIME_REG		0x83 /* R/W Wait time (non-gesture) */
#define APDS_AILTL_REG		0x84 /* R/W ALS interrupt low threshold low byte */
#define APDS_AILTH_REG		0x85 /* R/W ALS interrupt low threshold high byte */
#define APDS_AIHTL_REG		0x86 /* R/W ALS interrupt high threshold low byte */
#define APDS_AIHTH_REG		0x87 /* R/W ALS interrupt high threshold high byte */
#define APDS_PILT_REG		0x89 /* R/W Proximity interrupt low threshold */
#define APDS_PIHT_REG		0x8B /* R/W Proximity interrupt high threshold */
#define APDS_PERS_REG		0x8C /* R/W Interrupt persistence filters */
#define APDS_CONFIG1_REG	0x8D /* R/W Configuration register one */
#define APDS_PPULSE_REG		0x8E /* R/W Proximity pulse count and length */
#define APDS_CONTROL_REG	0x8F /* R/W Gain control */
#define APDS_CONFIG2_REG	0x90 /* R/W Configuration register two */
#define APDS_ID_REG		0x92 /* R Device ID */

#define APDS_STATUS_REG		0x93 /* R Device status */
#define APDS_STATUS_CPSAT_BIT	BIT(7) /* Clear Photodiode Saturation */
#define APDS_STATUS_PGSAT_BIT	BIT(6) /* Analog saturation event occurred */
#define APDS_STATUS_PINT_BIT	BIT(5) /* Proximity Interrupt */
#define APDS_STATUS_AINT_BIT	BIT(4) /* ALS Interrupt */
#define APDS_STATUS_GINT_BIT	BIT(2) /* Gesture Interrupt */
#define APDS_STATUS_PVALID_BIT	BIT(1) /* Proximity Valid */
#define APDS_STATUS_AVALID_BIT	BIT(0) /* ALS Valid */

#define APDS_CDATAL_REG		0x94 /* R Low byte of clear channel data */
#define APDS_CDATAH_REG		0x95 /* R High byte of clear channel data */
#define APDS_RDATAL_REG		0x96 /* R Low byte of red channel data */
#define APDS_RDATAH_REG		0x97 /* R High byte of red channel data */
#define APDS_GDATAL_REG		0x98 /* R Low byte of green channel data */
#define APDS_GDATAH_REG		0x99 /* R High byte of green channel data */
#define APDS_BDATAL_REG		0x9A /* R Low byte of blue channel data */
#define APDS_BDATAH_REG		0x9B /* R High byte of blue channel data */
#define APDS_PDATA_REG		0x9C /* R Proximity data */
#define APDS_POFFSET_UR_REG	0x9D /* R/W Proximity offset for U/R photodiodes */
#define APDS_POFFSET_DL_REG	0x9E /* R/W Proximity offset for D/L photodiodes */
#define APDS_CONFIG3_REG	0x9F /* R/W Configuration register three */
#define APDS_GPENTH_REG		0xA0 /* R/W Gesture proximity enter threshold */
#define APDS_GEXTH_REG		0xA1 /* R/W Gesture exit threshold */
#define APDS_GCONF1_REG		0xA2 /* R/W Gesture configuration one */
#define APDS_GCONF2_REG		0xA3 /* R/W Gesture configuration two */
#define APDS_GOFFSET_U_REG	0xA4 /* R/W Gesture UP offset register */
#define APDS_GOFFSET_D_REG	0xA5 /* R/W Gesture DOWN offset register */
#define APDS_GOFFSET_L_REG	0xA7 /* R/W Gesture LEFT offset register */
#define APDS_GOFFSET_R_REG	0xA9 /* R/W Gesture RIGHT offset register */
#define APDS_GPULSE_REG		0xA6 /* R/W Gesture pulse count and length */
#define APDS_GCONF3_REG		0xAA /* R/W Gesture configuration three */

#define APDS_GCONF4_REG		0xAB /* R/W Gesture configuration four */
#define APDS_CONFIG3_GFIFO_CLR	BIT(2) /* Setting this bit to '1' clears irq */
#define APDS_CONFIG3_GIEN	BIT(1) /* Gesture interrupt enable */
#define APDS_CONFIG3_GMODE	BIT(0) /* Gesture Mode */

#define APDS_GFLVL_REG		0xAE /* R Gesture FIFO level */

#define APDS_GSTATUS_REG	0xAF /* R Gesture status */
#define APDS_GSTATUS_GFOV	BIT(1) /* Gesture FIFO Overflow */
#define APDS_GSTATUS_GVALID	BIT(0) /* Gesture FIFO Data */

#define APDS_IFORCE_REG		0xE4 /* W Force interrupt */
#define APDS_PICLEAR_REG	0xE5 /* W Proximity interrupt clear */
#define APDS_CICLEAR_REG	0xE6 /* W ALS clear channel interrupt clear */
#define APDS_AICLEAR_REG	0xE7 /* W All non-gesture interrupts clear */
#define APDS_GFIFO_U_REG	0xFC /* R Gesture FIFO UP value */
#define APDS_GFIFO_D_REG	0xFD /* R Gesture FIFO DOWN value */
#define APDS_GFIFO_L_REG	0xFE /* R Gesture FIFO LEFT value */
#define APDS_GFIFO_R_REG	0xFF /* R Gesture FIFO RIGHT value */

/* ID Register (0x92) value, page 25 */
#define APDS_ID			0xAB
#define GESTURE_THRESHOLD_MIN   10
#define GESTURE_THRESHOLD_MAX   240

static uint8_t check_irq(struct apds9960 *dev, uint8_t bit)
{
	uint8_t irq;

	if (dev->check_irq && !dev->check_irq())
		return 0;

	dev->read_reg(APDS_STATUS_REG, &irq, sizeof(irq));
	return irq & bit;
}

static uint8_t wait_for_irq(struct apds9960 *dev, uint8_t bit)
{
	uint8_t ret;
	if (dev->wait_for_irq)
		ret = dev->wait_for_irq(dev, bit);
	else
		while (!(ret = check_irq(dev, bit)));
	return ret;
}

static uint8_t sleep_mode(struct apds9960 *dev)
{
	uint8_t ret;

	ret = dev->write_reg(APDS_ENABLE_REG, 0);

	/* Clear interrupt */
	dev->write_reg(APDS_PICLEAR_REG, 0);
	dev->write_reg(APDS_CICLEAR_REG, 0);
	dev->write_reg(APDS_AICLEAR_REG, 0);

	return ret;
}

static bool validate_gest_data(struct gesture_data *data)
{
	return (data->up > GESTURE_THRESHOLD_MIN &&
		data->down > GESTURE_THRESHOLD_MIN &&
		data->left > GESTURE_THRESHOLD_MIN &&
		data->right > GESTURE_THRESHOLD_MIN &&
		data->up < GESTURE_THRESHOLD_MAX &&
		data->down < GESTURE_THRESHOLD_MAX &&
		data->left < GESTURE_THRESHOLD_MAX &&
		data->right < GESTURE_THRESHOLD_MAX);
}

static enum apds_gesture proccess_gesture(struct gesture_data *min,
					  struct gesture_data *max)
{
	int ud_min = ((min->up - min->down) * 100) / (min->up + min->down);
	int lr_min = ((min->left - min->right) * 100) / (min->left + min->right);
	int ud_max = ((max->up - max->down) * 100) / (max->up + max->down);
	int lr_max = ((max->left - max->right) * 100) / (max->left + max->right);
	int diff_ud = ud_max - ud_min;
	int diff_lr = lr_max - lr_min;
	
	if (diff_ud > diff_lr && diff_ud > GESTURE_THRESHOLD_MIN)
		return DIR_UP;

	if (diff_ud > diff_lr && diff_ud < -GESTURE_THRESHOLD_MIN)
		return DIR_DOWN;

	if (diff_ud < diff_lr && diff_lr > GESTURE_THRESHOLD_MIN)
		return DIR_LEFT;

	if (diff_ud < diff_lr && diff_lr < -GESTURE_THRESHOLD_MIN)
		return DIR_RIGHT;

	return ERR_DATA_INVALID;
}

/*
 * Init chip with default settings
 * This settings can be overwritten after
 */
void apds9960_use_default(struct apds9960 *dev)
{
	dev->atime = 0xFF;
	dev->wtime = 0xFF;
	dev->als_treshold.low_threshold = 0;
	dev->als_treshold.high_threshold = 0;
	dev->prox_treshold.low_threshold = 0;
	dev->prox_treshold.high_threshold = 0;
	dev->persistance.apers = 0;
	dev->persistance.ppers = 0;
	dev->conf1.wlong = true;
	dev->prox_pulse_cnt.ppulse = 0;
	dev->prox_pulse_cnt.pplen = PLEN_8_US;
	dev->ctrl1.again = GAIN_1x;
	dev->ctrl1.pgain = GAIN_1x;
	dev->ctrl1.led_c = LED_100mA;
	dev->conf2.led_boost = BOOST_100;
	dev->conf2.CPSIEN = 0;
	dev->conf2.PSIEN = true;
	dev->p_offset_ur = 0;
	dev->p_offset_ds = 0;
	dev->pconf3.p_mask_r = false;
	dev->pconf3.p_mask_l = false;
	dev->pconf3.p_mask_d = false;
	dev->pconf3.p_mask_u = false;
	dev->pconf3.sleep_after_irq = false;
	dev->pconf3.p_gain_cmp = false;
	dev->ges_prox_enter = 0;
	dev->get_prox_exit = 0;
	dev->gconf1.gexpers = 0;
	dev->gconf1.gexmask = 0;
	dev->gconf1.gfifoth = FIFO_1;
	dev->gconf2.gwtime = 0;
	dev->gconf2.led_c = LED_100mA;
	dev->gconf2.ggain = GAIN_1x;
	dev->g_offset_u = 0;
	dev->g_offset_d = 0;
	dev->g_offset_l = 0;
	dev->g_offset_r = 0;
	dev->g_pulse_cnt.g_pulse = 0;
	dev->g_pulse_cnt.gplen = PLEN_8_US;
	dev->gconf3.up_down_gest_enable = false;
	dev->gconf3.left_right_gest_enable = false;
}

void apds9960_recomended_settings(struct apds9960 *dev)
{
	apds9960_use_default(dev);
	dev->gconf1.gfifoth = FIFO_16;
	dev->gconf3.up_down_gest_enable = true;
	dev->gconf3.left_right_gest_enable = true;
	dev->g_pulse_cnt.g_pulse = 10;
	dev->g_pulse_cnt.gplen = PLEN_16_US;
	dev->gconf2.gwtime = 2;
	dev->gconf2.led_c = LED_100mA;
	dev->gconf2.ggain = GAIN_4x;
	dev->ctrl1.again = GAIN_4x;
	dev->ctrl1.pgain = GAIN_4x;
	dev->ctrl1.led_c = LED_100mA;
	dev->prox_pulse_cnt.ppulse = 8;
	dev->prox_pulse_cnt.pplen = PLEN_16_US;
	dev->conf1.wlong = false;
	dev->persistance.apers = 1;
	dev->persistance.ppers = 1;
	dev->prox_treshold.low_threshold = 0;
	dev->prox_treshold.high_threshold = 50;
	dev->atime = 219;
	dev->wtime = 246;
	dev->als_treshold.low_threshold = 0xFFFF;
	dev->als_treshold.high_threshold = 0;
	dev->ges_prox_enter = 40;
	dev->get_prox_exit = 30;
}

/*
 * Init for common use
 */
uint8_t apds9960_init(struct apds9960 *dev, bool use_default,
		      bool use_recommended)
{
	uint8_t id, ret;
	ret = dev->read_reg(APDS_ID_REG, &id, sizeof(id));

	if (ret || id != APDS_ID)
		return ret;

	if (use_recommended)
		apds9960_recomended_settings(dev);
	else if (use_default)
		apds9960_use_default(dev);

	dev->write_reg(APDS_ATIME_REG, dev->atime);
	dev->write_reg(APDS_WTIME_REG, dev->wtime);
	dev->write_reg(APDS_AILTL_REG, dev->als_treshold.low_threshold >> 0);
	dev->write_reg(APDS_AILTH_REG, dev->als_treshold.low_threshold >> 8);
	dev->write_reg(APDS_AIHTL_REG, dev->als_treshold.high_threshold >> 0);
	dev->write_reg(APDS_AIHTH_REG, dev->als_treshold.high_threshold >> 8);
	dev->write_reg(APDS_PILT_REG, dev->prox_treshold.low_threshold);
	dev->write_reg(APDS_PIHT_REG, dev->prox_treshold.high_threshold);
	dev->write_reg(APDS_PERS_REG, *(uint8_t *)&dev->persistance);
	dev->write_reg(APDS_CONFIG1_REG, *(uint8_t *)&dev->conf1);
	dev->write_reg(APDS_PPULSE_REG, *(uint8_t *)&dev->prox_pulse_cnt);
	dev->write_reg(APDS_CONTROL_REG, *(uint8_t *)&dev->ctrl1);
	dev->write_reg(APDS_CONFIG2_REG, *(uint8_t *)&dev->conf2 | 1);
	dev->write_reg(APDS_POFFSET_UR_REG, dev->p_offset_ur);
	dev->write_reg(APDS_POFFSET_DL_REG, dev->p_offset_ds);
	dev->write_reg(APDS_CONFIG3_REG, *(uint8_t *)&dev->pconf3);
	dev->write_reg(APDS_GPENTH_REG, dev->ges_prox_enter);
	dev->write_reg(APDS_GEXTH_REG, dev->get_prox_exit);
	dev->write_reg(APDS_GCONF1_REG, *(uint8_t *)&dev->gconf1);
	dev->write_reg(APDS_GCONF2_REG, *(uint8_t *)&dev->gconf2);
	dev->write_reg(APDS_GOFFSET_U_REG, dev->g_offset_u);
	dev->write_reg(APDS_GOFFSET_D_REG, dev->g_offset_d);
	dev->write_reg(APDS_GOFFSET_L_REG, dev->g_offset_l);
	dev->write_reg(APDS_GOFFSET_R_REG, dev->g_offset_r);
	dev->write_reg(APDS_GPULSE_REG, *(uint8_t *)&dev->g_pulse_cnt);
	dev->write_reg(APDS_GCONF3_REG, *(uint8_t *)&dev->gconf3);

	return ret;
}

/*
 * Read whte, red, green and blue diode response
 */
uint8_t apds9960_meas_crgb(struct apds9960 *dev, struct rgbs_data *crgb)
{
	dev->write_reg(APDS_ENABLE_REG,
		       APDS_ENABLE_AEN | APDS_ENABLE_AIEN |
		       APDS_ENABLE_PON | APDS_ENABLE_WEN);

	wait_for_irq(dev, APDS_STATUS_AVALID_BIT);

	dev->read_reg(APDS_CDATAL_REG, (uint8_t *)crgb, sizeof(*crgb));

	return sleep_mode(dev);
}

/*
 * Wait for proximity even
 */
uint8_t apds9960_proximity(struct apds9960 *dev)
{
	uint8_t ret;

	dev->write_reg(APDS_ENABLE_REG,
		       APDS_ENABLE_PEN | APDS_ENABLE_PIEN |
		       APDS_ENABLE_PON | APDS_ENABLE_WEN);

	ret = wait_for_irq(dev, APDS_STATUS_PINT_BIT | APDS_STATUS_PVALID_BIT);

	sleep_mode(dev);

	return ret & APDS_STATUS_PINT_BIT;
}

enum apds_gesture apds9960_gesture(struct apds9960 *dev)
{
	uint8_t ret;
	uint8_t gvalid;
	uint8_t fifo_level, average_cnt;
	struct gesture_data gesture;
	struct {
		struct gesture_data data;
		bool is_filled;
	} min = { .is_filled = false }, max = { .is_filled = false };
	enum apds_gesture res = NO_ACTIVITY;

	dev->write_reg(APDS_ENABLE_REG,
		       APDS_ENABLE_GEN | APDS_ENABLE_PEN | APDS_ENABLE_AEN |
		       APDS_ENABLE_PON | APDS_ENABLE_WEN);

	ret = wait_for_irq(dev, APDS_STATUS_GINT_BIT);
	if (!ret) {
		res = NO_ACTIVITY;
		goto exit;
	}

	dev->read_reg(APDS_GSTATUS_REG, &gvalid, sizeof(gvalid));
	if (!gvalid) {
		res = ERR_DATA_INVALID;
		goto exit;
	}

	dev->read_reg(APDS_GFLVL_REG, &fifo_level, sizeof(fifo_level));
	if (!fifo_level) {
		res = ERR_FIFO_EMPTY;
		goto exit;
	}

	average_cnt = fifo_level;
	while(fifo_level--) {
		dev->read_reg(APDS_GFIFO_U_REG,
			      (uint8_t *)&gesture, sizeof(gesture));
		if (validate_gest_data(&gesture)) {
			if (!min.is_filled) { /* save first valid */
				min.data.up = gesture.up;
				min.data.down = gesture.down;
				min.data.left = gesture.left;
				min.data.right = gesture.right;
				min.is_filled = true;
			} else { /* save last valid */
				max.data.up = gesture.up;
				max.data.down = gesture.down;
				max.data.left = gesture.left;
				max.data.right = gesture.right;
				max.is_filled = true;
			}
		}
	}

exit:
	sleep_mode(dev);

	if (min.is_filled && max.is_filled)
		return proccess_gesture(&min.data, &max.data);

	return res;
}
