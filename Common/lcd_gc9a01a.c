/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2021 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
 * GC9A01A LCD TFT display driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "lcd_gc9a01a.h"

#define COL_ADDR_SET		0x2A
#define ROW_ADDR_SET		0x2B
#define MEM_WR			0x2C
#define MEM_WR_CONT		0x3C
#define MAX_X_SIZE		240
#define MAX_Y_SIZE		240

static void (*wr_data)(const uint8_t *data, uint16_t len);
static void (*wr_cmd)(uint8_t cmd);

void lcd_gc9a01a_init(
	enum gc9a01a_orientation orientation,
	enum gc9a01a_color_mode color_mode,
	void (*wr_d)(const uint8_t *data, uint16_t len),	// write data
	void (*wr_c)(uint8_t cmd),				// write cmd
	void (*delay)(void)					// delay
)
{
	const char o = (char)orientation;
	const char c = (char)color_mode;
	const struct __PACKED {
		uint8_t cmd;
		uint8_t size;
		uint16_t delay;
		const char *data;
	} init_seq[] = {
		{ 0xEF, 0, 1, 0 },
		{ 0xEB, 1, 0, "\x14" },
		{ 0xFE, 0, 0, 0 },
		{ 0xEF, 0, 0, 0 },
		{ 0xEB, 1, 0, "\x14" },
		{ 0x84, 1, 0, "\x40" },
		{ 0x85, 1, 0, "\xFF" },
		{ 0x86, 1, 0, "\xFF" },
		{ 0x87, 1, 0, "\xFF" },
		{ 0x88, 1, 0, "\x0A" },
		{ 0x89, 1, 0, "\x21" },
		{ 0x8A, 1, 0, "\x00" },
		{ 0x8B, 1, 0, "\x80" },
		{ 0x8C, 1, 0, "\x01" },
		{ 0x8D, 1, 0, "\x01" },
		{ 0x8E, 1, 0, "\xFF" },
		{ 0x8F, 1, 0, "\xFF" },
		{ 0xB6, 2, 0, "\x00\x00" },
		{ 0x36, 1, 0, &o },
		{ 0x3A, 1, 0, &c },
		{ 0x90, 4, 0, "\x08\x08\x08\x08" },
		{ 0xBD, 1, 0, "\x06" },
		{ 0xBC, 1, 0, "\x00" },
		{ 0xFF, 3, 0, "\x60\x01\x04" },
		{ 0xC3, 1, 0, "\x13" },
		{ 0xC4, 1, 0, "\x13" },
		{ 0xC9, 1, 0, "\x22" },
		{ 0xBE, 1, 0, "\x11" },
		{ 0xE1, 2, 0, "\x10\x0E" },
		{ 0xDF, 3, 0, "\x21\x0c\x02" },
		{ 0xF0, 6, 0, "\x45\x09\x08\x08\x26\x2A" },
		{ 0xF1, 6, 0, "\x43\x70\x72\x36\x37\x6F" },
		{ 0xF2, 6, 0, "\x45\x09\x08\x08\x26\x2A" },
		{ 0xF3, 6, 0, "\x43\x70\x72\x36\x37\x6F" },
		{ 0xED, 2, 0, "\x1B\x0B" },
		{ 0xAE, 1, 0, "\x77" },
		{ 0xCD, 1, 0, "\x63" },
		{ 0x70, 9, 0, "\x07\x07\x04\x0E\x0F\x09\x07\x08\x03" },
		{ 0xE8, 1, 0, "\x34" },
		{ 0x62, 12,0, "\x18\x0D\x71\xED\x70\x70\x18\x0F\x71\xEF\x70\x70" },
		{ 0xE8, 1, 0, "\x34" },
		{ 0x62, 12,0, "\x18\x0D\x71\xED\x70\x70\x18\x0F\x71\xEF\x70\x70" },
		{ 0x63, 12,0, "\x18\x11\x71\xF1\x70\x70\x18\x13\x71\xF3\x70\x70" },
		{ 0x64, 7, 0, "\x28\x29\xF1\x01\xF1\x00\x07" },
		{ 0x66, 10,0, "\x3C\x00\xCD\x67\x45\x45\x10\x00\x00\x00" },
		{ 0x67, 10,0, "\x00\x3C\x00\x00\x00\x01\x54\x10\x32\x98" },
		{ 0x74, 7, 0, "\x10\x85\x80\x00\x00\x4E\x00" },
		{ 0x98, 2, 0, "\x3e\x07" },
		{ 0x35, 0, 0, 0 },
		{ 0x21, 0, 0, 0 },
		{ 0x11, 0, 0, 0 },
		/* delay(120) */
		{ 0x29, 0, 1, 0 },
		/* delay(20) */
		{ 0, 0, 1, 0 }
	};
	uint8_t n = 0;

	wr_data = wr_d;
	wr_cmd = wr_c;

	while (1) {
		if (init_seq[n].cmd) {
			if (init_seq[n].delay)
				delay();

			wr_cmd(init_seq[n].cmd);

			if (init_seq[n].size)
				wr_data((const uint8_t *)init_seq[n].data,
					init_seq[n].size);
			n++;
		} else
			break;
	}
}

void lcd_gc9a01a_set_window(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	uint8_t frame[4] = { 0 };

	frame[1] = xs;
	frame[3] = xe;
	wr_cmd(COL_ADDR_SET);
	wr_data((const uint8_t *)frame, sizeof(frame));

	frame[1] = ys;
	frame[3] = ye;
	wr_cmd(ROW_ADDR_SET);
	wr_data((const uint8_t *)frame, sizeof(frame));
}

void lcd_gc9a01a_write(const uint8_t *data, uint16_t size)
{
	wr_cmd(MEM_WR);
	wr_data(data, size);
}

void lcd_gc9a01a_fill(uint16_t color)
{
	uint16_t fill[MAX_X_SIZE];
	uint16_t i;

	for (i = 0; i != MAX_X_SIZE - 1; i++)
		fill[i] = color;

	for (i = 0; i != MAX_Y_SIZE; i++) {
		lcd_gc9a01a_set_window(0, i, MAX_X_SIZE - 1, i);
		lcd_gc9a01a_write(fill, sizeof(fill));
	}
}
