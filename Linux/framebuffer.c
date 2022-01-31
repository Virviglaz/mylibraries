#include "framebuffer.h"
#include <string.h>
#include <stdlib.h>

struct fb_var_screeninfo vinfo; //can be used as public

static int framebuffer = 0;
static uint32_t screenSize;
static uint32_t * imagebuffer;
static uint32_t * bg_buffer = 0;
static Font_StructTypeDef * font = 0;

static struct
{
	bool windowInit;
	uint32_t * ptr;
	uint16_t x, x_max;
} window = { .windowInit = false };


const char * FrameBufferInit (const char * io, uint8_t multiBuffer)
{
	if (io == NULL) io = "/dev/fb0"; //default to main screen
	
	if (framebuffer)
		return "Already initialized!";

	if (bg_buffer) {
		free(bg_buffer);
		bg_buffer = 0;
	}
				
	framebuffer = open(io, O_RDWR);
	
	if (framebuffer < 0)
		return "Fail to open stream!";
	
	if (ioctl(framebuffer, FBIOGET_VSCREENINFO, &vinfo) < 0)
			return "Fail get info structure!";

	screenSize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	bg_buffer = malloc(screenSize);

	if (multiBuffer && multiBuffer <= 4)
	{
		vinfo.xres_virtual = vinfo.xres;
		vinfo.yres_virtual = vinfo.yres * multiBuffer;
		if (ioctl(framebuffer, FBIOPUT_VSCREENINFO, &vinfo) < 0)
			return "Error set resolution";
	}

	imagebuffer = (void *)mmap(0, screenSize, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer, 0);

	return "OK";
}

void FrameBufferUpdate(void)
{
	memcpy(imagebuffer, bg_buffer, screenSize);
}

void FrameBufferDeInit (void)
{
	munmap(bg_buffer, screenSize);
	close(framebuffer);
	free(bg_buffer);
	bg_buffer = 0;
}

void ClearScreen (uint32_t color)
{
	for (uint32_t i = 0; i < (vinfo.xres * vinfo.yres); i++)
		*(bg_buffer + i) = color;
}

void SetWindow (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y)
{
	window.windowInit = true;
	window.ptr = bg_buffer + (x0 + vinfo.xres * y0);
	window.x_max = size_x;
}

void FillWindow (uint32_t color)
{
	if (window.windowInit == false) return;

	*window.ptr++ = color;
	window.x++;

	if (window.x < window.x_max)
		return;

	window.x = 0;
	window.ptr += vinfo.xres - window.x_max;
}

void FlushWindow (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y, uint32_t * color)
{
	SetWindow(x0, y0, size_x, size_y);
	for (uint32_t i = 0; i < size_x * size_y; i++)
		FillWindow(color[i]);
}

void DrawPic32 (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y, uint32_t * pic)
{
	for (uint16_t y = 0; y < size_y; y++)
		for (uint16_t x = 0; x < size_x; x++)
			*(bg_buffer + (x + x0 + vinfo.xres * (y0 + y))) = *pic++;
}

void DrawPixel32 (uint16_t x0, uint16_t y0, uint32_t color)
{
	*(bg_buffer + (x0 + vinfo.xres * y0 )) = color;
}

void DrawHorizontalLine32 (uint16_t x0, uint16_t y0, uint16_t x1, uint32_t color)
{
	uint16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
	SetWindow(x0, y0, dx, 1);
	while(dx--)
			FillWindow(color);
}

void DrawVerticalLine32 (uint16_t x0, uint16_t y0, uint16_t y1, uint32_t color)
{
	uint16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
	SetWindow(x0, y0, 1, dy);
	while(dy--)
		FillWindow(color);
}

void DrawCircle32(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
    int16_t x = -r, y = 0, err = 2 - 2 * r, e2;
    do {
    	DrawPixel32(x0 - x, y0 + y, color);
    	DrawPixel32(x0 + x, y0 + y, color);
    	DrawPixel32(x0 + x, y0 - y, color);
    	DrawPixel32(x0 - x, y0 - y, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y * 2 + 1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x * 2 + 1;
    } while (x <= 0);
}

void DrawFilledCircle32 (int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
    int16_t x = -r, y = 0, err = 2 - 2 * r, e2;
    do {
    	DrawHorizontalLine32(x0 + x, y0 + y, x0 - x, color);
    	DrawHorizontalLine32(x0 + x, y0 - y, x0 - x, color);
        e2 = err;
        if (e2 <= y) {
            err += ++y * 2 + 1;
            if (-x == y && e2 <= x) e2 = 0;
        }
        if (e2 > x) err += ++x * 2 + 1;
    } while (x <= 0);
}

void GetScreenSize (uint16_t * width, uint16_t * height)
{
	*width = vinfo.xres;
	*height = vinfo.yres;
}

void DrawCross (uint16_t x0, uint16_t y0, uint16_t size, uint32_t color)
{
	DrawHorizontalLine32(x0 - size, y0, x0 + size, color);
	DrawVerticalLine32(x0, y0 - size, y0 + size, color);
}

Font_StructTypeDef * SetFont (Font_StructTypeDef * _font)
{
	if (_font) font = _font;
	return font;
}

static void PrintChar (uint16_t x0, uint16_t y0, char ch)
{
	SetWindow(x0, y0, font->FontXsize, font->FontYsize);
	for (uint8_t x = 0; x != font->FontXsize; x++)
		for (uint8_t y = 0; y != font->FontYsize; y++)
			FillWindow ((font->Font[ch * font->FontXsize + x] & (1 << y)) ? font->FontColor : font->BackColor);
}

uint16_t PrintText (uint16_t x0, uint16_t y0, const char * text)
{
	uint16_t x = x0, Row = 0;
	while(* text)
	{
		if (* text == '\n')
		{
			x0 = x;
			y0 += font->FontYsize;
			Row += 1;
		}
		else
		{
			PrintChar(x0, y0, * text);
			x0 += font->FontXsize;
		}
		text++;
	}
	return Row;
}

