#ifndef SI7005_H
#define SI7005_H

#include <stdint.h>

typedef enum
{
	MeasHum  = 0x01,
	MeasTemp = 0x11
}SI7005_MeasTypeDef;

typedef struct
{
	/* Data */
	char Humidity;
	float Temperature;
	
	/* Functions */
	uint8_t (*WriteReg)(uint8_t I2C_Adrs, uint8_t Reg, uint8_t Value);
	uint8_t (*ReadReg) (uint8_t I2C_Adrs, uint8_t Reg, uint8_t * buf, uint16_t size);	
	
	/* Settings */
	char I2C_Adrs;
	char UseRelativeMeas;
	SI7005_MeasTypeDef MeasType;
}SI7005_StructTypeDef;

uint8_t SI7005_CheckID (SI7005_StructTypeDef * SI7005_Struct);
uint8_t SI7005_StartConversion (SI7005_StructTypeDef * SI7005_Struct);
uint8_t SI7005_GetResult (SI7005_StructTypeDef * SI7005_Struct);
uint8_t SI7005_DataReady (SI7005_StructTypeDef * SI7005_Struct);

#endif
