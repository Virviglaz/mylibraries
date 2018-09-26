#ifndef SSD1289_H
#define SSD1289_H

#include <stdint.h>
#include <stdbool.h>

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
	uint8_t FontXsize, FontYsize;
	const uint8_t * Font;
}Font_StructTypeDef;

typedef struct
{
	void (*WriteReg) (uint8_t LCD_Reg, uint16_t LCD_RegValue);
	uint16_t (*ReadReg) (uint8_t LCD_Reg);
	void (*RamPrepare) (void);
	void (*WriteRam) (uint16_t RGB);
	bool Xmirror, Ymirror;
	Font_StructTypeDef * FontStruct;
	uint16_t Color, BackColor;
}SSD1289_StructTypeDef;

SSD1289_StructTypeDef *  SSD1289_Init (SSD1289_StructTypeDef * SSD1289_InitStruct);
void SSD1289_Clear (void);
void SSD1289_DrawPic (uint16_t StartX, uint16_t StartY, uint16_t SizeX, uint16_t SizeY, uint16_t * pic);
void SSD1289_PrintChar (uint16_t StartX, uint16_t StartY, char Ch);
uint16_t SSD1289_PrintText (uint16_t StartX, uint16_t StartY, char * Text);
#endif
