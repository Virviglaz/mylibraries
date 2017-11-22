#ifndef FLASH_H
#define FLASH_H

#include "stm32f0xx.h"                  // Device header

FLASH_Status FlashPagesErase (uint32_t Address, uint8_t Pages, const uint32_t PageSize);
FLASH_Status FlashErase (uint32_t Address, uint32_t Size, const uint32_t PageSize);
void Internal_Flash_Write32 (uint32_t address, uint32_t size, uint32_t * data);
void Internal_Flash_Write8 (uint32_t address, uint32_t count, char * data);
void Internal_Flash_Erase(uint32_t pageAddress);
#endif
