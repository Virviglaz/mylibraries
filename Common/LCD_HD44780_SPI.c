#include "LCD_HD44780_SPI.h"

#ifndef LCD_HD44780_SPI_ALT_HW
	/* Standart HW configuration */
	#define LCD_RS_BIT                  (1 << 2)
	#define LCD_E_BIT                   (1 << 3)
	#define LCD_Data_Mask               0xF0
#endif

/* Internal functions declaration */
void HD44780_SPI_Write(LCD_SPI_StructTypeDef * LCD_SPI_Struct, uint8_t data, uint8_t cmd);

/* Public functions */
void LCD_Init (LCD_SPI_StructTypeDef * LCD_SPI_Struct)
{
	HD44780_SPI_Write(LCD_SPI_Struct, 0x03 << 4, 1);
	HD44780_SPI_Write(LCD_SPI_Struct, 0x03 << 4, 1);
	HD44780_SPI_Write(LCD_SPI_Struct, 0x03 << 4, 1);
	HD44780_SPI_Write(LCD_SPI_Struct, 0x02 << 4, 1);  //4 bit wide bus
	LCD_SPI_Struct->Delay_Func(10); //delay 10ms		
	HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_4b_2lines | LCD_SPI_Struct->Font);  //5us delay
	                                                                                                                                                                                                                                                                                                                                      					
	if (LCD_SPI_Struct->LCD_Type == LCD_NORMAL)
	{
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_Home);
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_Clear);
		LCD_SPI_Struct->Delay_Func(10); //delay 10ms	
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_CursorOFF);
	}
	else
	{
		// OLED LCD TYPE		
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_Blank);
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_AutoIncrementOn);
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_PowerON);
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_Clear);
		LCD_SPI_Struct->Delay_Func(10); //delay 10ms	
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_Home);	
		HD44780_SPI_Cmd(LCD_SPI_Struct, LCD_CMD_CursorOFF);	
	}
}


void HD44780_SPI_PutChar (LCD_SPI_StructTypeDef * LCD_SPI_Struct, uint8_t dat)
{
   if (dat == '\n') 
		 HD44780_SPI_Cmd(LCD_SPI_Struct, (LCD_CMD_TypeDef)(0x80 | 0x40)); 
   else 
	 {
		 HD44780_SPI_Write(LCD_SPI_Struct, dat & 0xF0, 0);
		 HD44780_SPI_Write(LCD_SPI_Struct, (dat & 0x0F) << 4, 0);
	 }
}

void HD44780_SPI_Print (LCD_SPI_StructTypeDef * LCD_SPI_Struct, char * string)
{
	uint8_t i = 0;
   while (*string && i < (LCD_SPI_Struct->StringSize * 2 + 1)) 
	 {
		 HD44780_SPI_PutChar(LCD_SPI_Struct, *string++);
		 i++;
	 }
}

void HD44780_SPI_Set_Pos (LCD_SPI_StructTypeDef * LCD_SPI_Struct, uint8_t row, uint8_t colum)
{
        uint8_t position = 0x80;
        if (row > 0) { position |= 0x40;}
        HD44780_SPI_Cmd (LCD_SPI_Struct, (LCD_CMD_TypeDef)(position | colum));
}

/* Internal functions */
void HD44780_SPI_Write(LCD_SPI_StructTypeDef * LCD_SPI_Struct, uint8_t data, uint8_t cmd)
{
	/* Prepare SPI data */
	LCD_SPI_Struct->LatchData = data & LCD_Data_Mask;
	
	if (!cmd)
		LCD_SPI_Struct->LatchData = LCD_RS_BIT | data;
	
	/* Set pin states */
	if (LCD_SPI_Struct->BackLightOn)
		LCD_SPI_Struct->LatchData |= LCD_SPI_Struct->BackLightPin;
	if (LCD_SPI_Struct->EXT_IO_State)
		LCD_SPI_Struct->LatchData |= LCD_SPI_Struct->EXT_IO_Pin;

	/* Send data */
	LCD_SPI_Struct->WriteData(LCD_SPI_Struct->LatchData);	//optional
	
	if (cmd != LCD_RAW_DATA)
	{
		/* Send positive strobe */
		LCD_SPI_Struct->LatchData |= LCD_E_BIT;
		LCD_SPI_Struct->WriteData(LCD_SPI_Struct->LatchData);
		
		/* Send negative strobe */
		LCD_SPI_Struct->LatchData &= ~LCD_E_BIT;
		LCD_SPI_Struct->WriteData(LCD_SPI_Struct->LatchData);
	}
}

void HD44780_SPI_Cmd(LCD_SPI_StructTypeDef * LCD_SPI_Struct, LCD_CMD_TypeDef CMD)
{
        HD44780_SPI_Write(LCD_SPI_Struct, (uint8_t)CMD & 0xF0, 1);
        HD44780_SPI_Write(LCD_SPI_Struct, (((uint8_t)CMD & 0x0F) << 4), 1);
}
