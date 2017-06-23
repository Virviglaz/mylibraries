#include "SI7005.h"

/* Factory defined constants */
const float Q0 = 0.1973;
const float Q1 = 0.00237;
const float A0 = -4.7844;
const float A1 = 0.4008;
const float A2 = -0.00393;

/**
  * @brief  Read ID register and compare with predefined value
  * @param  SI7005_Struct: pointer to structer with all settings defined
  * @retval 0 if succes, interface error code or 0xFF if ID mismatch
  */
char SI7005_CheckID (SI7005_StructTypeDef * SI7005_Struct)
{
	char ID, Res;
	Res = SI7005_Struct->ReadReg(SI7005_Struct->I2C_Adrs, 17, &ID, 1);
	if (Res) return Res;
	if (ID != 0x50) return 0xFF;
	return 0;
}

/**
  * @brief  Start conversion
  * @param  SI7005_Struct: pointer to structer with all settings defined
  * @retval Interface error code
  */
char SI7005_StartConversion (SI7005_StructTypeDef * SI7005_Struct)
{
	return SI7005_Struct->WriteReg(SI7005_Struct->I2C_Adrs, 3, SI7005_Struct->MeasType);
}

/**
  * @brief  Get result value (temperature or humidity according value of MeasType
  * @param  SI7005_Struct: pointer to structer with all settings defined
  * @retval Interface error code
  */
char SI7005_GetResult (SI7005_StructTypeDef * SI7005_Struct)
{
	char Res;
	char buf[2];
	Res = SI7005_Struct->ReadReg(SI7005_Struct->I2C_Adrs, 1, buf, 2);
	if (SI7005_Struct->MeasType == MeasHum)
	{
		/* Humidity measurement */
		SI7005_Struct->Humidity = buf[0] - 24;
		SI7005_Struct->Humidity -= ((SI7005_Struct->Humidity * SI7005_Struct->Humidity) * A2 + SI7005_Struct->Humidity * A1 + A0);
		
		/* Use temperature compensation */
		if (SI7005_Struct->UseRelativeMeas)
			SI7005_Struct->Humidity += (SI7005_Struct->Temperature - 30.0) * ((float)SI7005_Struct->Humidity * Q1 + Q0);
	}
	else
	{
		/* Temperature measurement */
		SI7005_Struct->Temperature = (buf[0] << 8 | buf[1]) >> 2;
		SI7005_Struct->Temperature /= 32;
		SI7005_Struct->Temperature -= 50;
	}
	return Res;
}

/**
  * @brief  Get value of ready bit in status register
  * @param  SI7005_Struct: pointer to structer with all settings defined
  * @retval Value of ready bit in status register
  */
char SI7005_DataReady (SI7005_StructTypeDef * SI7005_Struct)
{
	char Res;
	SI7005_Struct->ReadReg(SI7005_Struct->I2C_Adrs, 0, &Res, 1);
	return Res;	//0 - complete, 1 - in progress
}
