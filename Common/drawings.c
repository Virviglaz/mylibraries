#include "drawings.h"

/* Interface functions */
static void (*set_window) (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y);
static void (*fill_window) (COLOR_DEPTH color);
static font_t *_font;
static adv_font_t * _adv_font;

/* Static functions */
static void pixel(uint16_t x, uint16_t y, COLOR_DEPTH color);
static void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, COLOR_DEPTH color);
static void hline(uint16_t x0, uint16_t y0, uint16_t x1, COLOR_DEPTH color);
static void vline(uint16_t x0, uint16_t y0, uint16_t y1, COLOR_DEPTH color);
static void circle(int16_t x, int16_t y, int16_t r, COLOR_DEPTH color);
static void filled_circle(int16_t x, int16_t y, int16_t r, COLOR_DEPTH color);
static void cross(uint16_t x0, uint16_t y0, uint16_t size, COLOR_DEPTH color);
static uint16_t print(uint16_t x0, uint16_t y0, const char * text);
static uint16_t print_adv(uint16_t x0, uint16_t y0, const char * text);

/* Init drawing struct */
drawings_t * drawings_init(void(*set_window_func) (uint16_t, uint16_t, uint16_t, uint16_t),
	void(*fill_window_func) (COLOR_DEPTH), font_t * font, adv_font_t * adv_font)
{
	static const drawings_t _drawings = {
		.pixel = pixel,
		.line = line,
		.hline = hline,
		.vline = vline,
		.circle = circle,
		.filled_circle = filled_circle,
		.cross = cross,
		.print = print,
		.print_adv = print_adv,
	};

	set_window = set_window_func;
	fill_window = fill_window_func;
	_font = font;
	_adv_font = adv_font;

	return (void*)&_drawings;
}

static void pixel(uint16_t x, uint16_t y, COLOR_DEPTH color)
{
	set_window(x, y, 1, 1);
	fill_window(color);
}

static void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, COLOR_DEPTH color)
{
	int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int16_t err = (dx > dy ? dx : -dy) / 2;

	while (pixel(x0, y0, color), x0 != x1 || y0 != y1)
	{
		int16_t e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

static void hline(uint16_t x0, uint16_t y0, uint16_t x1, COLOR_DEPTH color)
{
	uint16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
	set_window(x0, y0, dx, 1);
	while (dx--)
		fill_window(color);
}

static void vline(uint16_t x0, uint16_t y0, uint16_t y1, COLOR_DEPTH color)
{
	uint16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
	set_window(x0, y0, 1, dy);
	while (dy--)
		fill_window(color);
}

static void circle(int16_t x, int16_t y, int16_t r, COLOR_DEPTH color)
{
	int16_t dx = -r, dy = 0, err = 2 - 2 * r, e2;
	do {
		pixel(x - dx, y + dy, color);
		pixel(x + dx, y + dy, color);
		pixel(x + dx, y - dy, color);
		pixel(x - dx, y - dy, color);
		e2 = err;
		if (e2 <= dy) {
			err += ++dy * 2 + 1;
			if (-dx == dy && e2 <= dx) e2 = 0;
		}
		if (e2 > dx) err += ++dx * 2 + 1;
	} while (dx <= 0);
}

static void filled_circle(int16_t x, int16_t y, int16_t r, COLOR_DEPTH color)
{
	int16_t dx = -r, dy = 0, err = 2 - 2 * r, e2;
	do {
		DrawHorizontalLine32(x + dx, y + dy, x - dx, color);
		DrawHorizontalLine32(x + dx, y - dy, x - dx, color);
		e2 = err;
		if (e2 <= dy) {
			err += ++dy * 2 + 1;
			if (-dx == dy && e2 <= dx) e2 = 0;
		}
		if (e2 > dx) err += ++dx * 2 + 1;
	} while (dx <= 0);
}

static void cross(uint16_t x0, uint16_t y0, uint16_t size, COLOR_DEPTH color)
{
	hline(x0 - size, y0, x0 + size, color);
	vline(x0, y0 - size, y0 + size, color);
}

static void print_char(uint16_t x0, uint16_t y0, char ch)
{
	SetWindow(x0, y0, _font->FontXsize, _font->FontYsize);
	for (FONT_SIZE_T x = 0; x != _font->FontXsize; x++)
		for (FONT_SIZE_T y = 0; y != _font->FontYsize; y++)
			FillWindow((_font->Font[ch * _font->FontXsize + x] & (1 << y)) ? _font->FontColor : _font->BackColor);
}

static uint16_t print(uint16_t x0, uint16_t y0, const char * text)
{
	uint16_t x = x0, Row = 0;
	while (*text)
	{
		if (*text == '\n')
		{
			x0 = x;
			y0 += _font->FontYsize;
			Row += 1;
		}
		else
		{
			print_char(x0, y0, *text);
			x0 += _font->FontXsize;
		}
		text++;
	}
	return Row;
}

static void print_adv_char(uint16_t x0, uint16_t y0, char ch, uint8_t width, uint16_t offset)
{
	SetWindow(x0, y0, width * 8, _adv_font->FontYsize);
	for (FONT_SIZE_T y = 0; y < _adv_font->FontYsize; y++)
	{
		uint8_t w = width, i = 0;
		while(w--)
		{
			uint8_t c = _adv_font->Font[offset + width * y + i++];
			for (uint8_t x = 0x80; x > 0; x >>= 1)
				FillWindow((c & x) ? _adv_font->FontColor : _adv_font->BackColor);
		}
	}
}

static uint16_t print_adv(uint16_t x0, uint16_t y0, const char * text)
{
	uint16_t x = x0, Row = 0;
	while (*text)
	{
		if (*text == '\n')
		{
			x0 = x;
			y0 += _adv_font->FontYsize;
			Row += 1;
		}
		else
		{
			if (*text > _adv_font->offset)
			{
				char_desc_t * desc = _adv_font->char_desc + (*text - _adv_font->offset) * sizeof(uint16_t);
				print_adv_char(x0, y0, *text, desc->FontXsize / 8 + (desc->FontXsize % 8 ? 1 : 0), desc->Offset);
				x0 += desc->FontXsize;
			}
			else
				x0 += _adv_font->space;
		}
		text++;
	}
	return Row;
}