#include "BMP180.h"
#include <math.h>

/* Registers Deginition */
#define AC1_Reg		0xAA
#define AC2_Reg 	0xAC
#define AC3_Reg 	0xAE
#define AC4_Reg 	0xB0
#define AC5_Reg 	0xB2
#define AC6_Reg		0xB4
#define B1_Reg		0xB6
#define B2_Reg		0xB8
#define MB_Reg		0xBA
#define MC_Reg		0xBC
#define MD_Reg		0xBE

#define out_xlsb	0xF8
#define out_lsb		0xF7
#define out_msb		0xF6
#define ctrl_meas	0xF4
#define id				0xD0
#define soft_reset 0xE0

void BMP180_Read_UT_Value (BMP180_StructTypeDef * BMP180_Struct);	
void BMP180_Read_UP_Value (BMP180_StructTypeDef * BMP180_Struct);
void BMP180_StartConversion (BMP180_StructTypeDef * BMP180_Struct, char Flag);
unsigned char BMP180_Busy (BMP180_StructTypeDef * BMP180_Struct);
void BMP180_SW_Reset (BMP180_StructTypeDef * BMP180_Struct);

char BMP180_Init (BMP180_StructTypeDef * BMP180_Struct)
{
	char buf[22], Result;
	BMP180_SW_Reset(BMP180_Struct);
	if ((Result = BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, AC1_Reg, buf, sizeof(buf))) != 0) return Result;
	BMP180_Struct->AC1 = (buf[0]<<8)  | buf[1];
	BMP180_Struct->AC2 = (buf[2]<<8)  | buf[3];
	BMP180_Struct->AC3 = (buf[4]<<8)  | buf[5];
	BMP180_Struct->AC4 = (buf[6]<<8)  | buf[7];
	BMP180_Struct->AC5 = (buf[8]<<8)  | buf[9];
	BMP180_Struct->AC6 = (buf[10]<<8) | buf[11];
	BMP180_Struct->B1  = (buf[12]<<8) | buf[13];
	BMP180_Struct->B2  = (buf[14]<<8) | buf[15];
	BMP180_Struct->MB  = (buf[16]<<8) | buf[17];
	BMP180_Struct->MC  = (buf[18]<<8) | buf[19];
	BMP180_Struct->MD  = (buf[20]<<8) | buf[21];
	return Result;
}

void BMP180_Read_UT_Value (BMP180_StructTypeDef * BMP180_Struct)
{
	char buf[2];
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, ctrl_meas, 0x2E);
	BMP180_Struct->delay_func(50);
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, out_msb, buf, 2);
	BMP180_Struct->UT = (buf[0] << 8) + buf[1];
}

void BMP180_Read_UP_Value (BMP180_StructTypeDef * BMP180_Struct)
{
	char buf[3];
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, ctrl_meas, 0x34 + (BMP180_Struct->P_Oversampling << 6));
	BMP180_Struct->delay_func(100);
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, out_msb, buf, 3);
	BMP180_Struct->UP = ((buf[0] << 16) + (buf[1] << 8) + buf[2]) >> (8 - BMP180_Struct->P_Oversampling);
}

void BMP180_Get_Result (BMP180_StructTypeDef * BMP180_Struct)
{
	long X1, X2, B5, T;
	long B6, X3, B3;
	unsigned long B4, B7;
	BMP180_Read_UT_Value(BMP180_Struct);
	BMP180_Read_UP_Value(BMP180_Struct);
	
	/*Calculate temperature*/
	X1 = ((BMP180_Struct->UT - BMP180_Struct->AC6) * BMP180_Struct->AC5) >> 15;
	X2 = (BMP180_Struct->MC << 11) / (X1 + BMP180_Struct->MD);
	B5 = X1 + X2;
	T = (B5 + 8) >> 4;
	BMP180_Struct->Temperature = (float)T / 10;
	
	/*Calculate pressure*/
	B6 = B5 - 4000;
	X1 = (BMP180_Struct->B2 * ((B6 * B6) >> 12)) >> 11;
	X2 = (BMP180_Struct->AC2 * B6) >> 11;
	X3 = X1 + X2;
	B3 = (((BMP180_Struct->AC1 * 4 + X3) << BMP180_Struct->P_Oversampling) + 2) >> 2;
	X1 = (BMP180_Struct->AC3 * B6) >> 13;
	X2 = (BMP180_Struct->B1 * ((B6 * B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = (BMP180_Struct->AC4 * (unsigned long)(X3 + 32768)) >> 15;
	B7 = ((unsigned long)BMP180_Struct->UP - B3) * (50000 >> BMP180_Struct->P_Oversampling);
	if (B7 < 0x80000000) BMP180_Struct->Pressure = (B7 * 2) / B4;
		else BMP180_Struct->Pressure = (B7 / B4) * 2;
	X1 = (BMP180_Struct->Pressure >> 8) * (BMP180_Struct->Pressure >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * (BMP180_Struct->Pressure)) >> 16;
	BMP180_Struct->Pressure = BMP180_Struct->Pressure + ((X1 + X2 + 3791) >> 4);
}

/* Returns absolute altitude */
float Altitude (long Pressure)
{
	const float p0 = 101325;     // Pressure at sea level (Pa)
	return (float)44330 * (1 - pow(((float) Pressure/p0), 0.190295));
}

/* Converts pressure to mm Hg */
unsigned short Pa_To_Hg (long Pressure_In_Pascals)
{
	return (unsigned long)(Pressure_In_Pascals * 760) / 101325;
}

char BMP180_Check_ID (BMP180_StructTypeDef * BMP180_Struct)
{
	char inbuff;
	BMP180_Struct->ReadReg(BMP180_Struct->I2C_Adrs, 0xD0, &inbuff, 1);
	if (inbuff == 0x55) return 0;
	return 1;
}

void BMP180_StartConversion (BMP180_StructTypeDef * BMP180_Struct, char Flag)
{
	BMP180_Struct->WriteReg(BMP180_Struct->I2C_Adrs, ctrl_meas, Flag);
}

unsigned char BMP180_Busy (BMP180_StructTypeDef * BMP180_Struct)
{
	char inbuff;	
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
