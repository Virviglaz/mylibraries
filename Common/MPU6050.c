#include "MPU6050.h"

#define MPU6050_RA_XG_OFFS_TC       	0x00 //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC       	0x01 //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC       	0x02 //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN      	0x03 //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN      	0x04 //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN      	0x05 //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H        	0x06 //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC     	0x07
#define MPU6050_RA_YA_OFFS_H        	0x08 //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC     	0x09
#define MPU6050_RA_ZA_OFFS_H        	0x0A //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC     	0x0B
#define MPU6050_RA_XG_OFFS_USRH     	0x13 //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL     	0x14
#define MPU6050_RA_YG_OFFS_USRH     	0x15 //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL     	0x16
#define MPU6050_RA_ZG_OFFS_USRH     	0x17 //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL     	0x18
#define MPU6050_RA_SMPLRT_DIV       	0x19
#define MPU6050_RA_CONFIG           	0x1A
#define MPU6050_RA_GYRO_CONFIG      	0x1B
#define MPU6050_RA_ACCEL_CONFIG     	0x1C
#define MPU6050_RA_FF_THR           	0x1D
#define MPU6050_RA_FF_DUR           	0x1E
#define MPU6050_RA_MOT_THR          	0x1F
#define MPU6050_RA_MOT_DUR          	0x20
#define MPU6050_RA_ZRMOT_THR        	0x21
#define MPU6050_RA_ZRMOT_DUR        	0x22
#define MPU6050_RA_FIFO_EN          	0x23
#define MPU6050_RA_I2C_MST_CTRL     	0x24
#define MPU6050_RA_I2C_SLV0_ADDR    	0x25
#define MPU6050_RA_I2C_SLV0_REG     	0x26
#define MPU6050_RA_I2C_SLV0_CTRL    	0x27
#define MPU6050_RA_I2C_SLV1_ADDR    	0x28
#define MPU6050_RA_I2C_SLV1_REG     	0x29
#define MPU6050_RA_I2C_SLV1_CTRL    	0x2A
#define MPU6050_RA_I2C_SLV2_ADDR    	0x2B
#define MPU6050_RA_I2C_SLV2_REG     	0x2C
#define MPU6050_RA_I2C_SLV2_CTRL    	0x2D
#define MPU6050_RA_I2C_SLV3_ADDR    	0x2E
#define MPU6050_RA_I2C_SLV3_REG     	0x2F
#define MPU6050_RA_I2C_SLV3_CTRL    	0x30
#define MPU6050_RA_I2C_SLV4_ADDR    	0x31
#define MPU6050_RA_I2C_SLV4_REG     	0x32
#define MPU6050_RA_I2C_SLV4_DO      	0x33
#define MPU6050_RA_I2C_SLV4_CTRL    	0x34
#define MPU6050_RA_I2C_SLV4_DI      	0x35
#define MPU6050_RA_I2C_MST_STATUS   	0x36
#define MPU6050_RA_INT_PIN_CFG      	0x37
#define MPU6050_RA_INT_ENABLE       	0x38
#define MPU6050_RA_DMP_INT_STATUS   	0x39
#define MPU6050_RA_INT_STATUS       	0x3A
#define MPU6050_RA_ACCEL_XOUT_H     	0x3B
#define MPU6050_RA_ACCEL_XOUT_L     	0x3C
#define MPU6050_RA_ACCEL_YOUT_H     	0x3D
#define MPU6050_RA_ACCEL_YOUT_L     	0x3E
#define MPU6050_RA_ACCEL_ZOUT_H     	0x3F
#define MPU6050_RA_ACCEL_ZOUT_L     	0x40
#define MPU6050_RA_TEMP_OUT_H       	0x41
#define MPU6050_RA_TEMP_OUT_L       	0x42
#define MPU6050_RA_GYRO_XOUT_H      	0x43
#define MPU6050_RA_GYRO_XOUT_L      	0x44
#define MPU6050_RA_GYRO_YOUT_H      	0x45
#define MPU6050_RA_GYRO_YOUT_L      	0x46
#define MPU6050_RA_GYRO_ZOUT_H      	0x47
#define MPU6050_RA_GYRO_ZOUT_L      	0x48
#define MPU6050_RA_EXT_SENS_DATA_00 	0x49
#define MPU6050_RA_EXT_SENS_DATA_01 	0x4A
#define MPU6050_RA_EXT_SENS_DATA_02 	0x4B
#define MPU6050_RA_EXT_SENS_DATA_03 	0x4C
#define MPU6050_RA_EXT_SENS_DATA_04 	0x4D
#define MPU6050_RA_EXT_SENS_DATA_05 	0x4E
#define MPU6050_RA_EXT_SENS_DATA_06 	0x4F
#define MPU6050_RA_EXT_SENS_DATA_07 	0x50
#define MPU6050_RA_EXT_SENS_DATA_08 	0x51
#define MPU6050_RA_EXT_SENS_DATA_09 	0x52
#define MPU6050_RA_EXT_SENS_DATA_10 	0x53
#define MPU6050_RA_EXT_SENS_DATA_11 	0x54
#define MPU6050_RA_EXT_SENS_DATA_12 	0x55
#define MPU6050_RA_EXT_SENS_DATA_13 	0x56
#define MPU6050_RA_EXT_SENS_DATA_14 	0x57
#define MPU6050_RA_EXT_SENS_DATA_15 	0x58
#define MPU6050_RA_EXT_SENS_DATA_16 	0x59
#define MPU6050_RA_EXT_SENS_DATA_17 	0x5A
#define MPU6050_RA_EXT_SENS_DATA_18 	0x5B
#define MPU6050_RA_EXT_SENS_DATA_19 	0x5C
#define MPU6050_RA_EXT_SENS_DATA_20 	0x5D
#define MPU6050_RA_EXT_SENS_DATA_21 	0x5E
#define MPU6050_RA_EXT_SENS_DATA_22 	0x5F
#define MPU6050_RA_EXT_SENS_DATA_23 	0x60
#define MPU6050_RA_MOT_DETECT_STATUS  	0x61
#define MPU6050_RA_I2C_SLV0_DO      	0x63
#define MPU6050_RA_I2C_SLV1_DO      	0x64
#define MPU6050_RA_I2C_SLV2_DO      	0x65
#define MPU6050_RA_I2C_SLV3_DO      	0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL 	0x67
#define MPU6050_RA_SIGNAL_PATH_RESET  	0x68
#define MPU6050_RA_MOT_DETECT_CTRL    	0x69
#define MPU6050_RA_USER_CTRL        	0x6A
#define MPU6050_RA_PWR_MGMT_1       	0x6B
#define MPU6050_RA_PWR_MGMT_2       	0x6C
#define MPU6050_RA_BANK_SEL         	0x6D
#define MPU6050_RA_MEM_START_ADDR   	0x6E
#define MPU6050_RA_MEM_R_W          	0x6F
#define MPU6050_RA_DMP_CFG_1        	0x70
#define MPU6050_RA_DMP_CFG_2        	0x71
#define MPU6050_RA_FIFO_COUNTH      	0x72
#define MPU6050_RA_FIFO_COUNTL      	0x73
#define MPU6050_RA_FIFO_R_W         	0x74
#define MPU6050_RA_WHO_AM_I         	0x75

