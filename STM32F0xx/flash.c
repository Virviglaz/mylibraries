#include "flash.h"
#include "CRC.h"

FLASH_Status FlashPagesErase (uint32_t Address, uint8_t Pages, const uint32_t PageSize)
{
	static uint32_t LastAddress;
	static FLASH_Status Result;
	Result = FLASH_COMPLETE;
	
	LastAddress = Address + Pages * PageSize;
	
	while (Address < LastAddress && Result == FLASH_COMPLETE)
	{
		FLASH_Unlock();
		Result = FLASH_ErasePage(Address);
		Address += PageSize;
	}
	
	FLASH_Lock();
	
	return Result;
}

FLASH_Status FlashErase (uint32_t Address, uint32_t Size,
	const uint32_t PageSize)
{
	static FLASH_Status Result;
	Result = FLASH_COMPLETE;
		
	while (Size > PageSize && Result == FLASH_COMPLETE)
	{
		FLASH_Unlock();
		Result = FLASH_ErasePage(Address);
		Address += PageSize;
		Size -= PageSize;
	}
	
	FLASH_Lock();
	
	return Result;
}


void Internal_Flash_Write32 (uint32_t address, uint32_t size, uint32_t * data)
{
	static uint32_t i;
	
	FLASH_Unlock();
	
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) 	
		FLASH->SR = FLASH_SR_EOP;
	
	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i != size; i++) 
	{
		while (FLASH->SR & FLASH_SR_BSY);
		*(__IO uint32_t *)(address + i) = data[i];
		while (!(FLASH->SR & (FLASH_SR_EOP | FLASH_SR_PGERR)));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~FLASH_CR_PG;
	
	/* Lock flash */
	FLASH->CR |= FLASH_CR_LOCK;
}

void Internal_Flash_Write8 (uint32_t address, uint32_t count, char * data) 
{
	static uint32_t i;

	FLASH_Unlock();
	
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP)
		FLASH->SR = FLASH_SR_EOP;

	FLASH->CR |= FLASH_CR_PG;

	for (i = 0; i < count; i += 2) 
	{
		*(volatile unsigned short*)(address + i) =
			(((unsigned short)data[i + 1]) << 8) + data[i];
		while (!(FLASH->SR & (FLASH_SR_EOP | FLASH_SR_PGERR)));
		FLASH->SR = FLASH_SR_EOP;
	}

	FLASH->CR &= ~FLASH_CR_PG;
	
	FLASH_Lock();
}

void Internal_Flash_Erase(uint32_t pageAddress)
{
	FLASH_Unlock();
	
	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) 
		FLASH->SR = FLASH_SR_EOP;	

	FLASH->CR |= FLASH_CR_PER;
	FLASH->AR = pageAddress;
	FLASH->CR |= FLASH_CR_STRT;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	FLASH->CR &= ~FLASH_CR_PER;
	
	FLASH_Lock();
}

bool isFlashAreaBlank (FlashArea_TypeDef * area)
{
	for (uint32_t i = area->StartAddress;
		i < (area->StartAddress + area->Size); i += sizeof(uint32_t))
			if ( *(__IO uint32_t*)i != 0xFFFFFFFF) return false;
		
	return true;
}

uint32_t flashAreaCRC32 (FlashArea_TypeDef * area)
{
	return crc32((void*)area->StartAddress, area->Size);
}

void eraseFlashArea (FlashArea_TypeDef * area)
{
	FlashErase(area->StartAddress, area->Size, FLASH_PAGE_SIZE);
}

void writeFlashPage (FlashArea_TypeDef * area, uint16_t pageNum, uint8_t * data)
{
	Internal_Flash_Write8(area->StartAddress + pageNum * FLASH_PAGE_SIZE, FLASH_PAGE_SIZE, (char*)data);
}

void copyFlashArea (FlashArea_TypeDef * source, FlashArea_TypeDef * dest)
{
	if (source->Size != dest->Size) return; //size should be equal
	
	if (isFlashAreaBlank(dest) == false)
		eraseFlashArea(dest);
	
	Internal_Flash_Write8(dest->StartAddress, source->Size, (void*)source->StartAddress);
}
