#include "SSD1289.h"

#define XSize 240
#define YSize 320

SSD1289_StructTypeDef * SSD1289;

typedef union
{
	uint32_t RawData;
	uint16_t RegData[2];
}RegDataTypeDef;

const RegDataTypeDef GLCD_InitSequence[] = {
		0x00000001, 0x0003A8A4, 0x000C0000, 0x000D080C, 0x000E2B00, 0x001E00B0, 0x00012B3F,
		0x00020600, 0x00100000, 0x00116070, 0x00050000, 0x00060000, 0x0016EF1C, 0x00170003,
		0x00070133, 0x000B0000, 0x000F0000, 0x00410000, 0x00420000, 0x00480000, 0x0049013F,
		0x004A0000, 0x004B0000, 0x0044EF00, 0x00450000, 0x0046013F, 0x00300707, 0x00310204,
		0x00320204, 0x00330502, 0x00340507, 0x00350204, 0x00360204, 0x00370502, 0x003A0302,
		0x003B0302, 0x00230000, 0x00240000, 0x00258000, 0x004f0000, 0x004e0000};
		
void SSD1289_Init (SSD1289_StructTypeDef * SSD1289_InitStruct)
{
	uint8_t cnt;
	
	/* Assign driver to local structure */
	SSD1289 = SSD1289_InitStruct;

	SSD1289_InitStruct->DelayFunc(50);
	for (cnt = 0; cnt < sizeof(GLCD_InitSequence) / sizeof(uint32_t); cnt++)
	{		
		SSD1289->WriteReg(GLCD_InitSequence[cnt].RegData[1], GLCD_InitSequence[cnt].RegData[0]);
		SSD1289->DelayFunc(10);	
	}	
}

void SSD1289_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
   SSD1289->WriteReg(0x004e, Xpos); /* Row */
   SSD1289->WriteReg(0x004f, Ypos); /* Line */ 
}

void SSD1289_SetWindows (uint16_t xStart, uint16_t yStart, uint16_t xLong, uint16_t yLong)
{ 
	SSD1289->WriteReg(0x0044, xStart | ((xStart + --xLong) << 8));
	SSD1289->WriteReg(0x0045, yStart);
	SSD1289->WriteReg(0x0046, yStart + --yLong);
	SSD1289_SetCursor(xStart, yStart);
}

void SSD1289_Clear (uint16_t Color)
{
	SSD1289_FillWindow(0, 0, XSize, YSize, Color);
}

void SSD1289_FillWindow (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint16_t Color)
{
	uint32_t index = SizeX * SizeY;
	SSD1289_SetWindows(StartX, StartY, SizeX, SizeY);
	SSD1289->RamPrepare(); /* Prepare to write GRAM */
	while (index--) SSD1289->WriteRam(Color);
}

void SSD1289_DrawPic (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint16_t * pic)
{
	uint32_t index = SizeX * SizeY;
	SSD1289_SetWindows(StartX, StartY, SizeX, SizeY);
	SSD1289->RamPrepare(); /* Prepare to write GRAM */
	while (index--) SSD1289->WriteRam(* pic++);
}

void SSD1289_PrintChar (uint16_t StartX, uint16_t StartY, uint8_t FontX, uint8_t FontY, char * font, char buf, uint16_t Color, uint16_t BackColor)
{
	uint16_t * fontbuf = SSD1289->Fontbuffer;
	uint8_t x, y;
	for (x = 0; x != FontX; x++)
		for (y = 0; y != FontY; y++)
			* fontbuf++ = (font[buf * FontX + x] & (1 << y)) ? Color : BackColor;

	SSD1289_DrawPic(StartX, StartY, FontX, FontY, SSD1289->Fontbuffer);
}

uint16_t SSD1289_PrintText (uint16_t StartX, uint16_t StartY, uint8_t FontX, uint8_t FontY, char * font, char * buf, uint16_t Color, uint16_t BackColor)
{
	uint16_t X = StartX, Row = 0;
	while(* buf)
	{
		if (* buf == '\n')
		{	
			StartX = X;
			StartY += FontY;
			Row += 1;
		}
		else
		{
			SSD1289_PrintChar(StartX, StartY, FontX, FontY, font, * buf, Color, BackColor);
			StartX += FontX;			
		}
		buf++;
	}
	return Row;
}
