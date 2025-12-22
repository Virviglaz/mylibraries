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
 * HD44780 4/8 bit LCD/OLED driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __HD44780_H__
#define __HD44780_H__

#include <stdint.h>

enum hd44780_lcd_type {
	HD44780_TYPE_LCD,
	HD44780_TYPE_OLED,
};

enum hd44780_bus_width {
	HD44780_BUS_4B,
	HD44780_BUS_8B,
};

enum hd44780_font_table {
	HD44780_ENGLISH_JAPANESE_FONT,
	HD44780_WESTERN_EUROPEAN_FONT,
	HD44780_ENGLISH_RUSSIAN_FONT,
	HD44780_WESTERN_EUROPEAN2_FONT,
};

struct hd44780_ext_con {
	uint8_t ext_pin[2];
};

struct hd44780_conn {
	enum hd44780_bus_width bus_type;
	uint8_t data_shift;
	uint8_t rs_pin;
	uint8_t en_pin;
	uint8_t backlight_pin;
};

struct hd44780_lcd {
	void (*write)(uint8_t data);	/* for 4 bit connection */
	void (*write16)(uint16_t data);	/* for 8 bit connection */
	void (*delay_us)(uint16_t us);
  
	/* Settings */
	uint8_t is_backlight_enabled;
	enum hd44780_lcd_type type;
	enum hd44780_font_table font;
  
	/* Connection definition structure */
	struct hd44780_conn *conn;
	struct hd44780_ext_con *ext_con;
};

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * Initialize the device and store the pointer to the configuration structure.
 *
 * @param drv Pointer to the local configuration structure.
 */
struct hd44780_lcd *hd44780_init(struct hd44780_lcd *drv);

/**
 * Initialize the typical connection for popular PCF8574 based board.
 *
 * @param drv Pointer to the local configuration structure.
 */
void hd44780_pcf8574_con_init(struct hd44780_lcd *drv);

/**
 * Initialize the typical connection for 74hC595 shif register based board.
 *
 * @param drv Pointer to the local configuration structure.
 */
void hd44780_mc74hC595_con_init(struct hd44780_lcd *drv);

/**
 * Move cursor to certain position.
 *
 * @param row [0..1] row value.
 * @param col [0..15] column value.
 */
void hd44780_set_pos(uint8_t row, uint8_t col);

/**
 * Print the null-terminated string.
 *
 * @param string pointer to string.
 */
void hd44780_print(char *string);

/**
 * Clears the screen.
 */
void hd44780_clear(void);

/**
 * Print single char at current cursor position.
 *
 * @param data character to print.
 */
void hd44780_put_char(unsigned char data);

/**
 * Send nop command needed to update extension pins.
 */
void hd44780_update(void);

/**
 * Programm the additional custom character in GRAM.
 *
 * @param num number of character in GRAM.
 * @param data pointer to bit data.
 */
void hd44780_custom_char(uint8_t num, uint8_t *data);

/**
 * Send cmd to controller.
 *
 * @param cmd custom command code.
 */
void hd44780_send_cmd(uint8_t cmd);

#ifdef __cplusplus
}
#endif

#endif /* __HD44780_H__ */
