#include "flash.h"

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

FLASH_Status FlashErase (uint32_t Address, uint32_t Size, const uint32_t PageSize)
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
		*(volatile unsigned short*)(address + i) = (((unsigned short)data[i + 1]) << 8) + data[i];
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
