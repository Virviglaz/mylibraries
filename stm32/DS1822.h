#include "1-wire.h"
#include "crc_generic.h"

/* DS1822 Commands list */
#define DS1822_READ_ROM 0x33
#define DS1822_SKIP_ROM 0xCC
#define DS1822_SEARCH_ROM 0xF0
#define DS1822_MATCH_ROM 0x55

#define DS1822_CONVERT_T_CMD 0x44
#define DS1822_WRITE_STRATCHPAD_CMD 0x4E
#define DS1822_READ_STRATCHPAD_CMD 0xBE
#define DS1822_COPY_STRATCHPAD_CMD 0x48
#define DS1822_RECALL_E_CMD 0xB8
#define DS1822_READ_POWER_SUPPLY_CMD 0xB4

#define DS1822_STRATCHPAD_SIZE 0x09
#define DefaultResolution Res_12bit

typedef enum
{
	Res_9bit = 0,
	Res_10bit = 1,
	Res_11bit = 2,
	Res_12bit = 3
} DS1822_ResolutionTypeDef;

typedef struct
{
	One_Wire_StructTypeDef *One_Wire_Bus;
	unsigned char SN[8];
	signed int Temp16;
	signed char Th;
	signed char Tl;
	DS1822_ResolutionTypeDef Resolution;
} DS1822_TypeDef;

typedef struct
{
	One_Wire_StructTypeDef *One_Wire_Bus;
	float Temp;
} DS1822_single_TypeDef;

One_Wire_ErrorTypeDef DS1822_Configure(DS1822_TypeDef *DS1822);
One_Wire_ErrorTypeDef DS1822_Start_Conversion(DS1822_TypeDef *DS1822);
One_Wire_ErrorTypeDef DS1822_Get_Conversion_Result(DS1822_TypeDef *DS1822);
One_Wire_ErrorTypeDef DS1822_Search_Rom(DS1822_TypeDef *DS1822, One_Wire_StructTypeDef *Interface, unsigned char *devices_found);
One_Wire_ErrorTypeDef DS1822_Start_Conversion_Skip_Rom(DS1822_single_TypeDef *DS1822);
One_Wire_ErrorTypeDef DS1822_Read_Skip_Rom(DS1822_single_TypeDef *DS1822);
float DS1822_ConvertTemp(DS1822_TypeDef *DS1822);
unsigned int DS1822_GetConversionDelayValue(DS1822_TypeDef *DS1822);
