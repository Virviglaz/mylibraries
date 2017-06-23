#include "DS1822.h"

/* Internal functions */
void DS1822_FindSensor (DS1822_TypeDef * DS1822);

/**
  * @brief  Perform writing data configuration to configuration register
  * @param  DS1822: Sensor to configure
  * @retval One_Wire_Success or One wire error
  */
One_Wire_ErrorTypeDef DS1822_Configure (DS1822_TypeDef * DS1822)
{
	One_Wire_ErrorTypeDef Result;

	/* Send RESET pulse */
	Result = One_Wire_Reset(DS1822->One_Wire_Bus);
	if (Result != One_Wire_Success) 
		return Result;

	/* Find corresponding sensor */
	DS1822_FindSensor(DS1822);

	/* Perform configure */
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_WRITE_STRATCHPAD_CMD);
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822->Th);
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822->Tl);
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, (DS1822->Resolution << 5) | 0x1F);

	return One_Wire_Success;
}

/**
  * @brief  Send start conversion command to corresponding sensor
  * @param  DS1822: Sensor to start the conversion
  * @retval One_Wire_Success or One wire error
  */
One_Wire_ErrorTypeDef DS1822_Start_Conversion (DS1822_TypeDef * DS1822)
{
	One_Wire_ErrorTypeDef Result;

	/* Send RESET pulse */
	Result = One_Wire_Reset(DS1822->One_Wire_Bus);
	if (Result != One_Wire_Success) 
		return Result;

	/* Find corresponding sensor */
	DS1822_FindSensor(DS1822);

	/* Sens start conversion command */
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_CONVERT_T_CMD);

	return One_Wire_Success;
}

/**
  * @brief  Gets conversion result and store it to sensor structure
  * @param  DS1822: Sensor to read
  * @retval One_Wire_Success or One wire error
  */
One_Wire_ErrorTypeDef DS1822_Get_Conversion_Result (DS1822_TypeDef * DS1822)
{
	One_Wire_ErrorTypeDef Result;
	unsigned char cnt;
	unsigned char inbuff[DS1822_STRATCHPAD_SIZE];

	/* Send RESET pulse */
	Result = One_Wire_Reset(DS1822->One_Wire_Bus);
	if (Result != One_Wire_Success) 
		return Result;

	/* Find corresponding sensor */
	DS1822_FindSensor(DS1822);

	/* Send READ command */
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_READ_STRATCHPAD_CMD);

	/* Read sensor's buffer */
	for (cnt=0; cnt != DS1822_STRATCHPAD_SIZE; cnt++)
		inbuff[cnt] = One_Wire_Read_Byte(DS1822->One_Wire_Bus);

	/* Check CRC */
	if (!Crc8Dallas(DS1822_STRATCHPAD_SIZE, inbuff)) 
		{
			/* Convert obtained data */
			DS1822->Temp16 = inbuff[0] | (inbuff[1] << 8);
			DS1822->Th = inbuff[2];
					DS1822->Tl = inbuff[3];
					DS1822->Resolution = (DS1822_ResolutionTypeDef) ((inbuff[4] << 5) & 0x60);
					return One_Wire_Success;
		}
	return One_Wire_CRC_Error;
}

/**
  * @brief  Gets sensors list, connected to interface. Carefull! Sensors array should be equal or more of phisical sensors connected. All sensors should have common interface!
  * @param  DS1822: Sensor to configure
  * @param  devices_found: pointer to variable that will receive value equal to amount of sensor found on bus
  * @retval One_Wire_Success or One wire error
  */
