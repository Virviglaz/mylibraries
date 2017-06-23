#include "STM8L_FLASH.h"

void STM8L_EEPROM_WriteByte (u16 Address, u8 Data, FLASH_MemType_TypeDef Memory)
{
  FLASH_Unlock(Memory);
  while (FLASH->IAPSR & FLASH_IAPSR_DUL == 0);
  *(PointerAttr uint8_t*) (uint16_t) Address = Data;
  while (FLASH->IAPSR & FLASH_IAPSR_EOP == 0);
  FLASH_Lock(Memory);
}
