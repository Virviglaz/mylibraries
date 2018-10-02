#include "DS2482_I2C-1Wire.h"

OneWireErrorTypeDef DS2482_Init (DS2482_StructTypeDef * DS2482_InitStruct)
{
		uint8_t config = DS2482_InitStruct->Active_PullUp | DS2482_InitStruct->BusSpeed |
			DS2482_InitStruct->Strong_PullUp | (1 << 5);

		// Reset chip
		if (DS2482_InitStruct->Write(DS2482_DRST, 0, 0)) return One_Wire_InterfaceError; 
		
		// Write configuration
		DS2482_InitStruct->Write(DS2482_WCFG, &config, 1);
		
		return One_Wire_Success;
}

OneWireErrorTypeDef One_Wire_Reset (DS2482_StructTypeDef * DS2482_InitStruct)
{
	uint8_t value = DS2482_BusReset;
	OneWireErrorTypeDef result = (OneWireErrorTypeDef)DS2482_InitStruct->WriteWithFlagPooling(0, &value, DS2482_InitStruct->ReadAttemps, Status_PPD); 
	return result == One_Wire_Success ? One_Wire_Success : value & Status_PPD ? One_Wire_Success : One_Wire_Error_No_Echo;
}

OneWireErrorTypeDef One_Wire_WriteByte (DS2482_StructTypeDef * DS2482_InitStruct, uint8_t value)
{
	return (OneWireErrorTypeDef)DS2482_InitStruct->WriteWithFlagPooling(DS2482_1WWB, &value, DS2482_InitStruct->ReadAttemps, Status_1WB);
}

uint8_t One_Wire_ReadByte (DS2482_StructTypeDef * DS2482_InitStruct)
{
	const uint8_t data[2] = { DS2482_DataReg, DS2482_DataReg };
	uint8_t res;
	
	// read byte to DS2482 and wait while ready bit not set
	DS2482_InitStruct->WriteWithFlagPooling(DS2482_1WRB, 0, DS2482_InitStruct->ReadAttemps, Status_1WB);
	
	// get readed value
	DS2482_InitStruct->Read((uint8_t*)data, sizeof(data), &res, 1);
	
	return res;
}