One_Wire_ErrorTypeDef DS1822_Search_Rom (DS1822_TypeDef * DS1822, One_Wire_StructTypeDef * Interface, unsigned char * devices_found)
{
	unsigned long path = 0, next, pos;                     /* decision markers */                              
 	unsigned char bit, chk;                                /* bit values */
	static unsigned char cnt_bit, cnt_byte, cnt_num = 0;
	One_Wire_ErrorTypeDef Result;

	do
	{
		/* Collect all sensor to one single interface chain */
		DS1822[cnt_num].One_Wire_Bus = Interface;
		
		/* Set defaul resolution */
		DS1822[cnt_num].Resolution = DefaultResolution;
		
		/* each ROM search pass */
 		Result = One_Wire_Reset(DS1822[cnt_num].One_Wire_Bus);
 		if (Result != One_Wire_Success) 
			return Result;

		//(issue the 'ROM search' command)
		One_Wire_Write_Byte(DS1822[cnt_num].One_Wire_Bus, DS1822_SEARCH_ROM);
		next = 0;                                 /* next path to follow */
		pos = 1;                                  /* path bit pointer */                              
		for (cnt_byte = 0; cnt_byte != 8; cnt_byte++)
		{
			DS1822[cnt_num].SN[cnt_byte] = 0;
			for (cnt_bit = 0; cnt_bit != 8; cnt_bit++)
			{                                     /* each bit of the ROM value */
 				//(read two bits, 'bit' and 'chk', from the 1-wire bus)
				bit = One_Wire_Read_Bit(DS1822[cnt_num].One_Wire_Bus);
				chk = One_Wire_Read_Bit(DS1822[cnt_num].One_Wire_Bus);
				if(!bit && !chk)
					{                   /* collision, both are zero */
						if (pos & path) 
							bit = 1;             /* if we've been here before */
						else 
							next = (path & (pos-1)) | pos;   /* else, new branch for next */
						pos <<= 1;
				}
				//(write 'bit' to the 1-wire bus)
				One_Wire_Write_Bit(DS1822[cnt_num].One_Wire_Bus, bit);

				//(save this bit as part of the current ROM value)
				if (bit) DS1822[cnt_num].SN[cnt_byte] |= (1 << cnt_bit);
				}
		}
		//(output the just-completed ROM value)
		path = next;
		cnt_num++;
	}while(path);
	* devices_found = cnt_num;
	return One_Wire_Success;
}

/**
  * @brief  Simply starts the conversion on one sensor connected to bus
  * @param  DS1822: Sensor to start conversion (single sensor)
  * @retval One_Wire_Success or One wire error
  */
One_Wire_ErrorTypeDef DS1822_Start_Conversion_Skip_Rom (DS1822_single_TypeDef * DS1822)
{
	One_Wire_ErrorTypeDef Result;

	/* Send RESET pulse */
	Result = One_Wire_Reset(DS1822->One_Wire_Bus);
	if (Result != One_Wire_Success) 
		return Result;
	
	/* Start conversion skipping ROM */
	One_Wire_Reset(DS1822->One_Wire_Bus);	
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_SKIP_ROM);
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_CONVERT_T_CMD);

	if (!One_Wire_Read_Byte(DS1822->One_Wire_Bus))	
		return One_Wire_Success;

	return One_Wire_Device_Busy;
}

/**
  * @brief  Simply reads the conversion result from one sensor connected to bus
  * @param  DS1822: Sensor to read (single sensor)
  * @retval One_Wire_Success or One wire error
  */
One_Wire_ErrorTypeDef DS1822_Read_Skip_Rom (DS1822_single_TypeDef * DS1822)
{
	One_Wire_ErrorTypeDef Result;

	Result = One_Wire_Reset(DS1822->One_Wire_Bus);
	if (Result != One_Wire_Success)
		return Result;

	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_SKIP_ROM);
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_READ_STRATCHPAD_CMD);

	DS1822->Temp = (float)(One_Wire_Read_Byte(DS1822->One_Wire_Bus) | (One_Wire_Read_Byte(DS1822->One_Wire_Bus) << 8)) / 16;
	return One_Wire_Success;
}

/**
  * @brief  Converts result to float value according choosen resolution
  * @param  DS1822: Sensor to get data from
  * @retval float temperature value
  */
float DS1822_ConvertTemp (DS1822_TypeDef * DS1822)
{
	unsigned char Div = 1 << (DS1822->Resolution + 1);
	return (float)DS1822->Temp16 / Div;
}

/**
  * @brief  Gets delay value in ms needed to perform conversion
  * @param  DS1822: Sensor to get resolution value
  * @retval delay in us needed to convert temperature
  */
unsigned int DS1822_GetConversionDelayValue (DS1822_TypeDef * DS1822)
{
	const unsigned int ConvDelay[] = {100, 200, 400, 800};
	return ConvDelay[DS1822->Resolution];
}

/**
  * @brief  Search for sensor with corresponding serial number
  * @param  DS1822: Sensor to search for
  */
void DS1822_FindSensor (DS1822_TypeDef * DS1822)
{
	unsigned char cnt;

	/* Find corresponding sensor */
	One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822_MATCH_ROM);
	for (cnt=0; cnt!=8; cnt++) 
		One_Wire_Write_Byte(DS1822->One_Wire_Bus, DS1822->SN[cnt]);
}
