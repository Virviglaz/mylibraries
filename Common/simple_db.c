#include "simple_db.h"
#include <string.h>

static uint32_t (*crc_func)(void *buf, uint32_t size);

static void update_db(uint32_t size, void *db)
{
	/* Update db size */
	*(uint32_t*)db = size;

	/* Update CRC32 if assigned */
	if (crc_func)
		*((uint32_t*)db + 1) = crc_func((uint8_t*)db + 8, size - 8);
}

static uint32_t find_data(const char *tag, uint16_t *tags_found, void *db)
{
	uint32_t pointer = sizeof(uint32_t) * 2, dbsize = db_get_size(db);
	uint16_t tag_size, entry_size;

	if (tags_found)
		*tags_found = 0;

	/* If db not exist return 0 */
	if (dbsize <= pointer)
		return 0;

	do {
		if (tags_found)
			(*tags_found)++;

		/* If tag found, return pointer */
		if (!strcmp(tag, (char*)db + pointer))
			return pointer;

		/* Calculate offset to next tag */
		tag_size = strlen((char*)db + pointer);
		pointer += tag_size + 2;

		/* Update entry size */
		entry_size = *((uint8_t*)db + pointer++);
		entry_size |= *((uint8_t*)db + pointer++) << 8;

		/* Shift pointer to next tag */
		pointer += entry_size;
	} while (tag_size && pointer < dbsize);

	return 0;
}

/**
  * @brief  Assign CRC calculation function
  * @param  Pointer to function
  * @retval enum DB_Success/DB_NoCRC_Func
  */
void db_init(uint32_t (*crc)(void *buf, uint32_t size))
{
	crc_func = crc;
}

/**
  * @brief  Get db size
  * @param  Pointer to database
  * @retval Size of db in bytes
  */
uint32_t db_get_size(void *db)
{
	if (*(uint32_t*)db == 0xFFFFFFFF)
		return 0;
	return *(uint32_t*)db;
}

/**
  * @brief  Store data to database
  * @param  tag name
  * @param  Pointer to data to be stored
  * @param  Size of data
  * @param  Type of data
  * @param  Pointer to database
  * @retval enum DB_Success/DB_tagExist
  */
enum db_error db_write(const char *tag, void *data, uint16_t datasize,
		enum db_data_type type, void *db)
{
	uint32_t pointer, size = db_get_size(db);

	/* Check if tag already exist */
	if (find_data(tag, 0, db))
		return DB_TagExist;

	/* Calculate initial offset */
	pointer = (size <= 8 || size == 0xFFFFFFFF) ? 8 : size;

	/* Copy tag name to db */
	strcpy((char *)db + pointer, tag);

	/* Move pointer by adding tag */
	pointer += strlen(tag);

	/* Terminate tag with null character */
	*((char *)db + pointer++) = 0;

	/* Save data type */
	*((enum db_data_type *)db + pointer++) = type;

	if (type == DB_16b)
		datasize *= sizeof(uint16_t);
	else if (type == DB_32b)
		datasize *= sizeof(uint32_t);

	/* Save data size */
	*((uint8_t*)db + pointer++) = (uint8_t)datasize;
	*((uint8_t*)db + pointer++) = datasize >> 8;

	/* Copy data to db */
	memcpy((uint8_t *)db + pointer, (uint8_t*)data, datasize);

	/* Update db size and CRC */
	pointer += datasize;
	update_db(pointer, db);

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
enum db_error db_overwrite(const char *tag, void *data, uint16_t datasize,
		enum db_data_type type, void *db)
{
	/* Check if tag already exist */
	if (find_data(tag, 0, db))
		db_delete(tag, db);

