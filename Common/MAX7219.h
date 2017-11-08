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

typedef enum
{
	MAX7219_REG_Nop = 0x00,
	MAX7219_REG_Dig0 = 0x01,
	MAX7219_REG_Dig1 = 0x02,
	MAX7219_REG_Dig2 = 0x03,
	MAX7219_REG_Dig3 = 0x04,
	MAX7219_REG_Dig4 = 0x05,
	MAX7219_REG_Dig5 = 0x06,
	MAX7219_REG_Dig6 = 0x07,
	MAX7219_REG_Dig7 = 0x08,
	MAX7219_REG_DecodeMode = 0x09,
	MAX7219_REG_Intensity = 0x0A,
	MAX7219_REG_ScanLim = 0x0B,
	MAX7219_REG_ShutDown = 0x0C,
	MAX7219_REG_TestMode = 0x0F
}MAX7219_RegistersEnumTypeDef;

typedef struct
{
	MAX7219_PowerEnumTypeDef Power;
	MAX7219_TestModeEnumTypeDef TestMode;
	uint8_t (*SendReceive) (uint8_t * Data, uint8_t size);
	uint8_t (*SendByte)(uint8_t value);
	uint8_t DeviceN;
}MAX7219_InitStructTypeDef;

typedef struct
{
	uint8_t * Data;
	uint8_t Size;
	MAX7219_InitStructTypeDef * MAX7219_InitStruct;
}MAX7219_StructTypeDef;

uint8_t MAX7219_Init (MAX7219_InitStructTypeDef * MAX7219_InitStruct);

#endif
