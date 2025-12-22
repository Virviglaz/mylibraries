#include "SSD1289.h"

#define XSIZE 240
#define YSIZE 320

static struct ssd1289_t *lcd;

/* Private functions */
static void set_cursor(uint16_t x, uint16_t y)
{
	lcd->wr_reg(0x004e, x); /* Row */
	lcd->wr_reg(0x004f, y); /* Line */
}

static void set_window(uint16_t x, uint16_t y,
	uint16_t dx, uint16_t dy)
{
	lcd->wr_reg(0x0044, x | ((x + --dx) << 8));
	lcd->wr_reg(0x0045, y);
	lcd->wr_reg(0x0046, y + --dy);
	set_cursor(x, y);
}

static void fill_window(uint16_t x, uint16_t y, uint16_t dx,
	uint16_t dy, uint16_t c)
{
	uint32_t index = dx * dy;

	set_window(x, y, dx, dy);
	lcd->ram_prepare();
	while (index--)
		lcd->wr_ram(c);
}

/* Public functions */
uint16_t rgb2col(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t) ( \
		((b >> 3) & 0x1f) | \
		(((g >> 2) & 0x3f) << 5) | \
		(((r >> 3) & 0x1f) << 11));
}


struct ssd1289_t *ssd1289_init(struct ssd1289_t *dev)
{
	const uint32_t init_seq_raw[] = {
		0x00000001, 0x0003A8A4, 0x000C0000,
		0x000D080C, 0x000E2B00, 0x001E00B0,
		0x00012B3F, 0x00020600, 0x00100000,
		0x00116070, 0x00050000, 0x00060000,
		0x0016EF1C, 0x00170003, 0x00070133,
		0x000B0000, 0x000F0000, 0x00410000,
		0x00420000, 0x00480000, 0x0049013F,
		0x004A0000, 0x004B0000, 0x0044EF00,
		0x00450000, 0x0046013F, 0x00300707,
		0x00310204, 0x00320204, 0x00330502,
		0x00340507, 0x00350204, 0x00360204,
		0x00370502, 0x003A0302, 0x003B0302,
		0x00230000, 0x00240000, 0x00258000,
		0x004f0000, 0x004e0000
	};

	const uint8_t size =
		sizeof(init_seq_raw) / sizeof(init_seq_raw[0]);
	uint8_t i;

	/* Assign driver to local structure */
	if (dev)
		lcd = dev;
	else
		return lcd;

	for (i = 0; i < size; i++) {
		union {
			uint32_t raw_data;
			uint16_t reg_data_w[2];
		} init_seq;

		init_seq.raw_data = init_seq_raw[i];

		lcd->wr_reg(init_seq.reg_data_w[1],
			init_seq.reg_data_w[0]);
	}

	lcd->wr_reg(1, 0x293F | \
		(lcd->x_mirror ? 0x4000 : 0) | \
			(lcd->y_mirror ? 0x200 : 0));

	return lcd;
}

void ssd1289_clear(void)
{
	fill_window(0, 0, XSIZE, YSIZE, lcd->bc);
}

void ssd1289_pic(uint16_t x, uint16_t y, uint16_t dx,
	uint16_t dy, uint16_t *pic)
{
	uint32_t index = dx * dy;

	set_window(x, y, dx, dy);
	lcd->ram_prepare();
	while (index--)
		lcd->wr_ram(*pic++);
}

void ssd1289_putchar(uint16_t x, uint16_t y, char c)
{
	uint8_t ix, iy;

	set_window(x, y, lcd->font->x_size,
		lcd->font->y_size);

	lcd->ram_prepare();

	for (ix = 0; ix != lcd->font->x_size; ix++)
		for (iy = 0; iy != lcd->font->y_size; iy++)
			lcd->wr_ram((lcd->font->data[c * \
				lcd->font->x_size + ix] & \
					(1 << y)) ? \
						lcd->c : lcd->bc);
}

uint16_t ssd1289_print(uint16_t x, uint16_t y, char *t)
{
	uint16_t ix = x, Row = 0;
	while(*t) {
		if (*t == '\n') {
			x = ix;
			y += lcd->font->y_size;
			Row += 1;
		} else {
			ssd1289_putchar(x, y, *t);
			x += lcd->font->x_size;
		}
		t++;
	}

	return Row;
}
