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

#include "HD44780.h"

#ifndef BIT
#define BIT(x)						(1 << x)
#endif

enum lcd_cmd {
	HD44780_CMD_NOP					= 0x00,
	HD44780_CMD_CLEAR				= 0x01,
	HD44780_CMD_HOME				= 0x02,
	HD44780_CMD_8B					= 0x03,
	HD44780_CMD_AUTOINCREMENTON			= 0x06,
	HD44780_CMD_BLANK				= 0x08,
	HD44780_CMD_CURSORLEFT				= 0x10,
	HD44780_CMD_CURSORRIGHT				= 0x14,
	HD44780_CMD_POWERON				= 0x17,
	HD44780_CMD_4B_1LINE				= 0x20,
	HD44780_CMD_4B_2LINES				= 0x28,
	HD44780_CMD_8B_1LINE				= 0x30,
	HD44780_CMD_8B_2LINES				= 0x38,
	HD44780_CMD_SCROLLRIGHT				= 0x1E,
	HD44780_CMD_SCROLLLEFT				= 0x18,
	HD44780_CMD_UNDERLINECURSORON			= 0x0E,
	HD44780_CMD_BLOCKCURSORON			= 0x0F,
	HD44780_CMD_CURSOROFF				= 0x0C,
	HD44780_CMD_SETCGRAM				= 0x40,
	HD44780_NOP_DATA				= 0xFF,
};

#ifndef LCD_DELAY_LOW
#define LCD_DELAY_LOW					50
#endif

#ifndef LCD_DELAY_HIGH
#define LCD_DELAY_HIGH					2000
#endif

static const struct hd44780_conn pcf8574_con = {
	HD44780_BUS_4B, 4, 1 << 0, 1 << 2, 1 << 3 };
static const struct hd44780_conn mc74hC595_con = {
	HD44780_BUS_4B, 4, 1 << 2, 1 << 3, 3 };

static struct hd44780_lcd *dev = 0;
static void (*wr)(uint8_t data, uint8_t cmd);

static void write_to_bus(uint8_t dat)
{
	/* Send positive strobe */
	dat |= dev->conn->en_pin;
	dev->write(dat);

	dev->delay_us(LCD_DELAY_LOW);

	/* Send negative strobe */
	dat &= ~dev->conn->en_pin;
	dev->write(dat);

	dev->delay_us(LCD_DELAY_LOW);
}

static void wr4b(uint8_t data, uint8_t cmd)
{ 
	uint8_t latch = 0;

	if (!cmd)
		latch = dev->conn->rs_pin;

	if (dev->is_backlight_enabled)
		latch |= dev->conn->backlight_pin;

	if (dev->ext_con)
		latch |= dev->ext_con->ext_pin[0] | dev->ext_con->ext_pin[1];

	if (cmd == HD44780_NOP_DATA) {
		dev->write(latch);
		return;
	}	

	/* Send the high nible */
	write_to_bus(latch | ((data & 0xF0) >> 4) << dev->conn->data_shift);
	
	/* Send the low nible */
	write_to_bus(latch | ((data & 0x0F) >> 0) << dev->conn->data_shift);
}

static void wr8b(uint8_t data, uint8_t cmd)
{
	uint16_t latch = (data << 8) | dev->is_backlight_enabled;

	if (!cmd)
		latch |= dev->conn->rs_pin;
 
	if (dev->ext_con)
		latch |= dev->ext_con->ext_pin[0] | dev->ext_con->ext_pin[1];

	dev->write16(latch);
}

static void wr_cmd(enum lcd_cmd CMD)
{
	wr(CMD, 1);

	dev->delay_us(LCD_DELAY_HIGH);
}

