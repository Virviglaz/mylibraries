#ifndef FLASH_H
#define FLASH_H

#include "stm32f0xx.h"                  // Device header
#include "stm32f0xx_flash.h"
#include "PCB23_HW.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

FLASH_Status FlashPagesErase (uint32_t Address, uint8_t Pages, const uint32_t PageSize);
FLASH_Status FlashErase (uint32_t Address, uint32_t Size, const uint32_t PageSize);
void Internal_Flash_Write32 (uint32_t address, uint32_t size, uint32_t * data);
void Internal_Flash_Write8 (uint32_t address, uint32_t count, char * data);
void Internal_Flash_Erase(uint32_t pageAddress);
bool isFlashAreaBlank (FlashArea_TypeDef * area);
uint32_t flashAreaCRC32 (FlashArea_TypeDef * area);
void eraseFlashArea (FlashArea_TypeDef * area);
void writeFlashPage (FlashArea_TypeDef * area, uint16_t pageNum, uint8_t * data);
void copyFlashArea (FlashArea_TypeDef * source, FlashArea_TypeDef * dest);
#endif
