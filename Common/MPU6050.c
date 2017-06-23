#include "MPU6050.h"

char MPU6050_Init (MPU6050_StructTypeDef * MPU6050_Struct)
{
	char res = MPU6050_Struct->WriteReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_PWR_MGMT_1, 0x80);
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

char MPU6050_GetResult (MPU6050_StructTypeDef * MPU6050_Struct)
{
	char Buffer[15];
	if (MPU6050_Struct->UseRDYpin)
			if (!MPU6050_Struct->CheckRDY_pin()) return 1;
	MPU6050_Struct->ReadReg(MPU6050_Struct->I2C_Adrs, MPU6050_RA_INT_STATUS, Buffer, sizeof(Buffer));
	if (!(Buffer[0] & DATA_READY_BIT)) return 1;
	
	/* Fetch acc data */
	MPU6050_Struct->x = (short)(Buffer[1] <<8 | Buffer[2]) - MPU6050_Struct->x_offset;
	MPU6050_Struct->y = (short)(Buffer[3] <<8 | Buffer[4]) - MPU6050_Struct->y_offset;
	MPU6050_Struct->z = (short)(Buffer[5] <<8 | Buffer[6]) - MPU6050_Struct->z_offset;
	
	/* Fetch temperature */
	MPU6050_Struct->T = (float)(Buffer[7] <<8 | Buffer[8]) / 340.0 + 36.53;
	MPU6050_Struct->T /= 10;
	
	/* Fetch gyro data */
	MPU6050_Struct->ax = (short)(Buffer[9]  <<8 | Buffer[10]) - MPU6050_Struct->ax_offset;
	MPU6050_Struct->ay = (short)(Buffer[11] <<8 | Buffer[12]) - MPU6050_Struct->ay_offset;
	MPU6050_Struct->az = (short)(Buffer[13] <<8 | Buffer[14]) - MPU6050_Struct->az_offset;
	return 0;
}

void MPU6050_CalibrateZero (MPU6050_StructTypeDef * MPU6050_Struct)
{
	
	/* Remove offsets */
	MPU6050_Struct->x_offset = 0;
	MPU6050_Struct->y_offset = 0;
	MPU6050_Struct->z_offset = 0;

	MPU6050_Struct->ax_offset = 0;
	MPU6050_Struct->ay_offset = 0;
	MPU6050_Struct->az_offset = 0;
	
	/* Update data */
	MPU6050_GetResult(MPU6050_Struct);
	
	/* Allign accel */
	MPU6050_Struct->x_offset = MPU6050_Struct->x;
	MPU6050_Struct->y_offset = MPU6050_Struct->y;
	MPU6050_Struct->z_offset = MPU6050_Struct->z;
	
	/* Allign gyro */
	MPU6050_Struct->ax_offset = MPU6050_Struct->ax;
	MPU6050_Struct->ay_offset = MPU6050_Struct->ay;
	MPU6050_Struct->az_offset = MPU6050_Struct->az;
}