	/* erase entry and update with a new one */
	return db_write(tag, data, datasize, type, db);
}

/**
  * @brief  Reads data by tag from database
  * @param  Tag name
  * @param  Pointer to data location
  * @param  Pointer to database
  * @retval Data size, 0 if tag not found
  */
uint16_t db_read(const char *tag, void *data, void *db)
{
	uint16_t size;
	enum db_data_type type;

	/* Find Tag pointer */
	uint32_t pointer = find_data(tag, 0, db);

	/* Tag not found */
	if (pointer == 0)
		return 0;

	/* Move pointer after Tag and size */
	pointer += strlen(tag) + 1;

	/* Get data type */
	type = *((enum db_data_type *)db + pointer++);

	/* Read data size */
	size = *((uint8_t*)db + pointer++);
	size += *((uint8_t*)db + pointer++) << 8;

	/* Copy data to buffer */
	memcpy(data, (char*)db + pointer, size);

	if (type == DB_16b)
		size /= sizeof(uint16_t);
	else if (type == DB_32b)
		size /= sizeof(uint32_t);

	return size;
}

/**
  * @brief  Reads data type from database
  * @param  Tag name
  * @param  Pointer to database
  * @retval DB_Text, DB_8b, DB_16b, DB_32b or DB_TypeError if tag not found
  */
enum db_data_type db_get_type(const char *tag, void *db)
{
	/* Find Tag pointer */
	uint32_t pointer = find_data(tag, 0, db);

	/* Tag not found */
	if (pointer == 0)
		return DB_TypeError;

	/* Move pointer after Tag and size */
	pointer += strlen(tag) + 1;

	/* Read data size */
	return *((enum db_data_type *)db + pointer);
}

/**
  * @brief  Reads data size by tag from database
  * @param  Tag name
  * @param  Pointer to database
  * @retval Data size, 0 if tag not found
  */
uint16_t db_get_entry_size(const char *tag, void *db)
{
	uint16_t size;
	enum db_data_type type;

	/* Find Tag pointer */
	uint32_t pointer = find_data(tag, 0, db);

	/* Tag not found */
	if (pointer == 0)
		return 0;

	/* Move pointer after Tag and size */
	pointer += strlen(tag) + 1;

	/* Get data type */
	type = *((enum db_data_type *)db + pointer++);

	/* Read data size */
	size = *((uint8_t*)db + pointer++);
	size += *((uint8_t*)db + pointer++) << 8;

	if (type == DB_16b)
		size /= sizeof(uint16_t);
	else if (type == DB_32b)
		size /= sizeof(uint32_t);;

	return size;
}

/**
  * @brief  Validate db CRC32
  * @param  Pointer to database
  * @retval Enum DB_Success, DB_WrongCRC or DB_NoCRC_Func
  */
enum db_error db_validate (void *db)
{
	if (crc_func)
		return (*((uint32_t*)db + 1) == crc_func((uint8_t*)db + 8,
			db_get_size(db) - 8)) ? DB_Success : DB_WrongCRC;
	return DB_NoCRC_Func;
}



/**
  * @brief  Delete entry from DB
  * @param  Tag name
  * @retval Pointer to database
  */
enum db_error db_delete(const char *tag, void *db)
{
	/* Find Tag pointer */
	uint32_t pointer = find_data(tag, 0, db);
	uint32_t dbsize = db_get_size(db);
	uint16_t entry_size;

	/* Tag not found */
	if (pointer == 0)
		return DB_TagNotFound;

	/* Calculate entry size */
	entry_size = db_get_entry_size(tag, db) +
		strlen(tag) + 2 * sizeof(uint16_t);

	/* Shift the rest data */
	memcpy((uint8_t *)db + pointer, (uint8_t *)db +
		pointer + entry_size, dbsize - pointer - entry_size);

	/* Update DB size and CRC */
	update_db(dbsize - entry_size, db);

	return DB_Success;
}

/**
  * @brief  Get amount of tags in db
  * @retval Pointer to database
  */
uint16_t db_get_nof_tags(void *db)
{
	uint16_t tags;
	find_data("DEADBEEF", &tags, db);

	return tags;
}

char *db_error_string(enum db_error err)
{
	const char *DB_Error[] = {
		"Success",
		"Wrong CRC",
		"Tag not found",
		"Tag exist",
		"No CRC function",
		"Wrong format",
		"DB empty"
	};
	return (char*)DB_Error[err];
}
