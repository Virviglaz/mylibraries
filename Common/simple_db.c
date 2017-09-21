#include "simple_db.h"

/* Internal variables */
uint32_t (* CRC32_CalcFunc)(void * buf, uint32_t size);
void DB_UpdateSizeAndCRC (uint32_t size, uint32_t CRC32, void * db);
uint32_t DB_FindEntryPointer (const char * Tag, void * db);

/**
  * @brief  Assign CRC calculation function
  * @param  Pointer to function
  * @retval enum DB_Success/DB_NoCRC_Func
  */
DB_ErrorTypeDef DB_Init (uint32_t (*CRC_Func)(void * buf, uint32_t size))
{
	CRC32_CalcFunc = CRC_Func;
	if (* CRC32_CalcFunc) 
		return DB_Success;
	return DB_NoCRC_Func;
}

/**
  * @brief  Store data to database
  * @param  Tag name
  * @param  Pointer to data to be stored
  * @param  Size of data
  * @param  Type of data
  * @param  Pointer to database
  * @retval enum DB_Success/DB_TagExist
  */
DB_ErrorTypeDef DB_StoreData   (const char * Tag,	void * Data, uint16_t DataSize,	DB_DataTypeDef DataType, void * db)
{
	uint32_t pointer, size = DB_GetSize(db);
	
	/* Check if tag already exist */
	if (DB_FindEntryPointer(Tag, db)) return DB_TagExist;
	
	/* Calculate initial offset */
	pointer = (size <= 8 || size == 0xFFFFFFFF) ? 8 : size;
	
	/* Copy tag name to db */
	strcpy((char*)db + pointer, Tag);
	
	/* Move pointer by adding tag */
	pointer += strlen(Tag);
	
	/* Terminate tag with null character */
	*((char*)db + pointer++) = 0;
	
	/* Save data type */
	*((DB_DataTypeDef*)db + pointer++) = DataType;
	
	if (DataType == DB_16b) DataSize *= 2;
	if (DataType == DB_32b) DataSize *= 4;

/* Save data size */
	*((uint8_t*)db + pointer++) = (uint8_t)DataSize;
	*((uint8_t*)db + pointer++) = DataSize >> 8;

	/* Copy data to db */
	memcpy((uint8_t*)db + pointer, (uint8_t*)Data, DataSize);
	
	/* Update db size and CRC */
	pointer += DataSize;
	
	if (CRC32_CalcFunc)
		DB_UpdateSizeAndCRC(pointer, CRC32_CalcFunc((uint8_t*)db + 8, pointer), db);
	else
		DB_UpdateSizeAndCRC(pointer, 0, db); //CRC not used
	
	return DB_Success;
}

/**
  * @brief  Reads data by tag from database
  * @param  Tag name
  * @param  Pointer to data location
  * @param  Pointer to database
  * @retval Data size, 0 if tag not found
  */
uint16_t DB_ReadData (const char * Tag, void * Data, void * db)
{
	uint16_t size;
	DB_DataTypeDef DataType;
	
	/* Find Tag pointer */
	uint32_t pointer = DB_FindEntryPointer(Tag, db);
	
	/* Tag not found */
	if (pointer == 0) return 0;

	/* Move pointer after Tag and size */
	pointer += strlen(Tag) + 1;	
	
	/* Get data type */
	DataType = *((DB_DataTypeDef*)db + pointer++);
	
	/* Read data size */
	size = *((uint8_t*)db + pointer++);
	size += *((uint8_t*)db + pointer++) << 8;
	
	/* Copy data to buffer */
	memcpy(Data, (char*)db + pointer, size);

	if (DataType == DB_16b) size /= 2;
	if (DataType == DB_32b) size /= 4;
	
	return size;
}

/**
  * @brief  Reads data type from database
  * @param  Tag name
  * @param  Pointer to database
  * @retval DB_Text, DB_8b, DB_16b, DB_32b or DB_TypeError if tag not found
  */
DB_DataTypeDef DB_GetDataType (const char * Tag, void * db)
{
	/* Find Tag pointer */
	uint32_t pointer = DB_FindEntryPointer(Tag, db);
	
	/* Tag not found */
	if (pointer == 0) return DB_TypeError;

	/* Move pointer after Tag and size */
	pointer += strlen(Tag) + 1;	
	
	/* Read data size */
	return *((DB_DataTypeDef*)db + pointer);
}

/**
  * @brief  Reads data size by tag from database
  * @param  Tag name
  * @param  Pointer to database
  * @retval Data size, 0 if tag not found
  */
uint16_t DB_GetEntrySize (const char * Tag, void * db)
{
	uint16_t size;
	DB_DataTypeDef DataType;
	
	/* Find Tag pointer */
	uint32_t pointer = DB_FindEntryPointer(Tag, db);
	
	/* Tag not found */
	if (pointer == 0) return 0;

	/* Move pointer after Tag and size */
	pointer += strlen(Tag) + 1;	

	/* Get data type */
	DataType = *((DB_DataTypeDef*)db + pointer++);
	
	/* Read data size */
	size = *((uint8_t*)db + pointer++);
	size += *((uint8_t*)db + pointer++) << 8;

	if (DataType == DB_16b) size /= 2;
	if (DataType == DB_32b) size /= 4;
	
	return size;
}

/**
  * @brief  Validate db CRC32
  * @param  Pointer to database
  * @retval enum DB_Success, DB_WrongCRC or DB_NoCRC_Func
  */
DB_ErrorTypeDef DB_Validate (void * db)
{
	if (CRC32_CalcFunc)
		return (*((uint32_t*)db + 1) == CRC32_CalcFunc((uint8_t*)db + 8, DB_GetSize(db))) ? DB_Success : DB_WrongCRC;
	return DB_NoCRC_Func;
}

uint32_t DB_GetSize (void * db)
{
	return *(uint32_t*)db;
}

void DB_UpdateSizeAndCRC (uint32_t size, uint32_t CRC32, void * db)
{
	*(uint32_t*)db = size;
	*((uint32_t*)db + 1) = CRC32;
}

uint32_t DB_FindEntryPointer (const char * Tag, void * db)
{
	uint32_t pointer = 4, size = DB_GetSize(db);
	
	while (pointer++ < size)
		if (strcmp((char*)db + pointer, Tag) == 0)
			return pointer;
	
	return 0;
}
