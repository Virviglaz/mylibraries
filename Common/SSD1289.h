#ifndef SSD1289_H
#define SSD1289_H

#include <stdint.h>
#include <stdbool.h>

/* LCD color */
#define White		0xFFFF
#define Black		0x0000
#define Grey		0xAD55
#define Blue		0x001F
#define Blue2		0x051F
#define Red		0xF800
#define Magenta	0xF81F
#define Green		0x07E0
#define Cyan		0x7FFF
#define Yellow		0xFFE0

struct font_t {
	uint8_t x_size, y_size;
	const uint8_t *data;
};

struct ssd1289_t {
	void (*wr_reg) (uint8_t reg, uint16_t value);
	uint16_t (*rd_reg) (uint8_t reg);
	void (*ram_prepare) (void);
	void (*wr_ram) (uint16_t c);
	uint16_t (*rd_ram)(void);
	bool x_mirror, y_mirror;
	struct font_t * font;
	uint16_t c, bc; /* color/backcolor */
};

uint16_t rgb2col(uint8_t r, uint8_t g, uint8_t b);
struct ssd1289_t *ssd1289_init(struct ssd1289_t *dev);
void ssd1289_clear(void);
void ssd1289_pic(uint16_t x, uint16_t y, uint16_t dx,
	uint16_t dy, uint16_t *pic);
void ssd1289_putchar(uint16_t x, uint16_t y, char c);
uint16_t ssd1289_print(uint16_t x, uint16_t y, char *t);

#endif /* SSD1289_H */
