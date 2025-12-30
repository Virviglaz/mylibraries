/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2022-2025 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The MPU-60X0 features three 16-bit analog-to-digital converters (ADCs)
 * for digitizing the gyroscope outputs
 * and three 16-bit ADCs for digitizing the accelerometer outputs.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "mpu6050.h"

#define MPU6050_RA_XG_OFFS_TC 		0x00
#define MPU6050_RA_YG_OFFS_TC 		0x01
#define MPU6050_RA_ZG_OFFS_TC 		0x02
#define MPU6050_RA_X_FINE_GAIN  	0x03
#define MPU6050_RA_Y_FINE_GAIN  	0x04
#define MPU6050_RA_Z_FINE_GAIN  	0x05
#define MPU6050_RA_XA_OFFS_H  		0x06
#define MPU6050_RA_XA_OFFS_L_TC 	0x07
#define MPU6050_RA_YA_OFFS_H  		0x08
#define MPU6050_RA_YA_OFFS_L_TC 	0x09
#define MPU6050_RA_ZA_OFFS_H  		0x0A
#define MPU6050_RA_ZA_OFFS_L_TC 	0x0B
#define MPU6050_RA_XG_OFFS_USRH 	0x13
#define MPU6050_RA_XG_OFFS_USRL 	0x14
#define MPU6050_RA_YG_OFFS_USRH 	0x15
#define MPU6050_RA_YG_OFFS_USRL 	0x16
#define MPU6050_RA_ZG_OFFS_USRH 	0x17
#define MPU6050_RA_ZG_OFFS_USRL 	0x18
#define MPU6050_RA_SMPLRT_DIV 		0x19
#define MPU6050_RA_CONFIG   		0x1A
#define MPU6050_RA_GYRO_CONFIG  	0x1B
#define MPU6050_RA_ACCEL_CONFIG 	0x1C
#define MPU6050_RA_FF_THR   		0x1D
#define MPU6050_RA_FF_DUR   		0x1E
#define MPU6050_RA_MOT_THR  		0x1F
#define MPU6050_RA_MOT_DUR  		0x20
#define MPU6050_RA_ZRMOT_THR  		0x21
#define MPU6050_RA_ZRMOT_DUR  		0x22
#define MPU6050_RA_FIFO_EN  		0x23
#define MPU6050_RA_I2C_MST_CTRL 	0x24
#define MPU6050_RA_I2C_SLV0_ADDR  	0x25
#define MPU6050_RA_I2C_SLV0_REG 	0x26
#define MPU6050_RA_I2C_SLV0_CTRL  	0x27
#define MPU6050_RA_I2C_SLV1_ADDR  	0x28
#define MPU6050_RA_I2C_SLV1_REG 	0x29
#define MPU6050_RA_I2C_SLV1_CTRL  	0x2A
#define MPU6050_RA_I2C_SLV2_ADDR  	0x2B
#define MPU6050_RA_I2C_SLV2_REG 	0x2C
#define MPU6050_RA_I2C_SLV2_CTRL  	0x2D
#define MPU6050_RA_I2C_SLV3_ADDR  	0x2E
#define MPU6050_RA_I2C_SLV3_REG 	0x2F
#define MPU6050_RA_I2C_SLV3_CTRL  	0x30
#define MPU6050_RA_I2C_SLV4_ADDR  	0x31
#define MPU6050_RA_I2C_SLV4_REG 	0x32
#define MPU6050_RA_I2C_SLV4_DO  	0x33
#define MPU6050_RA_I2C_SLV4_CTRL  	0x34
#define MPU6050_RA_I2C_SLV4_DI  	0x35
#define MPU6050_RA_I2C_MST_STATUS 	0x36
#define MPU6050_RA_INT_PIN_CFG  	0x37
#define MPU6050_RA_INT_ENABLE 		0x38
#define MPU6050_RA_DMP_INT_STATUS 	0x39
#define MPU6050_RA_INT_STATUS 		0x3A
#define MPU6050_RA_ACCEL_XOUT_H 	0x3B
#define MPU6050_RA_ACCEL_XOUT_L 	0x3C
#define MPU6050_RA_ACCEL_YOUT_H 	0x3D
#define MPU6050_RA_ACCEL_YOUT_L 	0x3E
#define MPU6050_RA_ACCEL_ZOUT_H 	0x3F
#define MPU6050_RA_ACCEL_ZOUT_L 	0x40
#define MPU6050_RA_TEMP_OUT_H 		0x41
#define MPU6050_RA_TEMP_OUT_L 		0x42
#define MPU6050_RA_GYRO_XOUT_H  	0x43
#define MPU6050_RA_GYRO_XOUT_L  	0x44
#define MPU6050_RA_GYRO_YOUT_H  	0x45
#define MPU6050_RA_GYRO_YOUT_L  	0x46
#define MPU6050_RA_GYRO_ZOUT_H  	0x47
#define MPU6050_RA_GYRO_ZOUT_L  	0x48
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
#define MPU6050_RA_I2C_SLV0_DO  	0x63
#define MPU6050_RA_I2C_SLV1_DO  	0x64
#define MPU6050_RA_I2C_SLV2_DO  	0x65
#define MPU6050_RA_I2C_SLV3_DO  	0x66
#define MPU6050_RA_I2C_MST_DELAY_CTRL 	0x67
#define MPU6050_RA_SIGNAL_PATH_RESET  	0x68
#define MPU6050_RA_MOT_DETECT_CTRL  	0x69
#define MPU6050_RA_USER_CTRL  		0x6A
#define MPU6050_RA_PWR_MGMT_1 		0x6B
#define MPU6050_RA_PWR_MGMT_2 		0x6C
#define MPU6050_RA_BANK_SEL 		0x6D
#define MPU6050_RA_MEM_START_ADDR 	0x6E
#define MPU6050_RA_MEM_R_W  		0x6F
#define MPU6050_RA_DMP_CFG_1  		0x70
#define MPU6050_RA_DMP_CFG_2  		0x71
#define MPU6050_RA_FIFO_COUNTH  	0x72
#define MPU6050_RA_FIFO_COUNTL  	0x73
#define MPU6050_RA_FIFO_R_W 		0x74
#define MPU6050_RA_WHO_AM_I 		0x75

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

