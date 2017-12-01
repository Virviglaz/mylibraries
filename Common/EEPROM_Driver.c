#include "EEPROM_Driver.h"

EEPROM_DriverStructTypeDef * EE;
static uint8_t EEPROM_Driver_RW (uint32_t mempos, uint8_t * buf, uint16_t size, uint8_t isWriting);

EEPROM_DriverStructTypeDef * EEPROM_DriverAssign (EEPROM_DriverStructTypeDef * EEPROM_Driver)
{
	/* Kill driver */
	EE = 0;

	/* Validate parameters and assign driver */
	if (EEPROM_Driver->delay_func == 0)	return 0;
	if (EEPROM_Driver->MemAddressSize > 4) return 0;
	if (EEPROM_Driver->PageSize == 0) return 0;
	if (EEPROM_Driver->PageWriteTime_ms == 0) return 0;
	if (EEPROM_Driver->Read == 0) return 0;
	if (EEPROM_Driver->Write == 0) return 0;
	if (EEPROM_Driver)
		EE = EEPROM_Driver;
	
	return EE;
}

uint8_t EEPROM_Read (uint32_t mempos, uint8_t * buf, uint16_t size)
{
	return EEPROM_Driver_RW (mempos, buf, size, 0);
}

uint8_t EEPROM_Write (uint32_t mempos, uint8_t * buf, uint16_t size)
{
	return EEPROM_Driver_RW (mempos, buf, size, 1);
}

static uint8_t EEPROM_Driver_RW (uint32_t mempos, uint8_t * buf, uint16_t size, uint8_t isWriting)
{
	uint8_t Result;
	uint16_t BytesToHandle;
	
	/* Calculate bit mask */
	BytesToHandle = mempos & (EE->PageSize - 1);
	
	/* Apply bit mask. If first part of page exist */
	if (BytesToHandle)
	{
		if (isWriting)
		{
			/* Perform writing */
			Result = EE->Write((uint8_t*)&mempos, EE->MemAddressSize, buf, BytesToHandle);
			EE->delay_func(EE->PageWriteTime_ms);
		}
		else
			/* Perform reading */
			Result = EE->Read((uint8_t*)&mempos, EE->MemAddressSize, buf, BytesToHandle);
		
		size -= BytesToHandle;
		mempos += BytesToHandle;
	}
	
	if (size == 0) return Result; //nothing to write any more

	/* Reading or Writing full pages */
	while(size >= EE->PageSize)
	{
		if (isWriting)
		{
			/* Perform writing */
			Result = EE->Write((uint8_t*)&mempos, EE->MemAddressSize, buf, EE->PageSize);
			EE->delay_func(EE->PageWriteTime_ms);
		}
		else
			/* Perform reading */
			Result = EE->Read((uint8_t*)&mempos, EE->MemAddressSize, buf, EE->PageSize);
		
		/* Calculate new offset */
		buf += EE->PageSize;
		size -= EE->PageSize;
		mempos += EE->PageSize;		
	}
	
	if (size == 0) return Result; //nothing to write any more
	
	/* Read or write the rest of data */
	if (isWriting)
	{	
		/* Perform writing */
		Result = EE->Write((uint8_t*)&mempos, EE->MemAddressSize, buf, size);
		EE->delay_func(EE->PageWriteTime_ms);
	}
	else
		/* Perform reading */
		Result = EE->Read((uint8_t*)&mempos, EE->MemAddressSize, buf, size);
			
	return Result;
}
