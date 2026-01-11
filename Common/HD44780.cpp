/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2026 Pavel Nadein
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
 * HD44780 4/8 bit LCD/OLED C++ driver source file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "HD44780.h"
#include <cstdarg>
#include <cstdio>

#define LCD_DELAY_HIGH_US 2000
#define LCD_DELAY_LOW_US 50

enum lcd_cmd
{
	CMD_NOP = 0x00,
	CMD_CLEAR = 0x01,
	CMD_HOME = 0x02,
	CMD_8B = 0x03,
	CMD_AUTOINCREMENTON = 0x06,
	CMD_BLANK = 0x08,
	CMD_CURSORLEFT = 0x10,
	CMD_CURSORRIGHT = 0x14,
	CMD_POWERON = 0x17,
	CMD_4B_1LINE = 0x20,
	CMD_4B_2LINES = 0x28,
	CMD_8B_1LINE = 0x30,
	CMD_8B_2LINES = 0x38,
	CMD_SCROLLRIGHT = 0x1E,
	CMD_SCROLLLEFT = 0x18,
	CMD_UNDERLINECURSORON = 0x0E,
	CMD_BLOCKCURSORON = 0x0F,
	CMD_CURSOROFF = 0x0C,
	CMD_SETCGRAM = 0x40,
	NOP_DATA = 0xFF,
};

HD44780_Base &HD44780_Base::latch_write(uint16_t data)
{
	/* Send positive strobe */
	interface_.Write(data | en_pin_);
	interface_.DelayUs(LCD_DELAY_LOW_US);

	/* Send negative strobe */
	interface_.Write(data);
	interface_.DelayUs(LCD_DELAY_LOW_US);

	return *this;
}

HD44780_Base &HD44780_Base::intfs_write(uint16_t data, bool is_cmd)
{
	if (is_backlight_enabled_)
		data |= (1 << bl_pin_);

	if (!is_cmd)
		data |= (1 << rs_pin_);

	if (data == NOP_DATA)
	{
		interface_.Write(data);
		return *this;
	}

	if (bus_width_ == BUS_4B)
	{
		/* Send the high nible */
		latch_write((data & 0xF0) >> 4);

		/* Send the low nible */
		latch_write(data & 0x0F);
	}
	else
	{
		latch_write(data);
	}
	return *this;
}

HD44780_Base &HD44780_Base::Init(LCD_Type type, Font font)
{
	if (bus_width_ == BUS_4B)
	{
		latch_write(0x03);
		latch_write(0x03);
		latch_write(0x03);
		latch_write(0x02);
		interface_.DelayUs(LCD_DELAY_HIGH_US);
		intfs_write(CMD_4B_2LINES | font);
	}

	if (type == HD44780_Base::LCD)
	{
		intfs_write(CMD_HOME);
		intfs_write(CMD_CLEAR);
		interface_.DelayUs(LCD_DELAY_HIGH_US);
		intfs_write(CMD_CURSOROFF);
	}
	else
	{
		intfs_write(CMD_BLANK);
		intfs_write(CMD_AUTOINCREMENTON);
		intfs_write(CMD_POWERON);
		intfs_write(CMD_CLEAR);
		interface_.DelayUs(LCD_DELAY_HIGH_US);
		intfs_write(CMD_HOME);
		intfs_write(CMD_CURSOROFF);
	}

	return *this;
}

HD44780_Base &HD44780_Base::SetBacklight(bool is_enabled)
{
	is_backlight_enabled_ = is_enabled;
	intfs_write(NOP_DATA);
	return *this;
}

HD44780_Base &HD44780_Base::SetCursorPos(uint8_t row, uint8_t col)
{
	uint8_t position = 0x80;

	if (row)
		position |= 0x40;

	intfs_write(position | col);
	return *this;
}

HD44780_Base &HD44780_Base::Clear()
{
	intfs_write(CMD_CLEAR);
	interface_.DelayUs(LCD_DELAY_HIGH_US);
	intfs_write(CMD_HOME);
	interface_.DelayUs(LCD_DELAY_HIGH_US);
	return *this;
}

int HD44780_Base::Print(const char *format, ...)
{
	char buffer[16 * 2 + 1 + 1]; // 2 lines of 16 characters + new line + null terminator
	va_list args;
	va_start(args, format);
	int len = vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	for (int i = 0; i < len; i++)
	{
		PutChar(buffer[i]);
	}

	return len;
}

void HD44780_Base::PutChar(char data)
{
	if (data == '\n')
	{
		intfs_write(0x80 | 0x40);
		return;
	}

	intfs_write(data, false);
}