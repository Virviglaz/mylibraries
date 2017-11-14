#ifndef DS18B20_H
#define DS18B20_H

#include "CRC.h"
#include <stdint.h>

/* DS18B20 Commands list */
#define DS18B20_READ_ROM									0x33
#define DS18B20_SKIP_ROM									0xCC
#define DS18B20_SEARCH_ROM								0xF0
#define DS18B20_MATCH_ROM									0x55

#define DS18B20_CONVERT_T_CMD							0x44
#define DS18B20_WRITE_STRATCHPAD_CMD			0x4E
#define DS18B20_READ_STRATCHPAD_CMD				0xBE
#define DS18B20_COPY_STRATCHPAD_CMD				0x48
#define DS18B20_RECALL_E_CMD							0xB8
#define DS18B20_READ_POWER_SUPPLY_CMD			0xB4

#define DS18B20_STRATCHPAD_SIZE						0x09
#define DefaultResolution									Res_12bit

typedef enum
{
	Res_9bit  = 0,
	Res_10bit = 1,
	Res_11bit = 2,
	Res_12bit = 3
}DS18B20_ResolutionTypeDef;

typedef struct
{
	char (*ResetFunc) (void);
	char (*WriteByte) (char value);
	char (*ReadByte) (void);
}One_Wire_InterfaceTypeDef;

typedef struct
{
	One_Wire_InterfaceTypeDef * One_Wire_Interface;
	unsigned char SN[8];
	signed int Temp16;
	signed char Th;
	signed char Tl;
	DS18B20_ResolutionTypeDef Resolution;
}DS18B20_TypeDef;

typedef struct
{
	One_Wire_InterfaceTypeDef * One_Wire_Interface;
	float Temp;
}DS18B20_single_TypeDef;

char DS18B20_Configure (DS18B20_TypeDef * DS18B20);
char DS18B20_Start_Conversion (DS18B20_TypeDef * DS18B20);
char DS18B20_Get_Conversion_Result (DS18B20_TypeDef * DS18B20);
char DS18B20_Start_Conversion_Skip_Rom (DS18B20_single_TypeDef * DS18B20);
char DS18B20_Read_Skip_Rom (DS18B20_single_TypeDef * DS18B20);
float DS18B20_ConvertTemp (DS18B20_TypeDef * DS18B20);
unsigned int DS18B20_GetConversionDelayValue (DS18B20_TypeDef * DS18B20);

#endif