static char translate(unsigned char dat)
{
	const unsigned char rus_font[] = {
		'A', 0xA0, 'B', 0xA1, 0xE0,'E', 0xA3, 0xA4, 0xA5, 0xA6,'K',
		0xA7,'M', 'H', 'O', 0xA8,'P', 'C', 'T', 0xA9, 0xAA,'X',
		0xE1, 0xAB, 0xAC, 0xE2, 0xAD, 0xAE,'b', 0xA2, 0xB0, 0xB1,
		'a', 0xB2, 0xB3, 0xB4, 0xE3,'e', 0xB6, 0xB7, 0xB8, 0xB9,
		0xBA, 0xBB, 0xBC, 0xBD,'o', 0xBE,'p', 'c', 0xBF,'y', 0xE4,'x',
		0xE5, 0xC0, 0xC1, 0xE6, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7
	};

	 /* support only russian for now */
	if (dev->font != HD44780_ENGLISH_RUSSIAN_FONT)
		return dat;
	if (dat < 0xC0)
		return dat;
  
	return rus_font[dat - 0xC0];
}

struct hd44780_lcd *hd44780_init(struct hd44780_lcd *drv)
{
	if (!drv || !drv->conn)
		return dev;

	dev = drv;

	wr = dev->conn->bus_type ? wr8b : wr4b;

	if (dev->conn->bus_type == HD44780_BUS_4B) {
		write_to_bus(0x03 << dev->conn->data_shift);
		write_to_bus(0x03 << dev->conn->data_shift);
		write_to_bus(0x03 << dev->conn->data_shift);
		write_to_bus(0x02 << dev->conn->data_shift);
		dev->delay_us(LCD_DELAY_HIGH);
		wr(HD44780_CMD_4B_2LINES | dev->font, 1);
	}

	if (dev->type == HD44780_TYPE_LCD) {
		wr_cmd(HD44780_CMD_HOME);
		wr_cmd(HD44780_CMD_CLEAR);
		dev->delay_us(LCD_DELAY_HIGH);
		wr_cmd(HD44780_CMD_CURSOROFF);
	} else if (dev->type == HD44780_TYPE_OLED) {
		wr_cmd(HD44780_CMD_BLANK);
		wr_cmd(HD44780_CMD_AUTOINCREMENTON);
		wr_cmd(HD44780_CMD_POWERON);
		wr_cmd(HD44780_CMD_CLEAR);
		dev->delay_us(LCD_DELAY_HIGH);
		wr_cmd(HD44780_CMD_HOME);
		wr_cmd(HD44780_CMD_CURSOROFF);
	}

	return dev;
}

void hd44780_pcf8574_con_init(struct hd44780_lcd *drv)
{
	drv->conn = (void *)&pcf8574_con;
}

void hd44780_mc74hC595_con_init(struct hd44780_lcd *drv)
{
	drv->conn = (void *)&mc74hC595_con;
}

void hd44780_set_pos(uint8_t row, uint8_t col)
{
	uint8_t position = 0x80;

	if (row)
		position |= 0x40;

	wr_cmd((enum lcd_cmd)(position | col));
}

void hd44780_print(char *string)
{
	while (*string)
		hd44780_put_char(*string++);
}

void hd44780_clear(void)
{
	wr_cmd(HD44780_CMD_CLEAR);

	dev->delay_us(LCD_DELAY_HIGH);

	wr_cmd(HD44780_CMD_HOME);

	dev->delay_us(LCD_DELAY_HIGH);
}

void hd44780_put_char(unsigned char data)
{
	if (data == '\n') {
		wr_cmd((enum lcd_cmd)(0x80 | 0x40));
		return;
	}
	
	data = translate(data);
	wr(data, 0);
}

void hd44780_update(void)
{
	wr_cmd(HD44780_NOP_DATA);
}

void hd44780_custom_char(uint8_t num, uint8_t *data)
{
	uint8_t cnt;

	wr (HD44780_CMD_SETCGRAM + num * 8, 1);

	for (cnt = 0; cnt != 8; cnt++)
		wr(data[cnt], 0);

	wr_cmd(HD44780_CMD_HOME);
}

void hd44780_send_cmd(uint8_t cmd)
{
	wr_cmd((enum lcd_cmd)cmd);
}
