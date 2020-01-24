#ifndef DB_H
#define DB_H

#include <stdint.h>

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

enum db_data_type {
	DB_Text = 0,
	DB_8b = 1,
	DB_16b = 2,
	DB_32b = 4,
	DB_TypeError,
};

enum db_error {
	DB_Success,
	DB_WrongCRC,
	DB_TagNotFound,
	DB_TagExist,
	DB_NoCRC_Func,
	DB_WrongFormat,
	DB_Blank,
};

void db_init(uint32_t (*crc)(void *buf, uint32_t size));
uint32_t db_get_size(void *db);
enum db_error db_write(const char *tag, void *data, uint16_t datasize,
		enum db_data_type type, void *db);
enum db_error db_overwrite(const char *tag, void *data, uint16_t datasize,
		enum db_data_type type, void *db);
uint16_t db_read(const char *tag, void *data, void *db);
enum db_data_type db_get_type(const char *tag, void *db);
uint16_t db_get_entry_size(const char *tag, void *db);
enum db_error db_validate (void *db);
enum db_error db_delete(const char *tag, void *db);
uint16_t db_get_nof_tags(void *db);
char *db_error_string(enum db_error err);

#endif
