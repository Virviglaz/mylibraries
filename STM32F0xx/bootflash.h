#ifndef BOOTFLASH_H
#define BOOTFLASH_H

#include <stdint.h>
#include <stdio.h>

#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)

void SYSCFG_MemoryRemapConfig(uint32_t SYSCFG_MemoryRemap);
void JumpToMemory (uint32_t offset);
void Remap_Table(uint32_t FW_Address);
void DisableInterrupts (void);
#endif
