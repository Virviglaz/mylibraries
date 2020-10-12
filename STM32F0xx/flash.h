#ifndef FLASH_H
#define FLASH_H

#include "stm32f0xx.h"                  // Device header
#include "stm32f0xx_flash.h"
#include "bsp.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

FLASH_Status FlashPagesErase (uint32_t Address, uint8_t Pages, const uint32_t PageSize);
FLASH_Status FlashErase (uint32_t Address, uint32_t Size, const uint32_t PageSize);
void Internal_Flash_Write32 (uint32_t address, uint32_t size, uint32_t * data);
void Internal_Flash_Write8 (uint32_t address, uint32_t count, char * data);
void Internal_Flash_Erase(uint32_t pageAddress);
bool isFlashAreaBlank (flash_t * area);
uint32_t flashAreaCRC32 (flash_t * area);
void eraseFlashArea (flash_t * area);
void writeFlashPage (flash_t * area, uint16_t pageNum, uint8_t * data);
void copyFlashArea (flash_t * source, flash_t * dest);
#endif
