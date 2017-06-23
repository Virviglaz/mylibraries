/* DATABASE STRUCTURE

X X X X- db lenght
X X X X - CRC32 (starting from 8-th byte till the end) -< db_offset

X..X - entry name
X X - entry size
X..X - entry data
*/

typedef enum
{
  DB_Success = 0,
  DB_WrongCRC,
  DB_TagNotFound,
  DB_TagExist,
	DB_NoCRC_Func,
	DB_WrongFormat,
	DB_Blank,
	DB_MutexNotObtained
}DB_Error;

DB_Error dbInit (long (*CRC_Func)(char * buf, long size));
DB_Error dbStoreData   (const char * Tag,	char * Data, long Size,	char * db);
DB_Error dbReadData (const char * Tag, char * Data, long * Size, char * db);
DB_Error dbEraseData (const char * Tag, char * db);
DB_Error dbCheckCRC (char * db);
long dbGetSize (char * db);
long dbGetEntrySize (const char * Tag, char * db);