/* Optional struct to store zero point calibration */
struct acc_zeropoint {
	/* raw acceleration zero offset */
	int16_t x_offset, y_offset, z_offset;

	/* raw gyrospope zero offset */
	int16_t ax_offset, ay_offset, az_offset;
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof(x[0]))
#endif

enum acc_result mpu6050_init(struct acc *dev, void (*delay_100ms)(void))
{
	struct {
		uint8_t reg;
		uint8_t value;
	} s[] = {
		{ MPU6050_RA_PWR_MGMT_1,	0x80 },
		{ MPU6050_RA_GYRO_CONFIG,	(uint8_t)dev->s.gyro_gain },
		{ MPU6050_RA_ACCEL_CONFIG,	(uint8_t)dev->s.acc_gain },
		{ MPU6050_RA_INT_PIN_CFG,	LATCH_INT_EN | INT_RD_CLEAR },
		{ MPU6050_RA_INT_ENABLE,	DATA_RDY_EN },
		{ MPU6050_RA_USER_CTRL,		0x00 },
		{ MPU6050_RA_PWR_MGMT_1,	0x00 },
		{ MPU6050_RA_PWR_MGMT_2,	0x00 },
		{ MPU6050_RA_FIFO_EN,		0x00 },
		{ MPU6050_RA_SMPLRT_DIV, 1000 / dev->s.sample_rate_hz - 1 },
		{ MPU6050_RA_CONFIG,		dev->s.filter_order & 0x07 },
	};

	uint8_t res, i, who_i_am;

	res = dev->i.rd(MPU6050_RA_WHO_AM_I, &who_i_am, sizeof(who_i_am));
	if (res)
		return MPU6050_INTERFACE_ERROR;

	who_i_am &= 0x7E;
	who_i_am >>= 1;
	if (who_i_am != 0x34)
		return MPU6050_ERROR_ID;

	res = dev->i.wr(MPU6050_RA_PWR_MGMT_1, 0x80);
	if (res)
		return MPU6050_INTERFACE_ERROR;

	/* 100ms delay if needed */
	if (delay_100ms)
		delay_100ms();

	for (i = 0; i != ARRAY_SIZE(s); i++) {
		res = dev->i.wr(s[i].reg, s[i].value);
		if (res)
		return MPU6050_INTERFACE_ERROR;
	}

	return MPU6050_SUCCESS;
}

enum acc_result mpu6050_get_result(struct acc *dev)
{
	if (dev->i.ready) {
		if (!dev->i.ready())
			return MPU6050_BUSY;
	} else {
		uint8_t status, res;
		res = dev->i.rd(MPU6050_RA_INT_STATUS, &status,
				  sizeof(status));
		if (res)
			return res;
		if (!(status & DATA_READY_BIT))
			return MPU6050_BUSY;
	}

	if (dev->i.rd(MPU6050_RA_INT_STATUS, (uint8_t*)&dev->raw,
			  sizeof(struct acc_measdata)))
		return MPU6050_INTERFACE_ERROR;

	if (dev->zero) {
		/* Perform zero offset for accelerometer data */
		dev->raw.x -= dev->zero->x_offset;
		dev->raw.y -= dev->zero->y_offset;
		dev->raw.z -= dev->zero->z_offset;

		/* Perform zero offset for gyroscope data */
		dev->raw.ax -= dev->zero->ax_offset;
		dev->raw.ay -= dev->zero->ay_offset;
		dev->raw.az -= dev->zero->az_offset;
	}

	if (dev->res) {
		float g = 32768.0;
		dev->res->temp =
			(float)dev->raw.temp / 340.0 + 36.53;

		switch(dev->s.acc_gain) {
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

		dev->res->x = (float)dev->raw.x / g;
		dev->res->y = (float)dev->raw.y / g;
		dev->res->z = (float)dev->raw.z / g;
	}

	return MPU6050_SUCCESS;
}

enum acc_result mpu6050_zero_cal(struct acc *dev)
{
	enum acc_result res;

	if (!dev->zero)
		return MPU6050_INVAL;

	/* Remove offsets */
	dev->zero->x_offset = 0;
	dev->zero->y_offset = 0;
	dev->zero->z_offset = 0;

	dev->zero->ax_offset = 0;
	dev->zero->ay_offset = 0;
	dev->zero->az_offset = 0;

	/* Update data */
	do {
		res = mpu6050_get_result(dev);
	} while(res == MPU6050_BUSY);

	if (res)
		return res;

	/* Allign accel */
	dev->zero->x_offset = dev->raw.x;
	dev->zero->y_offset = dev->raw.y;
	dev->zero->z_offset = dev->raw.z;

	/* Allign gyro */
	dev->zero->ax_offset = dev->raw.ax;
	dev->zero->ay_offset = dev->raw.ay;
	dev->zero->az_offset = dev->raw.az;

	return MPU6050_SUCCESS;
}
