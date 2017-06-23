#include "LM75.h"

char LM75_PowerOn (LM75_StructTypeDef * LM75_Struct, LM75_PowerTypeDef Enable)
{
	return LM75_Struct->WriteReg(LM75_Struct->I2C_Adrs, LM75_ConfigReg, Enable & 1);
}

char LM75_GetResult (LM75_StructTypeDef * LM75_Struct)
{
	static char result;
	static signed char tmp[2];
	
	result = LM75_Struct->ReadReg(LM75_Struct->I2C_Adrs, LM75_TempReg, (char*)tmp, sizeof(tmp));
	LM75_Struct->T = (float)(tmp[0]);
	if (tmp[1] & 0x80) LM75_Struct->T += 0.5;
	
	return result;
}
