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

enum apds_current {
	LED_100mA,
	LED_50mA,
	LED_25mA,
	LED_12mA,
};

enum apds_gain {
	GAIN_1x,
	GAIN_2x,
	GAIN_4x,
	GAIN_8x,
};

enum apds_led_boost {
	BOOST_100,
	BOOST_150,
	BOOST_200,
	BOOST_300,
};

enum apds_gesture {
	NO_ACTIVITY,
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	ERROR,
};

enum apds_plen {
	PLEN_4_US,
	PLEN_8_US,
	PLEN_16_US,
	PLEN_32_US,
};

struct rgbs_data {
	uint16_t clear;
	uint16_t red;
	uint16_t green;
	uint16_t blue;
};

struct gesture_data {
	uint8_t up;
	uint8_t down;
	uint8_t left;
	uint8_t right;
};

struct apds9960 {
	/* Interface functions */
	uint8_t (*write_reg)(uint8_t reg, uint8_t value);
	uint8_t (*read_reg) (uint8_t reg, uint8_t *buf, uint16_t size);
	bool (*check_irq)(void); /* To read GPIO */
	/* For external interrupt, return status reg value */
	uint8_t (*wait_for_irq)(struct apds9960 *dev, uint8_t bit);

	uint8_t atime; /* ADC Integration Time Register (0x81) */
	uint8_t wtime; /* Wait Time Register (0x83) */

	/* ALS Interrupt Threshold (0x84 – 0x87) */
	struct {
		uint16_t low_threshold;
		uint16_t high_threshold;
	} als_treshold;

	/* Proximity Interrupt Threshold (0x89/0x8B) */
	struct {
		uint8_t low_threshold;
		uint8_t high_threshold;
	} prox_treshold;

	/* Persistence Register (0x8C) */
	struct {
		uint8_t apers : 4; /* ALS Interrupt Persistence */
		uint8_t ppers : 4; /* Proximity Interrupt Persistence */
	} persistance;

	/* Configuration Register One (0x8D) */
	struct {
		uint8_t : 1;
		bool wlong : 1;
		uint8_t : 6;
	} conf1;

	/* Proximity Pulse Count Register (0x8E) */
	struct {
		uint8_t ppulse : 6;
		enum apds_plen pplen : 2;
	} prox_pulse_cnd;

	/* Control Register One (0x8F) */
	struct {
		enum apds_gain again : 2;
		enum apds_gain pgain : 2;
		uint8_t : 2;
		enum apds_current led_c : 2;
	} ctrl1;

	/* Configuration Register Two (0x90) */
	struct {
		uint8_t : 4;
		enum apds_led_boost led_boost : 2;
		bool CPSIEN : 1;
		bool PSIEN : 1;
	} conf2;

	uint8_t p_offset_ur; /* Proximity Offset UP / RIGHT Register (0x9D) */
	uint8_t p_offset_ds; /* Proximity Offset DOWN / LEFT Register (0x9E) */

	/* Configuration Three Register (0x9F) */
	struct {
		bool p_mask_r : 1;
		bool p_mask_l : 1;
		bool p_mask_d : 1;
		bool p_mask_u : 1;
		bool sleep_after_irq : 1;
		bool p_gain_cmp : 1;
		uint8_t : 2;
	} pconf3;
	
	uint8_t ges_prox_enter; /* Gesture Proximity Enter Threshold (0xA0) */
	uint8_t get_prox_exit; /* Gesture Exit Threshold Register (0xA1) */

	/* Gesture Configuration One Register (0xA2) */
	struct {
		uint8_t gexpers : 2;
		uint8_t gexmask : 4;
		uint8_t gfifoth : 2;
	} gconf1;

	/* Gesture configuration two (0xA3) */
	struct {
		uint8_t gwtime : 3;
		enum apds_current led_c : 2;
		enum apds_gain ggain : 2;
		uint8_t : 1;
	} gconf2;

	uint8_t g_offset_u; /* Gesture UP Offset Register (0xA4) */
	uint8_t g_offset_d; /* Gesture DOWN Offset Register (0xA5) */
	uint8_t g_offset_l; /* Gesture LEFT Offset Register (0xA7) */
	uint8_t g_offset_r; /* Gesture RIGHT Offset Register (0xA9) */

	/* Gesture Pulse Count and Length Register (0xA6) */
	struct {
		uint8_t g_pulse : 6;
		enum apds_plen gplen : 2; 
	} g_pulse_cnt;

	/* Gesture Configuration Three Register (0xAA) */
	struct {
		bool up_down_gest_enable : 1;
		bool left_right_gest_enable : 1;
		uint8_t : 6;
	} gconf3;
};

void apds9960_use_default(struct apds9960 *);
uint8_t apds9960_init(struct apds9960 *, bool);
uint8_t apds9960_meas_crgb(struct apds9960 *, struct rgbs_data *);
uint8_t apds9960_proximity(struct apds9960 *);

#endif /* __APDS9960_H__ */
