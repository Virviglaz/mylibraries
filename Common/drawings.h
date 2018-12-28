#ifndef DRAWINGS_H

#include <stdint.h>

#ifndef COLOR_DEPTH
	#define COLOR_DEPTH uint32_t
#endif // !COLOR_DEPTH

#ifndef FONT_SIZE_T
	#define FONT_SIZE_T uint8_t
#endif // !FONT_SIZE_T

typedef struct
{
	void(*pixel) (uint16_t x, uint16_t y, COLOR_DEPTH color);
	void (*line) (int16_t x0, int16_t y0, int16_t x1, int16_t y1, COLOR_DEPTH color);
	void(*hline) (uint16_t x0, uint16_t y0, uint16_t x1, COLOR_DEPTH color);
	void(*vline) (uint16_t x0, uint16_t y0, uint16_t y1, COLOR_DEPTH color);
	void(*circle)(int16_t x, int16_t y, int16_t r, COLOR_DEPTH color);
	void(*filled_circle)(int16_t x, int16_t y, int16_t r, COLOR_DEPTH color);
	void (*cross)(uint16_t x0, uint16_t y0, uint16_t size, COLOR_DEPTH color);
	uint16_t(*print)(uint16_t x0, uint16_t y0, const char * text);
	uint16_t (*print_adv)(uint16_t x0, uint16_t y0, const char * text);
}drawings_t;

typedef struct
{
	FONT_SIZE_T FontXsize, FontYsize;
	const uint8_t * Font;
	COLOR_DEPTH FontColor, BackColor;
}font_t;

typedef struct
{
	uint16_t FontXsize;
	uint16_t Offset;
}char_desc_t;

typedef struct
{
	FONT_SIZE_T FontYsize;
	uint8_t offset, space;
	const uint16_t * char_desc;
	const uint8_t * Font;
	COLOR_DEPTH FontColor, BackColor;
}adv_font_t;

drawings_t * drawings_init(void(*set_window_func) (uint16_t, uint16_t, uint16_t, uint16_t),
	void(*fill_window_func) (COLOR_DEPTH), font_t * font, adv_font_t * adv_font);

#endif // !DRAWINGS_H
