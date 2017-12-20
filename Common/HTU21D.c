#include "HTU21D.h"

HTU21D_StructTypeDef * HTU21D_Driver;
uint8_t buf[2];

HTU21D_StructTypeDef * HTU21D_Init (HTU21D_StructTypeDef * Driver)
{
	if (Driver && Driver->Read && Driver->Write)
		HTU21D_Driver = Driver;
	return HTU21D_Driver;
}

uint8_t HTU21D_SoftReset (void)
{
	if (HTU21D_Driver)
		return HTU21D_Driver->Write(HTU21D_SoftResetRegister, 0, 0);
	return 0xFF; //no driver error
}

uint8_t HTU21D_Configure (void)
{
	uint8_t Config = HTU21D_Driver->HTU21D_EnableHeater | HTU21D_Driver->HTU21D_Resolution;
	return HTU21D_Driver->Write(HTU21D_WriteUserRegister, &Config, 1);
}

float HTU21D_GetTemperature (void)
{
	HTU21D_Driver->Read(HTU21D_NOHOLD_TemperatureMeasurement, buf, 2);
	buf[1] &= 0xFC;
	return (float)((int16_t)(buf[0] << 8 | buf[1])) * 175.72 / 65536 - 46.85;
}

float HTU21D_GetHumidity (void)
{
	HTU21D_Driver->Read(HTU21D_NOHOLD_HumidityMeasurement, buf, 2);
	buf[1] &= 0xFC;
	return (float)(buf[0] << 8 | buf[1]) * 125 / 65536 - 6;
}
