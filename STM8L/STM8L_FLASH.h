#ifndef STM8L_FLASH_H
#define STM8L_FLASH_H

#define EEPROM_OFFSET   0x9800

void STM8L_EEPROM_WriteByte (u16 Address, u8 Data, FLASH_MemType_TypeDef Memory);
#endif
