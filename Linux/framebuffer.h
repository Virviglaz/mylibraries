#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  /* ioctl */
#include <linux/fb.h>
#include <sys/mman.h>

const char * frameBufferInit (const char * io);
void frameBufferDeInit (void);
void ClearScreen (uint32_t Color);
void SetWindow (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY);
void FillWindow (uint32_t Color);
void FlushWindow (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint32_t * color);
void DrawPic32 (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint32_t * Pic);
void DrawPixel32 (uint16_t StartX, uint16_t StartY, uint32_t Color);
void DrawHorizontalLine32 (uint16_t StartX, uint16_t StartY, uint16_t Len, uint32_t Color);
void DrawFilledCircle32 (uint16_t StartX, uint16_t StartY, uint16_t R, uint32_t Color);
#endif
