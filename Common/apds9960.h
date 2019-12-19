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

#ifndef __APDS9960_H__
#define __APDS9960_H__

#include <stdint.h>
#include <stdbool.h>

/* LED Drive Strength */
enum apds_led_c {
	LED_100mA,
	LED_50mA,
	LED_25mA,
	LED_12mA,
};

/* Proximity Gain Control */
enum apds_pgain {
	PGAIN_1x,
	PGAIN_2x,
	PGAIN_4x,
	PGAIN_8x,
};

/* ALS and Color Gain Control */
enum apds_again {
	AGAIN_1x,
	AGAIN_2x,
	AGAIN_4x,
	AGAIN_8x,
};

/*
 * Additional LDR current during proximity and gesture LED pulses.
 * Current value, set by LDRIVE, is increased by the percentage of LED_BOOST.
*/
enum apds_led_boost {
	BOOST_100,
	BOOST_150,
	BOOST_200,
	BOOST_300,
};

struct rgbs_data {
	uint16_t c, r, g, b;
};

struct apds9960 {
	/* Interface functions */
	uint8_t (*write_reg)(uint8_t reg, uint8_t value);
	uint8_t (*read_reg) (uint8_t reg, uint8_t *buf, uint16_t size);
	bool (*check_irq)(void); /* To read GPIO */
	/* For external interrupt, return status reg value */
	uint8_t (*wait_for_irq)(struct apds9960 *dev, uint8_t bit);

	struct {
		enum apds_again again : 2;
		enum apds_pgain pgain : 2;
		uint8_t : 2;
		enum apds_led_c led_c : 2;
	} ctrl1;

	struct {
		uint8_t : 4;
		enum apds_led_boost led_boost : 2;
		bool CPSIEN : 1;
		bool PSIEN : 1;
	} conf2;
	uint8_t proxy_low_tresh, proxy_high_tres;
};

uint8_t apds9960_init(struct apds9960 *);
uint8_t apds9960_meas_crgb(struct apds9960 *, struct rgbs_data *);
uint8_t apds9960_proximity(struct apds9960 *);

#endif /* __APDS9960_H__ */
