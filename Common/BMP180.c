#include "BMP180.h"
#include <math.h>

/* Internal variables and functions */
long UT;
long UP;
void BMP180_Read_UT_Value (BMP180_StructTypeDef * BMP180_Struct);	
void BMP180_Read_UP_Value (BMP180_StructTypeDef * BMP180_Struct);
void BMP180_StartConversion (BMP180_StructTypeDef * BMP180_Struct, uint8_t Flag);
uint8_t BMP180_Busy (BMP180_StructTypeDef * BMP180_Struct);
void BMP180_SW_Reset (BMP180_StructTypeDef * BMP180_Struct);

/* Public functions */
uint8_t BMP180_Init (BMP180_StructTypeDef * BMP180_Struct)
{
	uint8_t buf[22], Result;
	BMP180_SW_Reset(BMP180_Struct);
	if ((Result = BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, AC1_Reg, buf, sizeof(buf))) != 0) return Result;
	BMP180_Struct->BMP180_Calibration->AC1 = (buf[0]<<8)  | buf[1];
	BMP180_Struct->BMP180_Calibration->AC2 = (buf[2]<<8)  | buf[3];
	BMP180_Struct->BMP180_Calibration->AC3 = (buf[4]<<8)  | buf[5];
	BMP180_Struct->BMP180_Calibration->AC4 = (buf[6]<<8)  | buf[7];
	BMP180_Struct->BMP180_Calibration->AC5 = (buf[8]<<8)  | buf[9];
	BMP180_Struct->BMP180_Calibration->AC6 = (buf[10]<<8) | buf[11];
	BMP180_Struct->BMP180_Calibration->B1  = (buf[12]<<8) | buf[13];
	BMP180_Struct->BMP180_Calibration->B2  = (buf[14]<<8) | buf[15];
	BMP180_Struct->BMP180_Calibration->MB  = (buf[16]<<8) | buf[17];
	BMP180_Struct->BMP180_Calibration->MC  = (buf[18]<<8) | buf[19];
	BMP180_Struct->BMP180_Calibration->MD  = (buf[20]<<8) | buf[21];
	if (BMP180_Struct == 0 || BMP180_Struct->BMP180_Calibration == 0) return 1;
	//Result = BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, AC1_Reg, (uint8_t*)&BMP180_Struct->BMP180_Calibration, sizeof(BMP180_CalibrationStructTypeDef));
	return Result;
}

void BMP180_Read_UT_Value (BMP180_StructTypeDef * BMP180_Struct)
{
	uint8_t buf[2];
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, ctrl_meas, 0x2E);
	BMP180_Struct->delay_func(50);
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, out_msb, buf, 2);
	UT = (buf[0] << 8) + buf[1];
}

void BMP180_Read_UP_Value (BMP180_StructTypeDef * BMP180_Struct)
{
	uint8_t buf[3];
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, ctrl_meas, 0x34 + (BMP180_Struct->P_Oversampling << 6));
	BMP180_Struct->delay_func(100);
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, out_msb, buf, 3);
	UP = ((buf[0] << 16) + (buf[1] << 8) + buf[2]) >> (8 - BMP180_Struct->P_Oversampling);
}

void BMP180_Get_Result (BMP180_StructTypeDef * BMP180_Struct)
{
	long X1, X2, B5, T;
	long B6, X3, B3;
	unsigned long B4, B7;
	BMP180_Read_UT_Value(BMP180_Struct);
	BMP180_Read_UP_Value(BMP180_Struct);
	
	/*Calculate temperature*/
	X1 = ((UT - BMP180_Struct->BMP180_Calibration->AC6) * BMP180_Struct->BMP180_Calibration->AC5) >> 15;
	X2 = (BMP180_Struct->BMP180_Calibration->MC << 11) / (X1 + BMP180_Struct->BMP180_Calibration->MD);
	B5 = X1 + X2;
	T = (B5 + 8) >> 4;
	BMP180_Struct->Temperature = (float)T / 10;
	
	/*Calculate pressure*/
	B6 = B5 - 4000;
	X1 = (BMP180_Struct->BMP180_Calibration->B2 * ((B6 * B6) >> 12)) >> 11;
	X2 = (BMP180_Struct->BMP180_Calibration->AC2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = (((BMP180_Struct->BMP180_Calibration->AC1 * 4 + X3) << BMP180_Struct->P_Oversampling) + 2) >> 2;
	X1 = (BMP180_Struct->BMP180_Calibration->AC3 * B6) >> 13;
	X2 = (BMP180_Struct->BMP180_Calibration->B1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = (BMP180_Struct->BMP180_Calibration->AC4 * (unsigned long)(X3 + 32768)) >> 15;
	B7 = ((unsigned long)UP - B3) * (50000 >> BMP180_Struct->P_Oversampling);
	if (B7 < 0x80000000) BMP180_Struct->Pressure = (B7 * 2) / B4;
		else BMP180_Struct->Pressure = (B7 / B4) * 2;
	X1 = (BMP180_Struct->Pressure >> 8) * (BMP180_Struct->Pressure >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * (BMP180_Struct->Pressure)) >> 16;
	BMP180_Struct->Pressure = BMP180_Struct->Pressure + ((X1 + X2 + 3791) >> 4);
}

/* Returns absolute altitude */
float Altitude (uint32_t Pressure)
{
	const float p0 = 101325;     // Pressure at sea level (Pa)
	return (float)44330 * (1 - pow(((float) Pressure/p0), 0.190295));
}

/* Converts pressure to mm Hg */
unsigned short Pa_To_Hg (uint32_t Pressure_In_Pascals)
{
	return (unsigned long)(Pressure_In_Pascals * 760) / 101325;
}

uint8_t BMP180_Check_ID (BMP180_StructTypeDef * BMP180_Struct)
{
	uint8_t inbuff;
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, 0xD0, &inbuff, 1);
	if (inbuff == 0x55) return 0;
	return 1;
}

void BMP180_StartConversion (BMP180_StructTypeDef * BMP180_Struct, uint8_t Flag)
{
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, ctrl_meas, Flag);
}

unsigned char BMP180_Busy (BMP180_StructTypeDef * BMP180_Struct)
{
	uint8_t inbuff;	
	BMP180_Struct->delay_func(10);
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, ctrl_meas, &inbuff, 1);
	return inbuff & (1<<5);
}

void BMP180_SW_Reset (BMP180_StructTypeDef * BMP180_Struct)
{
	BMP180_Struct->delay_func(100);
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, soft_reset, 0xB6);
	BMP180_Struct->delay_func(100);
}
