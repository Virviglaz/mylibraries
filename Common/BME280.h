#ifndef BME280_H
#define BME280_H

#include <stdint.h>

typedef enum
{
	BME280_OV_Skipped = 0,
	BME280_OversamplingX1,
	BME280_OversamplingX2,
	BME280_OversamplingX4,
	BME280_OversamplingX8,
	BME280_OversamplingX16,	
}BME280_OversamplingEnumTypeDef;

typedef struct
{
	unsigned short 	dig_T1;
	short 					dig_T2;
	short						dig_T3;
	unsigned short 	dig_P1;
	short 					dig_P2;
	short 					dig_P3;
	short 					dig_P4;
	short 					dig_P5;
	short 					dig_P6;
	short 					dig_P7;
	short 					dig_P8;
	short 					dig_P9;
	char						dig_H1;
	short						dig_H2;
	char						dig_H3;
	short						dig_H4;
	short						dig_H5;
	signed char			dig_H6;
}BME280_CalibrationStructTypeDef;

typedef struct
{
	/* Data */
	float Humidity;
	long Pressure;
	float Temperature;
	
	/* Functions */
	uint8_t (*WriteReg)(uint8_t I2C_Adrs, uint8_t Reg, uint8_t Value);
	uint8_t (*ReadReg) (uint8_t I2C_Adrs, uint8_t Reg, uint8_t * buf, uint16_t size);
	
	/* Settings */
	BME280_OversamplingEnumTypeDef HumidityOversampling;
	BME280_OversamplingEnumTypeDef PressureOversampling;
	BME280_OversamplingEnumTypeDef TemperatureOversampling;
	uint8_t I2C_Adrs;				//I2c address. Default value 0xEC
	BME280_CalibrationStructTypeDef * BME280_Calibration;
}BME280_StructTypeDef;

uint8_t BME280_Init (BME280_StructTypeDef * BME280_Struct);
uint8_t BME280_Get_Result (BME280_StructTypeDef * BME280_Struct);
uint8_t BME280_Busy (BME280_StructTypeDef * BME280_Struct);
uint8_t BME280_Check_ID (BME280_StructTypeDef * BME280_Struct);

#endif
