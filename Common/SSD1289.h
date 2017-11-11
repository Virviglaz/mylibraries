#ifndef SSD1289_H
#define SSD1289_H

#include <stdint.h>

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xAD55
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

typedef struct
{
	void (*WriteReg) (uint8_t LCD_Reg, uint16_t LCD_RegValue);
	uint16_t (*ReadReg) (uint8_t LCD_Reg);
	void (*DelayFunc) (uint16_t ms);
	void (*RamPrepare) (void);
	void (*WriteRam) (uint16_t RGB);
	uint16_t * Fontbuffer;
}SSD1289_StructTypeDef;

void SSD1289_Init (SSD1289_StructTypeDef * SSD1289_InitStruct);
void SSD1289_SetCursor(uint16_t Xpos, uint16_t Ypos);
void SSD1289_SetWindows (uint16_t xStart, uint16_t yStart, uint16_t xLong, uint16_t yLong);
void SSD1289_Clear (uint16_t Color);
void SSD1289_FillWindow (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint16_t Color);
void SSD1289_DrawPic (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint16_t * pic);
uint16_t SSD1289_PrintText (uint16_t StartX, uint16_t StartY, uint8_t FontX, uint8_t FontY, char * font, char * buf, uint16_t Color, uint16_t BackColor);
#endif