/* MPU6050_RA_INT_PIN_CFG */
#define INT_LEVEL			0x80
#define INT_OPEN			0x40
#define LATCH_INT_EN			0x20
#define INT_RD_CLEAR			0x10
#define FSYNC_INT_LEVEL			0x08
#define FSYNC_INT_EN			0x04
#define I2C_BYPASS_EN			0x02

/* MPU6050_RA_INT_ENABLE */
#define MOT_EN				0x40
#define FIFI_OFLOW_EN			0x10
#define I2C_MST_INT_EN			0x08
#define DATA_RDY_EN			0x01

/* MPU6050_RA_PWR_MGMT_1 */
#define DEVICE_RESET			0x80
#define SLEEP_BIT			0x40
#define CYCLE				0x20
#define TEMP_DIS			0x08
#define DATA_READY_BIT			0x01

uint8_t mpu6050_init(struct mpu_conf *dev, void (*delay_500ms)(void))
{
	uint8_t res = dev->write_reg(MPU6050_RA_PWR_MGMT_1, 0x80);
	if (res)
		return res;

	/* 500ms delay needed */
	if (delay_500ms)
		delay_500ms();

	dev->write_reg(MPU6050_RA_GYRO_CONFIG, (uint8_t)dev->gyro_scale);
	dev->write_reg(MPU6050_RA_ACCEL_CONFIG, (uint8_t)dev->acc_scale);
	dev->write_reg(MPU6050_RA_INT_PIN_CFG, LATCH_INT_EN | INT_RD_CLEAR);
	dev->write_reg(MPU6050_RA_INT_ENABLE, DATA_RDY_EN);
	dev->write_reg(MPU6050_RA_USER_CTRL, 0x00);
	dev->write_reg(MPU6050_RA_PWR_MGMT_1, 0x00);
	dev->write_reg(MPU6050_RA_PWR_MGMT_2, 0x00);
	dev->write_reg(MPU6050_RA_FIFO_EN, 0x00);
	dev->write_reg(MPU6050_RA_SMPLRT_DIV, 1000 / dev->sample_rate_hz - 1);
	dev->write_reg(MPU6050_RA_CONFIG, dev->filter_order & 0x07);	
	return res;
}

