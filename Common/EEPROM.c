#include "EEPROM.h"

char EEPROM_RW (EEPROM_StructTypeDef * EEPROM)
{
	uint8_t m_adrs[2], Result = 0, BytesToHandle;
	
	/* Calculate start address */
	m_adrs[1] = (char)EEPROM->Mem_adrs;
	m_adrs[0] = (char)(EEPROM->Mem_adrs >> 8);
	
	/* Calculate bit mask */
	BytesToHandle = m_adrs[0] & (EEPROM->PageSize - 1);
	
	/* Apply bit mask. If first part of page exist */
	if (BytesToHandle)
	{
		if (EEPROM->isWriting)
		{
			/* Perform writing */
			Result = EEPROM->WritePage(EEPROM->I2C_Adrs, m_adrs, 2, EEPROM->buf, BytesToHandle);
			EEPROM->delay_func(EEPROM->PageWriteTime);
		}
		else
			/* Perform reading */
			Result = EEPROM->ReadPage(EEPROM->I2C_Adrs, m_adrs, 2, EEPROM->buf, BytesToHandle);
		
		EEPROM->size -= BytesToHandle;
		EEPROM->Mem_adrs += BytesToHandle;
	}
	if (!EEPROM->size || Result) return Result; //nothing to write any more

	/* Reading or Writing full pages */
	while(EEPROM->size >= EEPROM->PageSize)
	{
		if (EEPROM->isWriting)
		{
			/* Perform writing */
			Result = EEPROM->WritePage(EEPROM->I2C_Adrs, m_adrs, 2, EEPROM->buf, EEPROM->PageSize);
			EEPROM->delay_func(EEPROM->PageWriteTime);
		}
		else
			/* Perform reading */
			Result = EEPROM->ReadPage(EEPROM->I2C_Adrs, m_adrs, 2, EEPROM->buf, EEPROM->PageSize);
		
		/* Calculate new offset */
		EEPROM->buf += EEPROM->PageSize;
		EEPROM->size -= EEPROM->PageSize;
		EEPROM->Mem_adrs += EEPROM->PageSize;
		m_adrs[1] = (char)EEPROM->Mem_adrs;
		m_adrs[0] = (char)(EEPROM->Mem_adrs >> 8);			
	}
	
	if (!EEPROM->size) return Result; //nothing to write any more
	
	/* Read or write the rest of data */
	if (EEPROM->isWriting)
	{	
		/* Perform writing */
		Result = EEPROM->WritePage(EEPROM->I2C_Adrs, m_adrs, 2, EEPROM->buf, EEPROM->size);
		EEPROM->delay_func(EEPROM->PageWriteTime);
	}
	else
		/* Perform reading */
		Result = EEPROM->ReadPage(EEPROM->I2C_Adrs, m_adrs, 2, EEPROM->buf, EEPROM->size);
			
	return Result;
}
