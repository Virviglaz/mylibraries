#ifndef LCD_HD_H
#define LCD_HD_H

#include <stdint.h>

typedef enum
{
	LCD_NORMAL,
	LCD_OLED
}LCD_TypeDef;

typedef enum
{
	ENGLISH_JAPANESE_FONT,
	WESTERN_EUROPEAN_FONT,
	ENGLISH_RUSSIAN_FONT,
	WESTERN_EUROPEAN2_FONT,
}CharacterFontTableTypeDef;

/* Init structure */
typedef struct
{
	/* Functions */
	void (*WriteData)(uint8_t data);
	void (*Delay_Func)(uint16_t us);
	
	/* Variables */
	uint8_t BackLightOn;
	uint8_t BackLightPin;
	
	uint8_t EXT_IO_State;
	uint8_t EXT_IO_Pin;
	
	/* Internal use */
	uint8_t LatchData;

	/* Choose LCD type here */
	LCD_TypeDef LCD_Type;
	uint8_t StringSize;
	CharacterFontTableTypeDef Font;
}LCD_SPI_StructTypeDef;

typedef enum
{
	LCD_CMD_NOP																		= 0x00,
	LCD_CMD_Clear                                 = 0x01,
	LCD_CMD_Home																	= 0x02,
	LCD_CMD_8b																		=	0x03,
	LCD_CMD_AutoIncrementOn												= 0x06,
	LCD_CMD_Blank																	= 0x08,
	LCD_CMD_CursorLeft														= 0x10,
	LCD_CMD_CursorRight														= 0x14,
	LCD_CMD_PowerON																= 0x17,
	LCD_CMD_4b_1line															= 0x20,
	LCD_CMD_4b_2lines															= 0x28,
	LCD_CMD_8b_1line															= 0x30,
	LCD_CMD_8b_2lines															=	0x38,
	LCD_CMD_ScrollRight														= 0x1E,
	LCD_CMD_ScrollLeft														= 0x18,
	LCD_CMD_UnderlineCursorON											= 0x0E,
	LCD_CMD_BlockCursorON													= 0x0F,
	LCD_CMD_CursorOFF															= 0x0C,
	LCD_RAW_DATA																	= 0xFF,
}LCD_CMD_TypeDef;

typedef struct 
{
	void (* Init)			(LCD_SPI_StructTypeDef * LCD_SPI_Struct);
	void (* SendCMD)	(LCD_SPI_StructTypeDef * LCD_SPI_Struct, LCD_CMD_TypeDef CMD);
	void (* Print)		(LCD_SPI_StructTypeDef * LCD_SPI_Struct, char * string);
}HD44780_SPI_ClassTypeDef;

void LCD_Init (LCD_SPI_StructTypeDef * LCD_SPI_Struct);
void HD44780_SPI_Cmd(LCD_SPI_StructTypeDef * LCD_SPI_Struct, LCD_CMD_TypeDef CMD);
void HD44780_SPI_PutChar (LCD_SPI_StructTypeDef * LCD_SPI_Struct, uint8_t dat);
void HD44780_SPI_Print (LCD_SPI_StructTypeDef * LCD_SPI_Struct, char * string);
void HD44780_SPI_Set_Pos (LCD_SPI_StructTypeDef * LCD_SPI_Struct, uint8_t row, uint8_t colum);

static const HD44780_SPI_ClassTypeDef HD44780_SPI = {LCD_Init, HD44780_SPI_Cmd, HD44780_SPI_Print};
#endif
