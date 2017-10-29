#include "MPU6050.h"

uint8_t MPU6050_Init (MPU6050_StructTypeDef * MPU6050_Struct)
{
	uint8_t res = MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_PWR_MGMT_1, 0x80);
	if (res) return res;
	/* 500ms delay needed */
	MPU6050_Struct->delay_func(500);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_GYRO_CONFIG, MPU6050_Struct->GyroScale);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_ACCEL_CONFIG, MPU6050_Struct->AccelScale);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_INT_PIN_CFG, LATCH_INT_EN | INT_RD_CLEAR);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_INT_ENABLE, DATA_RDY_EN);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_USER_CTRL, 0x00);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_PWR_MGMT_1, 0x00);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_PWR_MGMT_2, 0x00);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_FIFO_EN, 0x00);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_SMPLRT_DIV, 1000 / MPU6050_Struct->GyroSampleRateHz - 1);
	MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_CONFIG, MPU6050_Struct->FilterOrder & 0x07);	
	return res;
}

uint8_t MPU6050_GetResult (MPU6050_StructTypeDef * MPU6050_Struct)
{
	if (MPU6050_Struct->CheckRDY_pin)
			if (!MPU6050_Struct->CheckRDY_pin()) return 1;
	
	if (MPU6050_Struct->ReadReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_INT_STATUS, (uint8_t*)MPU6050_Struct->MPU6050_Result, 15))
		return 1;
		
	if (!(MPU6050_Struct->MPU6050_Result->Status & DATA_READY_BIT)) return 1;
	
	/* Fetch & calculate the temperature */
	MPU6050_Struct->Temperature = (float)(MPU6050_Struct->MPU6050_Result->T) / 340.0 + 36.53;
	MPU6050_Struct->Temperature /= 10;
	
	if (MPU6050_Struct->MPU6050_ZeroCal == 0)	return 0; //zero calibration not needed
	
	/* Perform zero offset for accelerometer data */
	MPU6050_Struct->MPU6050_Result->X -= MPU6050_Struct->MPU6050_ZeroCal->x_offset;
	MPU6050_Struct->MPU6050_Result->Y -= MPU6050_Struct->MPU6050_ZeroCal->y_offset;
	MPU6050_Struct->MPU6050_Result->Z -= MPU6050_Struct->MPU6050_ZeroCal->z_offset;

	/* Perform zero offset for gyroscope data */	
	MPU6050_Struct->MPU6050_Result->aX -= MPU6050_Struct->MPU6050_ZeroCal->ax_offset;
	MPU6050_Struct->MPU6050_Result->aY -= MPU6050_Struct->MPU6050_ZeroCal->ay_offset;
	MPU6050_Struct->MPU6050_Result->aZ -= MPU6050_Struct->MPU6050_ZeroCal->az_offset;
	
	return 0;
}

void MPU6050_CalibrateZero (MPU6050_StructTypeDef * MPU6050_Struct)
{
	if (MPU6050_Struct->MPU6050_ZeroCal == 0) return; //pointer to zero cal not exist
	
	/* Remove offsets */
	MPU6050_Struct->MPU6050_ZeroCal->x_offset = 0;
	MPU6050_Struct->MPU6050_ZeroCal->y_offset = 0;
	MPU6050_Struct->MPU6050_ZeroCal->z_offset = 0;

	MPU6050_Struct->MPU6050_ZeroCal->ax_offset = 0;
	MPU6050_Struct->MPU6050_ZeroCal->ay_offset = 0;
	MPU6050_Struct->MPU6050_ZeroCal->az_offset = 0;
	
	/* Update data */
	while(MPU6050_GetResult(MPU6050_Struct));
	
	/* Allign accel */
	MPU6050_Struct->MPU6050_ZeroCal->x_offset = MPU6050_Struct->MPU6050_Result->X;
	MPU6050_Struct->MPU6050_ZeroCal->y_offset = MPU6050_Struct->MPU6050_Result->Y;
	MPU6050_Struct->MPU6050_ZeroCal->z_offset = MPU6050_Struct->MPU6050_Result->Z;
	
	/* Allign gyro */
	MPU6050_Struct->MPU6050_ZeroCal->ax_offset = MPU6050_Struct->MPU6050_Result->aX;
	MPU6050_Struct->MPU6050_ZeroCal->ay_offset = MPU6050_Struct->MPU6050_Result->aY;
	MPU6050_Struct->MPU6050_ZeroCal->az_offset = MPU6050_Struct->MPU6050_Result->aZ;
}
