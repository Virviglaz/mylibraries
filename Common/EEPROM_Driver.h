#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include <stdint.h>

typedef struct
{
        /* Functions */
        uint8_t (*Write) (uint8_t * MemPos, uint8_t MemPosSize, uint8_t * buf, uint16_t size);
        uint8_t (*Read)  (uint8_t * MemPos, uint8_t MemPosSize, uint8_t * buf, uint16_t size);        
        void (*delay_func) (uint16_t ms);
        
        /* Settings */
        uint8_t MemAddressSize; //1, 2 or 4 bytes wide
        uint16_t PageSize;
        uint16_t PageWriteTime_ms;
}EEPROM_DriverStructTypeDef;

EEPROM_DriverStructTypeDef * EEPROM_DriverAssign (EEPROM_DriverStructTypeDef * EEPROM_Driver);
uint8_t EEPROM_Read (uint32_t mempos, uint8_t * buf, uint16_t size);
uint8_t EEPROM_Write (uint32_t mempos, uint8_t * buf, uint16_t size);

static const struct
{
        EEPROM_DriverStructTypeDef * (* DriverAssign) (EEPROM_DriverStructTypeDef * EEPROM_Driver);
        uint8_t (* Read) (uint32_t mempos, uint8_t * buf, uint16_t size);
        uint8_t (* Write) (uint32_t mempos, uint8_t * buf, uint16_t size);
}EEPROM = { EEPROM_DriverAssign, EEPROM_Read, EEPROM_Write };
#endif
