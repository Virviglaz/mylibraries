#include "simple_db.h"
#include "CRC.h"

/* Internal variables */
uint32_t (* CRC32_CalcFunc)(void * buf, uint32_t size);
static void DB_UpdateSizeAndCRC (uint32_t size, void * db);
static uint32_t DB_FindEntryPointer (const char * Tag, uint16_t * TagsFound, void * db);

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
	if (DB_FindEntryPointer(Tag, 0, db)) return DB_TagExist;
	
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
	
	if (DataType == DB_16b) DataSize *= sizeof(uint16_t);
	if (DataType == DB_32b) DataSize *= sizeof(uint32_t);

	/* Save data size */
	*((uint8_t*)db + pointer++) = (uint8_t)DataSize;
	*((uint8_t*)db + pointer++) = DataSize >> 8;

	/* Copy data to db */
	memcpy((uint8_t*)db + pointer, (uint8_t*)Data, DataSize);
	
	/* Update db size and CRC */
	pointer += DataSize;
	DB_UpdateSizeAndCRC(pointer, db);

	return DB_Success;
}

/**
  * @brief  Store data to database overiting existing one
  * @param  Tag name
  * @param  Pointer to data to be stored
  * @param  Size of data
  * @param  Type of data
  * @param  Pointer to database
  * @retval enum DB_Success/DB_TagExist
  */
DB_ErrorTypeDef DB_OverWriteData   (const char * Tag,	void * Data, uint16_t DataSize,	DB_DataTypeDef DataType, void * db)
{
	/* Check if tag already exist */
	if (DB_FindEntryPointer(Tag, 0, db))
		DB_DeleteEntry(Tag, db);
	
	/* erase entry and update with a new one */
	return DB_StoreData(Tag, Data, DataSize, DataType, db);
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
	uint32_t pointer = DB_FindEntryPointer(Tag, 0, db);
	
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

	if (DataType == DB_16b) size /= sizeof(uint16_t);
	if (DataType == DB_32b) size /= sizeof(uint32_t);
	
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
	uint32_t pointer = DB_FindEntryPointer(Tag, 0, db);
	
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
	uint32_t pointer = DB_FindEntryPointer(Tag, 0, db);
	
	/* Tag not found */
	if (pointer == 0) return 0;

	/* Move pointer after Tag and size */
	pointer += strlen(Tag) + 1;	

	/* Get data type */
	DataType = *((DB_DataTypeDef*)db + pointer++);
	
	/* Read data size */
	size = *((uint8_t*)db + pointer++);
	size += *((uint8_t*)db + pointer++) << 8;

	if (DataType == DB_16b) size /= sizeof(uint16_t);;
	if (DataType == DB_32b) size /= sizeof(uint32_t);;
	
	return size;
}

/**
  * @brief  Validate db CRC32
  * @param  Pointer to database
  * @retval Enum DB_Success, DB_WrongCRC or DB_NoCRC_Func
  */
DB_ErrorTypeDef DB_Validate (void * db)
{
	if (CRC32_CalcFunc)
		return (*((uint32_t*)db + 1) == CRC32_CalcFunc((uint8_t*)db + 8, DB_GetSize(db) - 8)) ? DB_Success : DB_WrongCRC;
	return DB_NoCRC_Func;
}

/**
  * @brief  Get db size
  * @param  Pointer to database
  * @retval Size of db in bytes
  */
uint32_t DB_GetSize (void * db)
{
	if (*(uint32_t*)db == 0xFFFFFFFF) return 0; //flash location
	return *(uint32_t*)db;
}

/**
  * @brief  Delete entry from DB
  * @param  Tag name
  * @retval Pointer to database
  */
DB_ErrorTypeDef DB_DeleteEntry (const char * Tag, void * db)
{
	/* Find Tag pointer */
	uint32_t pointer = DB_FindEntryPointer(Tag, 0, db);
	uint32_t dbsize = DB_GetSize(db);
	uint16_t entry_size;
	
	/* Tag not found */
	if (pointer == 0) return DB_TagNotFound;
	
	/* Calculate entry size */
	entry_size = DB_GetEntrySize(Tag, db) + strlen(Tag) + 2 * sizeof(uint16_t);
	
	/* Shift the rest data */
	memcpy((uint8_t *)db + pointer, (uint8_t *)db + pointer + entry_size, dbsize - pointer - entry_size);
	
	/* Update DB size and CRC */
	DB_UpdateSizeAndCRC(dbsize - entry_size, db);
	
	return DB_Success;
}

/**
  * @brief  Get amount of tags in db
  * @retval Pointer to database
  */
uint16_t DB_GetAmountOfTags (void * db)
{
	uint16_t Tags;
	DB_FindEntryPointer("DEADBEEF", &Tags, db);

	return Tags;
}

static void DB_UpdateSizeAndCRC (uint32_t size, void * db)
{
	/* Update db size */
	*(uint32_t*)db = size;
	
	/* Update CRC32 if assigned */
	if (CRC32_CalcFunc)
		*((uint32_t*)db + 1) = CRC32_CalcFunc((uint8_t*)db + 8, size - 8);
}

static uint32_t DB_FindEntryPointer (const char * Tag, uint16_t * TagsFound, void * db)
{
	uint32_t pointer = sizeof(uint32_t) * 2, dbsize = DB_GetSize(db);
	uint16_t tag_size, entry_size;
	
	if (TagsFound) *TagsFound = 0;

	/* If db not exist return 0 */
	if (dbsize <= pointer) return 0;
	
	/* Search for tag */
	do
	{
		if (TagsFound) *TagsFound += 1;
		
		/* If tag found, return pointer */
		if (strcmp(Tag, (char*)db + pointer) == 0)
			return pointer;
		
		/* Calculate offset to next tag */
		tag_size = strlen((char*)db + pointer);
		pointer += tag_size + 2;

		/* Update entry size */
		entry_size = *((uint8_t*)db + pointer++);
		entry_size |= *((uint8_t*)db + pointer++) << 8;

		/* Shift pointer to next tag */
		pointer += entry_size;
	}while (tag_size && pointer < dbsize);
	
	return 0;
}
