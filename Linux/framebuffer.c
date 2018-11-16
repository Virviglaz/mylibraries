#include "framebuffer.h"
#include <string.h>

struct fb_var_screeninfo vinfo; //can be used as public

static int framebuffer = 0;
static uint32_t screenSize;
static uint32_t * imagebuffer;

static struct
{
	bool windowInit;
	uint32_t * ptr;
	uint16_t x, x_max;
} window = { .windowInit = false };


const char * frameBufferInit (const char * io)
{
	if (io == NULL) io = "/dev/fb0"; //default to main screen
	
	if (framebuffer)
		return "Already initialized!";
				
	framebuffer = open(io, O_RDWR);
	
	if (framebuffer < 0)
		return "Fail to open stream!";
	
	if (ioctl(framebuffer, FBIOGET_VSCREENINFO, &vinfo) < 0)
			return "Fail get info structure!";

	screenSize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	imagebuffer = (void *)mmap(0, screenSize, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer, 0);

	return "OK";
}

void frameBufferDeInit (void)
{
	munmap(imagebuffer, screenSize);
	close(framebuffer);
}

void ClearScreen (uint32_t Color)
{
	for (uint32_t i = 0; i < (vinfo.xres * vinfo.yres); i++)
		*(imagebuffer + i) = Color;
}

void SetWindow (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY)
{
	window.windowInit = true;
	window.ptr = imagebuffer + (StartX + vinfo.xres * StartY);
	window.x_max = SizeX;
}

void FillWindow (uint32_t Color)
{
	if (window.windowInit == false) return;

	*window.ptr++ = Color;
	window.x++;

	if (window.x < window.x_max)
		return;

	window.x = 0;
	window.ptr += vinfo.xres - window.x_max;
}

void FlushWindow (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint32_t * color)
{
	SetWindow(StartX, StartY, SizeX, SizeY);
	for (uint32_t i = 0; i < SizeX * SizeY; i++)
		FillWindow(color[i]);
}

void DrawPic32 (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint32_t * Pic)
{
	for (uint16_t y = 0; y < SizeY; y++)
		for (uint16_t x = 0; x < SizeX; x++)
			*(imagebuffer + (x + StartX + vinfo.xres * (StartY + y))) = *Pic++;
}

void DrawPixel32 (uint16_t StartX, uint16_t StartY, uint32_t Color)
{
	*(imagebuffer + (StartX + vinfo.xres * StartY )) = Color;
}

void DrawHorizontalLine32 (uint16_t StartX, uint16_t StartY, uint16_t Len, uint32_t Color)
{
	uint32_t * ptr = imagebuffer + (StartX + vinfo.xres * StartY);
	while(Len--)
		*ptr++ = Color;
}

void DrawFilledCircle32 (uint16_t StartX, uint16_t StartY, uint16_t R, uint32_t Color)
{
	int16_t x1, y1, yk = 0, sigma, delta, f;
	x1 = 0;
	y1 = R;
	delta = 2 * (1 - R);
	do
	{
		DrawHorizontalLine32(StartX - x1, StartY + y1, x1 * 2, Color);
		DrawHorizontalLine32(StartX - x1, StartY - y1, x1 * 2, Color);
		f = 0;
		if (y1 < yk) break;
		if (delta < 0)
		{
		  sigma = 2 * (delta + y1) - 1;
		  if (sigma <= 0)
		  {
			x1++;
			delta += 2 * x1 + 1;
			f = 1;
		  }
		}
		else if (delta > 0)
		{
		  sigma = 2 * (delta - x1) - 1;
		  if (sigma > 0)
		  {
			y1--;
			delta += 1 - 2 * y1;
			f = 1;
		  }
		}
		if(!f)
		{
		  x1++;
		  y1--;
		  delta += 2 * (x1 - y1 - 1);
		}
	} while(1);
}