uint8_t mpu6050_get_result(struct mpu_conf *dev)
{
	if (dev->check_ready_pin)
		if (!dev->check_ready_pin())
			return 1;

	if (dev->read_reg(MPU6050_RA_INT_STATUS, (uint8_t*)&dev->raw_result, sizeof(struct mpu_measdata)))
		return 1;

	if (!(dev->raw_result.status & DATA_READY_BIT))
		return 1;

	if (dev->zero_point) {
		/* Perform zero offset for accelerometer data */
		dev->raw_result.x -= dev->zero_point->x_offset;
		dev->raw_result.y -= dev->zero_point->y_offset;
		dev->raw_result.z -= dev->zero_point->z_offset;

		/* Perform zero offset for gyroscope data */	
		dev->raw_result.ax -= dev->zero_point->ax_offset;
		dev->raw_result.ay -= dev->zero_point->ay_offset;
		dev->raw_result.az -= dev->zero_point->az_offset;
	}

	if (dev->real_values) {
		float g = 32768.0;
		dev->real_values->temp = ((float)dev->raw_result.temp / 340.0 + 36.53) / 10.0;

		switch(dev->acc_scale) {
		case SCALE_2G:
			g /= 2;
			break;
		case SCALE_4G:
			g /= 4;
			break;
		case SCALE_8G:
			g /= 8;
			break;
		case SCALE_16G:
			g /= 16;
			break;
		}

		dev->real_values->x = (float)dev->raw_result.x / g;
		dev->real_values->y = (float)dev->raw_result.y / g;
		dev->real_values->z = (float)dev->raw_result.z / g;
	}

	return 0;
}

void mpu6050_zero_cal(struct mpu_conf *dev)
{
	if (!dev->zero_point) 
		return;

	/* Remove offsets */
	dev->zero_point->x_offset = 0;
	dev->zero_point->y_offset = 0;
	dev->zero_point->z_offset = 0;

	dev->zero_point->ax_offset = 0;
	dev->zero_point->ay_offset = 0;
	dev->zero_point->az_offset = 0;

	/* Update data */
	while(mpu6050_get_result(dev));
	
	/* Allign accel */
	dev->zero_point->x_offset = dev->raw_result.x;
	dev->zero_point->y_offset = dev->raw_result.y;
	dev->zero_point->z_offset = dev->raw_result.z;
	
	/* Allign gyro */
	dev->zero_point->ax_offset = dev->raw_result.ax;
	dev->zero_point->ay_offset = dev->raw_result.ay;
	dev->zero_point->az_offset = dev->raw_result.az;
}
