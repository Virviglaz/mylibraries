#include "DS18B20.h"

/* Internal functions */
void DS18B20_FindSensor (DS18B20_TypeDef * DS18B20);

/**
  * @brief  Perform writing data configuration to configuration register
  * @param  DS18B20: Sensor to configure
  * @retval One_Wire_Success or One wire error
  */
char DS18B20_Configure (DS18B20_TypeDef * DS18B20)
{
	char Result;

	/* Send RESET pulse */
	Result = DS18B20->One_Wire_Interface->ResetFunc();
	if (Result)	return Result;

	/* Find corresponding sensor */
	DS18B20_FindSensor(DS18B20);

	/* Perform configure */
	DS18B20->One_Wire_Interface->WriteByte (DS18B20_WRITE_STRATCHPAD_CMD);
	DS18B20->One_Wire_Interface->WriteByte (DS18B20->Th);
	DS18B20->One_Wire_Interface->WriteByte (DS18B20->Tl);
	DS18B20->One_Wire_Interface->WriteByte ((DS18B20->Resolution << 5) | 0x1F);

	return Result;
}

/**
  * @brief  Send start conversion command to corresponding sensor
  * @param  DS18B20: Sensor to start the conversion
  * @retval One_Wire_Success or One wire error
  */
char DS18B20_Start_Conversion (DS18B20_TypeDef * DS18B20)
{
	char Result;

	/* Send RESET pulse */
	Result = DS18B20->One_Wire_Interface->ResetFunc();
	if (Result)	return Result;

	/* Find corresponding sensor */
	DS18B20_FindSensor(DS18B20);

	/* Sens start conversion command */
	DS18B20->One_Wire_Interface->WriteByte (DS18B20_CONVERT_T_CMD);

	return Result;
}

/**
  * @brief  Gets conversion result and store it to sensor structure
  * @param  DS18B20: Sensor to read
  * @retval One_Wire_Success or One wire error
  */
char DS18B20_Get_Conversion_Result (DS18B20_TypeDef * DS18B20)
{
	char Result;
	unsigned char cnt;
	unsigned char inbuff[DS18B20_STRATCHPAD_SIZE];

	/* Send RESET pulse */
	Result = DS18B20->One_Wire_Interface->ResetFunc();
	if (Result)	return Result;

	/* Find corresponding sensor */
	DS18B20_FindSensor(DS18B20);

	/* Send READ command */
	DS18B20->One_Wire_Interface->WriteByte (DS18B20_READ_STRATCHPAD_CMD);

	/* Read sensor's buffer */
	for (cnt=0; cnt != DS18B20_STRATCHPAD_SIZE; cnt++)
		inbuff[cnt] = DS18B20->One_Wire_Interface->ReadByte();

	/* Check CRC */
	Result = Crc8Dallas(DS18B20_STRATCHPAD_SIZE, inbuff);
	if (!Result) 
		{
			/* Convert obtained data */
			DS18B20->Temp16 = inbuff[0] | (inbuff[1] << 8);
			DS18B20->Th = inbuff[2];
					DS18B20->Tl = inbuff[3];
					DS18B20->Resolution = (DS18B20_ResolutionTypeDef) ((inbuff[4] << 5) & 0x60);
		}
	return Result;
}

/**
  * @brief  Simply starts the conversion on one sensor connected to bus
  * @param  DS18B20: Sensor to start conversion (single sensor)
  * @retval One_Wire_Success or One wire error
  */
char DS18B20_Start_Conversion_Skip_Rom (DS18B20_single_TypeDef * DS18B20)
{
	/* Send RESET pulse */
	char Result = DS18B20->One_Wire_Interface->ResetFunc();
	if (Result)	return Result;
	
	/* Start conversion skipping ROM */
	DS18B20->One_Wire_Interface->ResetFunc();	
	DS18B20->One_Wire_Interface->WriteByte (DS18B20_SKIP_ROM);
	DS18B20->One_Wire_Interface->WriteByte (DS18B20_CONVERT_T_CMD);

	return Result;
}

/**
  * @brief  Simply reads the conversion result from one sensor connected to bus
  * @param  DS18B20: Sensor to read (single sensor)
  * @retval One_Wire_Success or One wire error
  */
char DS18B20_Read_Skip_Rom (DS18B20_single_TypeDef * DS18B20)
{
	char Result;
	int16_t rawvalue;

	Result = DS18B20->One_Wire_Interface->ResetFunc();
	if (Result)	return Result;

	DS18B20->One_Wire_Interface->WriteByte (DS18B20_SKIP_ROM);
	DS18B20->One_Wire_Interface->WriteByte (DS18B20_READ_STRATCHPAD_CMD);
	rawvalue = DS18B20->One_Wire_Interface->ReadByte();
	rawvalue |= DS18B20->One_Wire_Interface->ReadByte() << 8;
	
	DS18B20->Temp = (float)(rawvalue) / 16;
	return Result;
}

/**
  * @brief  Converts result to float value according choosen resolution
  * @param  DS18B20: Sensor to get data from
  * @retval float temperature value
  */
float DS18B20_ConvertTemp (DS18B20_TypeDef * DS18B20)
{
	unsigned char Div = 1 << (DS18B20->Resolution + 1);
	return (float)DS18B20->Temp16 / Div;
}

/**
  * @brief  Gets delay value in ms needed to perform conversion
  * @param  DS18B20: Sensor to get resolution value
  * @retval delay in us needed to convert temperature
  */
unsigned int DS18B20_GetConversionDelayValue (DS18B20_TypeDef * DS18B20)
{
	const unsigned int ConvDelay[] = {100, 200, 400, 800};
	return ConvDelay[DS18B20->Resolution];
}

/**
  * @brief  Search for sensor with corresponding serial number
  * @param  DS18B20: Sensor to search for
  */
void DS18B20_FindSensor (DS18B20_TypeDef * DS18B20)
{
	unsigned char cnt;

	/* Find corresponding sensor */
	DS18B20->One_Wire_Interface->WriteByte ( DS18B20_MATCH_ROM);
	for (cnt=0; cnt!=8; cnt++) 
		DS18B20->One_Wire_Interface->WriteByte ( DS18B20->SN[cnt]);
}
