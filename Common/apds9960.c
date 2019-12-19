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
#define APDS_ATIME_REG		0x81 /* R/W ADC integration time */
#define APDS_WTIME_REG		0x83 /* R/W Wait time (non-gesture) */
#define APDS_AILTL_REG		0x84 /* R/W ALS interrupt low threshold low byte */
#define APDS_AILTH_REG		0x85 /* R/W ALS interrupt low threshold high byte */
#define APDS_AIHTL_REG		0x86 /* R/W ALS interrupt high threshold low byte */
#define APDS_AIHTH_REG		0x87 /* R/W ALS interrupt high threshold high byte */
#define APDS_PILT_REG		0x89 /* R/W Proximity interrupt low threshold */
#define APDS_PIHT_REG		0x8B /* R/W Proximity interrupt high threshold */
#define APDS_PERS_REG		0x8C /* R/W Interrupt persistence filters (non-gesture) */
#define APDS_CONFIG1_REG	0x8D /* R/W Configuration register one */
#define APDS_PPULSE_REG		0x8E /* R/W Proximity pulse count and length */
#define APDS_CONTROL_REG	0x8F /* R/W Gain control */
#define APDS_CONFIG2_REG	0x90 /* R/W Configuration register two */
#define APDS_ID_REG		0x92 /* R Device ID */
#define APDS_STATUS_REG		0x93 /* R Device status */
#define APDS_CDATAL_REG		0x94 /* R Low byte of clear channel data */
#define APDS_CDATAH_REG		0x95 /* R High byte of clear channel data */
#define APDS_RDATAL_REG		0x96 /* R Low byte of red channel data */
#define APDS_RDATAH_REG		0x97 /* R High byte of red channel data */
#define APDS_GDATAL_REG		0x98 /* R Low byte of green channel data */
#define APDS_GDATAH_REG		0x99 /* R High byte of green channel data */
#define APDS_BDATAL_REG		0x9A /* R Low byte of blue channel data */
#define APDS_BDATAH_REG		0x9B /* R High byte of blue channel data */
#define APDS_PDATA_REG		0x9C /* R Proximity data */
#define APDS_POFFSET_UR_REG	0x9D /* R/W Proximity offset for UP and RIGHT photodiodes */
#define APDS_POFFSET_DL_REG	0x9E /* R/W Proximity offset for DOWN and LEFT photodiodes */
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
#define APDS_GFLVL_REG		0xAE /* R Gesture FIFO level */
#define APDS_GSTATUS_REG	0xAF /* R Gesture status */
#define APDS_IFORCE_REG		0xE4 /* W Force interrupt */
#define APDS_PICLEAR_REG	0xE5 /* W Proximity interrupt clear */
#define APDS_CICLEAR_REG	0xE6 /* W ALS clear channel interrupt clear */
#define APDS_AICLEAR_REG	0xE7 /* W All non-gesture interrupts clear */
#define APDS_GFIFO_U_REG	0xFC /* R Gesture FIFO UP value */
#define APDS_GFIFO_D_REG	0xFD /* R Gesture FIFO DOWN value */
#define APDS_GFIFO_L_REG	0xFE /* R Gesture FIFO LEFT value */
#define APDS_GFIFO_R_REG	0xFF /* R Gesture FIFO RIGHT value */

#define APDS_STATUS_CPSAT_BIT	BIT(7)
#define APDS_STATUS_PGSAT_BIT	BIT(6)
#define APDS_STATUS_PINT_BIT	BIT(5)
#define APDS_STATUS_AINT_BIT	BIT(4)
#define APDS_STATUS_GINT_BIT	BIT(2)
#define APDS_STATUS_PVALID_BIT	BIT(1)
#define APDS_STATUS_AVALID_BIT	BIT(0)

#define APDS_ENABLE_GEN		BIT(6)
#define APDS_ENABLE_PIEN	BIT(5)
#define APDS_ENABLE_AIEN	BIT(4)
#define APDS_ENABLE_WEN		BIT(3)
#define APDS_ENABLE_PEN		BIT(2)
#define APDS_ENABLE_AEN		BIT(1)
#define APDS_ENABLE_PON		BIT(0)

/* ID Register (0x92) value, page 25 */
#define APDS_ID			0xAB

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

/*
 * Init for common use
 */
uint8_t apds9960_init(struct apds9960 *dev)
{
	uint8_t id, ret;
	ret = dev->read_reg(APDS_ID_REG, &id, sizeof(id));

	if (ret || id != APDS_ID)
		return ret;

	ret |= dev->write_reg(APDS_CONTROL_REG, *(uint8_t *)&dev->ctrl1);
	ret |= dev->write_reg(APDS_CONFIG2_REG, *(uint8_t *)&dev->conf2);

	if (dev->proxy_high_tres) {
		dev->write_reg(APDS_PILT_REG, dev->proxy_low_tresh);
		dev->write_reg(APDS_PIHT_REG, dev->proxy_high_tres);
	}

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

	return dev->write_reg(APDS_ENABLE_REG, 0);
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

	dev->write_reg(APDS_ENABLE_REG, 0);

	/* Clear interrupt */
	dev->write_reg(APDS_PICLEAR_REG, 0);
	dev->write_reg(APDS_AICLEAR_REG, 0);

	return ret & APDS_STATUS_PINT_BIT;
}
