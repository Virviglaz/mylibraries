#include "db.h"
#include <string.h>

/* Internal types */
union
{
	long Lvar;
	char Bytes[4];
} W32;
	
/* Internal variables */
long (*CRC32)(char * buf, long size);
const char db_offset = 8;

/* Internal functions */
long FindTag (const char * Tag, char * db);
void dbUpdateSizeCRC (long db_size, char * db);

/**
  * @brief  Assign CRC calculation function
  * @param  Pointer to function
  * @retval enum DB_Success/DB_NoCRC_Func
  */
DB_Error dbInit (long (*CRC_Func)(char * buf, long size))
{
	CRC32 = CRC_Func;
	if (*CRC32 == NULL) 
		return DB_NoCRC_Func;
	return DB_Success;
}

/**
  * @brief  Store data to database
  * @param  Tag name
  * @param  Pointer to data to be stored
  * @param  Size of data
  * @param  Pointer to database
  * @retval enum DB_Success/DB_TagExist
  */
DB_Error dbStoreData   (const char * Tag,	char * Data, long Size,	char * db)
{
  long db_pointer = dbGetSize(db) + db_offset;
  
  /* Check if tag already exist */
  if (FindTag(Tag, db))
		return DB_TagExist;
  
  /* Store tag */
  while(* Tag)
    db[db_pointer++] = *Tag++;
  db[db_pointer++] = 0;	//null terminator
  
  /* Store entry size */
	W32.Lvar = Size;
  db[db_pointer++] = W32.Bytes[0];
  db[db_pointer++] = W32.Bytes[1];
  db[db_pointer++] = W32.Bytes[2];
  db[db_pointer++] = W32.Bytes[3];

  /* Store data */  
  while(Size--)
    db[db_pointer++] = *Data++;
  
  /* Update db with new size and crc */;
  dbUpdateSizeCRC(db_pointer, db);
  
  return DB_Success;
}

/**
  * @brief  Reads data by tag from database
  * @param  Tag name
  * @param  Pointer to data location
  * @param  Pointer to variable will hold size value
  * @param  Pointer to database
  * @retval enum DB_Success/DB_TagNotFound
  */
DB_Error dbReadData (const char * Tag, char * Data, long * Size, char * db)
{
  long db_pointer;
  long db_size = dbGetSize(db) - strlen(Tag);
	
	/* Find tag offset */
	db_pointer = FindTag(Tag, db) + 1;
	if (db_pointer == 0) return DB_TagNotFound;
   
  /* Get data size */
	W32.Bytes[0] = db[db_pointer++];
	W32.Bytes[1] = db[db_pointer++];
	W32.Bytes[2] = db[db_pointer++];
	W32.Bytes[3] = db[db_pointer++];
  *Size = W32.Lvar;
	
	/* Check size is valid */
	if (*Size > db_size) 
		return DB_WrongFormat;
	
  db_size = *Size;
  
  /* Fetch the data */
  while (db_size--)
    * Data++ = db[db_pointer++];
  
  return DB_Success;
}

/**
  * @brief  Erase data from database by tag
  * @param  Tag name
  * @param  Pointer to database
  * @retval enum DB_Success/DB_TagNotFound
  */
DB_Error dbEraseData (const char * Tag, char * db)
{
  long db_pointer, entrysize, tagsize;
  long db_size = dbGetSize(db) - strlen(Tag);
  
  /* Find tag by name */
	db_pointer = FindTag(Tag, db);
	if (db_pointer == 0) return DB_TagNotFound;
    
  /* Get data size */
	W32.Bytes[0] = db[db_pointer++];
	W32.Bytes[1] = db[db_pointer++];
	W32.Bytes[2] = db[db_pointer++];
	W32.Bytes[3] = db[db_pointer++];
  entrysize = W32.Lvar;
  
  /* Erase entry by moving data left */
  for (tagsize = 0; tagsize < entrysize; tagsize++)
    db[db_pointer] = db[db_pointer + entrysize];
  
  /* Update db with new size and crc */
  db_size -= entrysize;
  dbUpdateSizeCRC(db_size, db);
  return DB_Success;  
}

/**
  * @brief  Checks database for data corruption
  * @param  Pointer to database
  * @retval enum DB_Success/DB_WrongCRC
  */
DB_Error dbCheckCRC (char * db)
{
	long CRCs, CRCc, db_size = dbGetSize(db);
	CRCc = CRC32(db + db_offset, db_size - db_offset);
	W32.Bytes[0] = db[4];
	W32.Bytes[1] = db[5];
	W32.Bytes[2] = db[6];
	W32.Bytes[3] = db[7];
	CRCs = W32.Lvar;
	
  if (CRCc == CRCs)
    return DB_Success;
  else 
    return DB_WrongCRC;
}

/**
  * @brief  Reads database size
  * @param  Pointer to database
  * @retval Size of database
  */
long dbGetSize (char * db)
{
	W32.Bytes[0] = db[0];
	W32.Bytes[1] = db[1];
	W32.Bytes[2] = db[2];
	W32.Bytes[3] = db[3];
	
  return W32.Lvar;
}

/**
  * @brief  Reads size of entry by tag
  * @param  Tag name
  * @param  Pointer to database
  * @retval Size of entry
  */
long dbGetEntrySize (const char * Tag, char * db)
{
  long db_pointer = db_offset;

  /* Find tag by name */
  db_pointer = FindTag(Tag, db);
  if (db_pointer == 0) return DB_TagNotFound;
	
  /* Get data size */
	W32.Bytes[0] = db[db_pointer++];
	W32.Bytes[1] = db[db_pointer++];
	W32.Bytes[2] = db[db_pointer++];
	W32.Bytes[3] = db[db_pointer];
	
  return W32.Lvar;
}

void dbUpdateSizeCRC (long db_size, char * db)
{
	/* Size */
	W32.Lvar = db_size;
  db[0] = W32.Bytes[0];
  db[1] = W32.Bytes[1];
  db[2] = W32.Bytes[2];
  db[3] = W32.Bytes[3];  
	
	/* CRC */
  W32.Lvar = CRC32(db + db_offset, db_size - db_offset);
  db[4] = W32.Bytes[0];
  db[5] = W32.Bytes[1];
  db[6] = W32.Bytes[2];
  db[7] = W32.Bytes[3];
}

long FindTag (const char * Tag, char * db)
{
	long db_pointer = db_offset;
  long db_size = dbGetSize(db);
	if (db_size == 0) return 0;
	db_size -= strlen(Tag);
	
  /* Find tag by name */
  while (strcmp(Tag, db + db_pointer) && db_pointer < db_size)
		db_pointer++;
	
  if (db_pointer >= db_size) return 0;
  db_pointer += strlen(Tag);
	return db_pointer;
}
