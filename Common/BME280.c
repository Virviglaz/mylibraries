#include "BME280.h"

/* Registers Deginition */
#define BME280_hum_lsb 				0xFE
#define BME280_hum_msb 				0xFD
#define BME280_temp_xlsb 			0xFC
#define BME280_temp_lsb 			0xFB
#define BME280_temp_msb 			0xFA
#define BME280_press_xlsb 		0xF9
#define BME280_press_lsb 			0xF8
#define BME280_press_msb 			0xF7
#define BME280_config 				0xF5
#define BME280_ctrl_meas 			0xF4
#define BME280_status 				0xF3
#define BME280_ctrl_hum 			0xF2
#define BME280_reset 					0xE0
#define BME280_id 						0xD0

/* Types Deginition */
#define BME280_S32_t			signed long
#define BME280_U32_t			unsigned long
#define BME280_S64_t			signed long long

/* Internal functions */
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T, BME280_StructTypeDef * BME280_Struct);
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, BME280_StructTypeDef * BME280_Struct);
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H, BME280_StructTypeDef * BME280_Struct);

uint8_t BME280_Init (BME280_StructTypeDef * BME280_Struct)
{
	uint8_t Result;
	uint8_t buf[4];
	
	if (BME280_Struct == 0 || BME280_Struct->BME280_Calibration == 0) return 1;

	if ((Result = BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0x88, (uint8_t*)BME280_Struct->BME280_Calibration, 24)) != 0) return Result; //T1..T3, P1..P9 -> 0x88..0x9F
	BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0xA1, (uint8_t*)&BME280_Struct->BME280_Calibration->dig_H1, 1);	//H1 -> 0xA1
	BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0xE1, (uint8_t*)&BME280_Struct->BME280_Calibration->dig_H2, 2); //H2 -> 0xE1..0xE2
	BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0xE3, (uint8_t*)&BME280_Struct->BME280_Calibration->dig_H3, 1); //H3 -> 0xE3
	BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0xE7, (uint8_t*)&BME280_Struct->BME280_Calibration->dig_H6, 1); //H6 -> 0xE7

	BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0xE4, buf, 1); //H6 -> 0xE7
	BME280_Struct->BME280_Calibration->dig_H4 = (buf[1] & 0x07) | ( buf[0] << 4);
	BME280_Struct->BME280_Calibration->dig_H5 = (buf[2] & 0x07) | ( buf[3] << 4);

	BME280_Struct->WriteReg(BME280_Struct->I2C_Adrs, BME280_ctrl_meas, (BME280_Struct->TemperatureOversampling & 7) << 5 | (BME280_Struct->PressureOversampling & 7) << 2 | 3);
	BME280_Struct->WriteReg(BME280_Struct->I2C_Adrs, BME280_ctrl_hum, BME280_Struct->HumidityOversampling & 7);
	return Result;
}

uint8_t BME280_Get_Result (BME280_StructTypeDef * BME280_Struct)
{
	uint8_t Result;
	uint8_t buff[8];
	if ((Result = BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, 0xF7, buff, sizeof(buff))) != 0) return Result;
	
	BME280_Struct->Temperature = (float)BME280_compensate_T_int32((BME280_S32_t)(buff[5] | buff[4] << 8 | buff[3] << 16) >> 4, BME280_Struct) / 100;
	BME280_Struct->Pressure = (long)(BME280_compensate_P_int64((BME280_S32_t)(buff[2] | buff[1] << 8 | buff[0] << 16) >> 4, BME280_Struct) >> 8);
	BME280_Struct->Humidity = (float)(BME280_compensate_H_int32((BME280_S32_t)(buff[7] | buff[6] << 8), BME280_Struct)) / 1024;
	return Result;
}

// 0 when conversion is done
uint8_t BME280_Busy (BME280_StructTypeDef * BME280_Struct)
{
  uint8_t Result;
  BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, BME280_status, &Result, 1);
	return Result & (1 << 3);
}

uint8_t BME280_Check_ID (BME280_StructTypeDef * BME280_Struct)
{
	uint8_t Result;
	BME280_Struct->ReadReg(BME280_Struct->I2C_Adrs, BME280_id, &Result, 1);
	return Result - 0x60;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of 5123 equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t t_fine;
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T, BME280_StructTypeDef * BME280_Struct)
{
	BME280_S32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_T1 << 1))) * ((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_T1))) >> 12) *
	((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of 24674867 represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P, BME280_StructTypeDef * BME280_Struct)
{
	BME280_S64_t var1, var2, p;
	var1 = ((BME280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P6;
	var2 = var2 + ((var1*(BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P5) << 17);
	var2 = var2 + (((BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P4) << 35);
	var1 = ((var1 * var1 * (BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P3) >> 8) + ((var1 * (BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P2) << 12);
	var1 = (((((BME280_S64_t)1) << 47) + var1)) * ((BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P1) >> 33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)BME280_Struct->BME280_Calibration->dig_P7) << 4);
	return (BME280_U32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of 47445 represents 47445/1024 = 46.333 %RH
BME280_U32_t BME280_compensate_H_int32(BME280_S32_t adc_H, BME280_StructTypeDef * BME280_Struct)
{
	BME280_S32_t Hum_s32_var;
	
	Hum_s32_var = (t_fine - ((BME280_S32_t)76800));
	Hum_s32_var = (((((adc_H << 14) - (((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_H4) << 20) - (((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_H5) * Hum_s32_var)) +
		((BME280_S32_t)16384)) >> 15) * (((((((Hum_s32_var * ((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_H6)) >> 10) * (((Hum_s32_var *
			((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
				((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_H2) + 8192) >> 14));
	Hum_s32_var = (Hum_s32_var - (((((Hum_s32_var >> 15) * (Hum_s32_var >> 15)) >> 7) * ((BME280_S32_t)BME280_Struct->BME280_Calibration->dig_H1)) >> 4));
	Hum_s32_var = (Hum_s32_var < 0 ? 0 : Hum_s32_var);
	Hum_s32_var = (Hum_s32_var > 419430400 ? 419430400 : Hum_s32_var);

	return (BME280_U32_t)(Hum_s32_var >> 12);
}
