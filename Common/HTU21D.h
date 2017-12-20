#ifndef HTU21D_H
#define HTU21D_H

#include <stdint.h>

#define HTU21D_CLK_HOLD_TemperatureMeasurement							0xE3
#define HTU21D_CLK_HOLD_HumidityMeasurement 								0xE5
#define HTU21D_NOHOLD_TemperatureMeasurement								0xF3
#define HTU21D_NOHOLD_HumidityMeasurement										0xF5
#define HTU21D_WriteUserRegister														0xE6
#define HTU21D_ReadUserRegister															0xE7
#define HTU21D_SoftResetRegister														0xFE

typedef enum
{
	HTU21D_12H_14T_Max = 0x00, //12 bit humidity, 14 bit temperature
	HTU21D_8H_12T_Mid = 0x01,  //8  bit humidity, 12 bit temperature
	HTU21D_10H_13T_Hi = 0x80,  //10 bit humidity, 13 bit temperature
	HTU21D_11H_11T_Lo = 0x81   //11 bit humidity, 11 bit temperature
}HTU21D_ResolutionTypeDef;

typedef enum
{
	HTU21D_HeaterDisabled = 0x00,
	HTU21D_HeaterEnabled = 0x04
}HTU21D_HeaterTypeDef;

typedef struct
{
	HTU21D_ResolutionTypeDef HTU21D_Resolution;
	HTU21D_HeaterTypeDef HTU21D_EnableHeater;
	uint8_t (*Write)(uint8_t reg, uint8_t * buf, uint8_t size);
	uint8_t (*Read) (uint8_t reg, uint8_t * buf, uint8_t size);
}HTU21D_StructTypeDef;

HTU21D_StructTypeDef * HTU21D_Init (HTU21D_StructTypeDef * Driver);
uint8_t HTU21D_SoftReset (void);
uint8_t HTU21D_Configure (void);
float HTU21D_GetTemperature (void);
float HTU21D_GetHumidity (void);

static const struct
{
	HTU21D_StructTypeDef * (* Init) (HTU21D_StructTypeDef * Driver);
	uint8_t (* SoftReset) (void);
	uint8_t (* Configure) (void);
	float (* GetTemperature) (void);
	float (* GetHumidity) (void);
}HTU21D = { HTU21D_Init, HTU21D_SoftReset, HTU21D_Configure, HTU21D_GetTemperature, HTU21D_GetHumidity };

#endif
