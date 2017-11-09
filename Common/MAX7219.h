#ifndef MAX7129_H
#define MAX7219_H

#include <stdio.h>

#define MAX7219_NODECODE	0x00
#define MAX7219_ALLDECODE	0xFF

typedef enum
{
	MAX7219_Power_ShutDownMode,
	MAX7219_Power_NormalOperation
}MAX7219_PowerEnumTypeDef;

typedef enum
{
	MAX7219_Test_NormalOperation,
	MAX7219_Test_DisplayTest
}MAX7219_TestModeEnumTypeDef;

#define	MAX7219_REG_Nop	        0x00
#define	MAX7219_REG_Dig0	0x01
#define	MAX7219_REG_Dig1	0x02
#define	MAX7219_REG_Dig2	0x03
#define	MAX7219_REG_Dig3	0x04
#define	MAX7219_REG_Dig4	0x05
#define	MAX7219_REG_Dig5	0x06
#define	MAX7219_REG_Dig6	0x07
#define	MAX7219_REG_Dig7	0x08
#define	MAX7219_REG_DecodeMode	0x09
#define	MAX7219_REG_Intensity	0x0A
#define	MAX7219_REG_ScanLim	0x0B
#define	MAX7219_REG_ShutDown	0x0C
#define	MAX7219_REG_TestMode	0x0F

typedef struct
{
	MAX7219_PowerEnumTypeDef Power;
	MAX7219_TestModeEnumTypeDef TestMode;
        uint8_t DecodeMode;
        uint8_t Intensity;
        uint8_t ScanLimit;
	uint8_t DeviceN;
        uint8_t DigitsPerChip;
        uint8_t xMirror;
        
	uint8_t (*SendReceive)(uint8_t value);
        void (*ChipEnable) (uint8_t value);
}MAX7219_InitStructTypeDef;

uint8_t MAX7219_Init (MAX7219_InitStructTypeDef * MAX7219_InitStruct);
void MAX7219_Print (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t * buf, uint8_t * font);
void MAX7219_UpdateDigit (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t DigNum, uint8_t * buf);
void MAX7219_DrawWindow (MAX7219_InitStructTypeDef * MAX7219_InitStruct, uint8_t * buf, uint16_t size);
void MAX7219_StructInitForDecode (MAX7219_InitStructTypeDef * MAX7219_InitStruct);
void MAX7219_StructInitNoDecode (MAX7219_InitStructTypeDef * MAX7219_InitStruct);

#endif
