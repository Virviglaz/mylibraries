#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

typedef struct
{
	/* Data */
	uint16_t Mem_adrs;
	uint8_t * buf;
	uint16_t size;
	
	/* Functions */
	uint8_t (*WritePage)(uint8_t I2C_Adrs, uint8_t * MemPos, uint8_t MemPosSize, uint8_t * buf, uint16_t size);
	uint8_t (*ReadPage) (uint8_t I2C_Adrs, uint8_t * MemPos, uint8_t MemPosSize, uint8_t * buf, uint16_t size);	
	void (*delay_func)(unsigned int ms);
	
	/* Settings */
	uint8_t I2C_Adrs;
	uint8_t PageSize;
	uint8_t PageWriteTime;
	uint8_t isWriting;
}EEPROM_StructTypeDef;

char EEPROM_RW (EEPROM_StructTypeDef * EEPROM);

#endif
