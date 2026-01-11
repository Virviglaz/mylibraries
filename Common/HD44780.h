/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2022-2026 Pavel Nadein
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
 * HD44780 4/8 bit LCD/OLED driver for C and C++
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __HD44780_H__
#define __HD44780_H__

#include <stdint.h>

#ifdef __cplusplus

/**
 * HD44780 Interface Base Class
 */
class HD44780_InterfaceBase
{
public:
	/**
	 * @brief Data write interface function
	 */
	virtual void Write(uint16_t) = 0;

	/**
	 * @brief Delay function in microseconds
	 */
	virtual void DelayUs(uint16_t) {};
};

/**
 * HD44780 Alphanumeric display base class
 */
class HD44780_Base
{
public:
	/**
	 * LCD type (LCD or OLED)
	 */
	enum LCD_Type
	{
		LCD,
		OLED,
	};

	/**
	 * Bus width type (4 or 8 bit)
	 */
	enum BusWidth
	{
		BUS_4B,
		BUS_8B,
	};

	/**
	 * Font table type
	 */
	enum Font
	{
		ENGLISH_JAPANESE_FONT,
		WESTERN_EUROPEAN_FONT,
		ENGLISH_RUSSIAN_FONT,
		WESTERN_EUROPEAN2_FONT,
	};

	explicit HD44780_Base() = delete;
	virtual ~HD44780_Base() = default;

	/**
	 * @brief Constructor with interface and pin configuration
	 *
	 * @param ifs Reference to interface object
	 * @param bus_width Bus width type
	 * @param en_pin Enable pin number
	 * @param rs_pin Register select pin number
	 * @param bl_pin Backlight control pin number
	 */
	HD44780_Base(HD44780_InterfaceBase &ifs,
				 BusWidth bus_width,
				 uint16_t en_pin,
				 uint16_t rs_pin,
				 uint16_t bl_pin) :
				 interface_(ifs),
				 bus_width_(bus_width),
				 en_pin_(en_pin),
				 rs_pin_(rs_pin),
				 bl_pin_(bl_pin) {};

	/**
	 * @brief Initialize the display
	 *
	 * @param type LCD type
	 * @param font Font table type
	 * @return Reference to self
	 */
	virtual HD44780_Base &Init(LCD_Type type, Font font);

	/**
	 * @brief Set backlight state
	 *
	 * @param is_enabled true to enable backlight, false to disable
	 * @return Reference to self
	 */
	virtual HD44780_Base &SetBacklight(bool is_enabled);

	/**
	 * @brief Set cursor position
	 *
	 * @param row Row number (0 or 1)
	 * @param col Column number (0..15)
	 * @return Reference to self
	 */
	virtual HD44780_Base &SetCursorPos(uint8_t row, uint8_t col);

	/**
	 * @brief Clear the display
	 *
	 * @return Reference to self
	 */
	virtual HD44780_Base &Clear();

	/**
	 * @brief Print a null-terminated string
	 *
	 * @param str Pointer to the string
	 * @return Reference to self
	 */
	virtual int Print(const char *format, ...) __attribute__ ((format (printf, 2, 3)));
private:
	HD44780_InterfaceBase &interface_;
	BusWidth bus_width_;
	uint16_t en_pin_;
	uint16_t rs_pin_;
	uint16_t bl_pin_;

	bool is_backlight_enabled_;

	HD44780_Base &latch_write(uint16_t data);
	HD44780_Base &intfs_write(uint16_t data, bool is_cmd = true);
	void PutChar(char data);
};

#else /* __cplusplus */

enum hd44780_lcd_type
{
	HD44780_TYPE_LCD,
	HD44780_TYPE_OLED,
};

enum hd44780_bus_width
{
	HD44780_BUS_4B,
	HD44780_BUS_8B,
};

enum hd44780_font_table
{
	HD44780_ENGLISH_JAPANESE_FONT,
	HD44780_WESTERN_EUROPEAN_FONT,
	HD44780_ENGLISH_RUSSIAN_FONT,
	HD44780_WESTERN_EUROPEAN2_FONT,
};

struct hd44780_ext_con
{
	uint8_t ext_pin[2];
};

struct hd44780_conn
{
	enum hd44780_bus_width bus_type;
	uint8_t data_shift;
	uint8_t rs_pin;
	uint8_t en_pin;
	uint8_t backlight_pin;
};

struct hd44780_lcd
{
	void (*write)(uint8_t data);	/* for 4 bit connection */
	void (*write16)(uint16_t data); /* for 8 bit connection */
	void (*delay_us)(uint16_t us);

	/* Settings */
	uint8_t is_backlight_enabled;
	enum hd44780_lcd_type type;
	enum hd44780_font_table font;

	/* Connection definition structure */
	struct hd44780_conn *conn;
	struct hd44780_ext_con *ext_con;
};

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

#endif /* __cplusplus */

#endif /* __HD44780_H__ */
