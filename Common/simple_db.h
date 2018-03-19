#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <string.h>

/* DATABASE STRUCTURE
8 8 8 8 - whole db size (32 bit)
8 8 8 8 - CRC32 (32 bit)

[Tag] - entry name (null-terminated)
DB_DataTypeDef DataType [8 - 16 bit]
8 8 - entry size (16 bit)
[Data] - entry data (raw data)
...
[Tag] - entry name (null-terminated)
null
DB_DataTypeDef DataType [8 bit]
8 8 - entry size (16 bit)
[Data] - entry data (raw data)
*/

typedef enum
{
	DB_Text = 0,
	DB_8b = 1,
	DB_16b = 2,
	DB_32b = 4,
	DB_TypeError,
}DB_DataTypeDef;

typedef enum
{
  DB_Success = 0,
  DB_WrongCRC,
  DB_TagNotFound,
  DB_TagExist,
	DB_NoCRC_Func,
	DB_WrongFormat,
	DB_Blank,
}DB_ErrorTypeDef;

DB_ErrorTypeDef DB_Init (uint32_t (*CRC_Func)(void * buf, uint32_t size));
DB_ErrorTypeDef DB_StoreData   (const char * Tag,	void * Data, uint16_t Size,	DB_DataTypeDef DataType, void * db);
uint16_t DB_ReadData (const char * Tag, void * Data, void * db);
DB_ErrorTypeDef DB_Validate (void * db);
DB_DataTypeDef DB_GetDataType (const char * Tag, void * db);
uint16_t DB_GetEntrySize (const char * Tag, void * db);
uint32_t DB_GetSize (void * db);
DB_ErrorTypeDef DB_DeleteEntry (const char * Tag, void * db);
uint16_t DB_GetAmountOfTags (void * db);

static const struct
{
	DB_ErrorTypeDef (* Init)  (uint32_t (*CRC_Func)(void * buf, uint32_t size));
	DB_ErrorTypeDef (* Write) (const char * Tag,	void * Data, uint16_t Size,	DB_DataTypeDef DataType, void * db);
	uint16_t (* Read) (const char * Tag, void * Data, void * db);
	DB_ErrorTypeDef (* Validate) (void * db);
	DB_DataTypeDef (* GetDataType) (const char * Tag, void * db);
	uint16_t (* GetEntrySize) (const char * Tag, void * db);
	uint32_t (* GetSize) (void * db);
	DB_ErrorTypeDef (* DeleteEntry) (const char * Tag, void * db);
	uint16_t (* GetAmountOfTags) (void * db);
}SimpleDB = { DB_Init, DB_StoreData, DB_ReadData, DB_Validate, DB_GetDataType, DB_GetEntrySize, DB_GetSize, DB_DeleteEntry, DB_GetAmountOfTags };
#endif
