#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  /* ioctl */
#include <linux/fb.h>
#include <sys/mman.h>

#define BLACK32		0x000000
#define WHITE32		0xFFFFFF
#define RED32		0xFF0000
#define GREEN32		0x00FF00
#define BLUE32		0x0000FF
#define YELLOW32	0xFFFF00
#define CYAN32		0x00FFFF
#define MAGENTA32	0xFF00FF
#define SILVER32	0xC0C0C0
#define GRAY32		0xF0F0F0
#define MAROON32	0x800000
#define OLIVE32		0x808000
#define DGREEN		0x008000
#define PURPLE32	0x800080
#define TEAL32		0x008080
#define NAVI32		0x000080

typedef struct
{
	uint8_t FontXsize, FontYsize;
	const uint8_t * Font;
	uint32_t FontColor, BackColor;
}Font_StructTypeDef;

const char * FrameBufferInit (const char * io);
void FrameBufferDeInit (void);
void ClearScreen (uint32_t color);
void SetWindow (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y);
void FillWindow (uint32_t color);
void FlushWindow (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y, uint32_t * color);
void DrawPic32 (uint16_t x0, uint16_t y0, uint16_t size_x, uint16_t size_y, uint32_t * pic);
void DrawPixel32 (uint16_t x0, uint16_t y0, uint32_t color);
void DrawHorizontalLine32 (uint16_t x0, uint16_t y0, uint16_t x1, uint32_t color);
void DrawVerticalLine32 (uint16_t x0, uint16_t y0, uint16_t y1, uint32_t color);
void DrawCircle32(int16_t x0, int16_t y0, int16_t r, uint32_t color);
void DrawFilledCircle32 (int16_t x0, int16_t y0, int16_t r, uint32_t color);
void GetScreenSize (uint16_t * width, uint16_t * height);
void DrawCross (uint16_t x0, uint16_t y0, uint16_t size, uint32_t color);
Font_StructTypeDef * SetFont (Font_StructTypeDef * _font);
uint16_t PrintText (uint16_t x0, uint16_t y0, const char * text);
void FrameBufferUpdate (void);
#endif
