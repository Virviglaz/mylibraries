#include "MAX7219.h"

/* Private functions  declaration */
uint8_t GetDeviceN (MAX7219_InitStructTypeDef * MAX7219_InitStruct);

/* Public functions */
uint8_t MAX7219_Init (MAX7219_InitStructTypeDef * MAX7219_InitStruct)
{
	if (MAX7219_InitStruct->DeviceN == 0) //autodetect amount of devices on bus if not defined
		MAX7219_InitStruct->DeviceN = GetDeviceN(MAX7219_InitStruct);
	
	
}

uint8_t GetDeviceN (MAX7219_InitStructTypeDef * MAX7219_InitStruct)
{
	uint8_t cnt = 0, rtn;
	if (MAX7219_InitStruct->SendByte == 0) return 0;
	
	do
	{
		cnt++;
		rtn = MAX7219_InitStruct->SendByte(0xFF);
	}while (cnt < 100 && rtn != 0xFF)
		
	cnt = 0;
	
	do
	{
		cnt++;
		rtn = MAX7219_InitStruct->SendByte(0x00);
	}while (cnt < 100 && rtn != 0x00)
		
	return cnt;
}


