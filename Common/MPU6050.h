/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020 Pavel Nadein
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

#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

#define MPU6050_SUCCESS				0
#define MPU6050_BUSY				1
#define MPU6050_INTERFACE_ERROR			2
#define MPU6050_DEFAULT_I2C_ADDRESS		0x68

enum mpu_gyro {
	GYRO_0250DS = 0x00,
	GYRO_0500DS = 0x08,
	GYRO_1000DS = 0x10,
	GYRO_2000DS = 0x18,
};

enum mpu_acc {
	SCALE_2G  = 0x00,
	SCALE_4G  = 0x08,
	SCALE_8G  = 0x10,
	SCALE_16G = 0x18
};

/* Raw measurements results */
#ifdef __ICCSTM8__
struct mpu_measdata {
#else
struct __attribute__((__packed__)) mpu_measdata {
#endif
	/* raw acceleration data */
	int16_t x; /* MPU6050_RA_ACCEL_XOUT H/L */
	int16_t y; /* MPU6050_RA_ACCEL_YOUT H/L */
	int16_t z; /* MPU6050_RA_ACCEL_ZOUT H/L */

	/* Temperature Data */
	int16_t temp; /* MPU6050_RA_TEMP_OUT H/L */

	/* Gyro Data */
	int16_t ax; /* MPU6050_RA_GYRO_XOUT H/L */
	int16_t ay; /* MPU6050_RA_GYRO_YOUT H/L */
	int16_t az; /* MPU6050_RA_GYRO_ZOUT H/L */
};

/* Optional struct to store zero point calibration */
struct mpu_zeropoint {
	/* raw acceleration zero offset */
	int16_t x_offset, y_offset, z_offset;

	/* raw gyrospope zero offset */
	int16_t ax_offset, ay_offset, az_offset;
};

struct mpu_real_values {
	double x, y, z;
	double temp;
};

struct mpu_conf {
	/* Interface functions */
	uint8_t (*write_reg)(uint8_t reg, uint8_t value);
	uint8_t (*read_reg) (uint8_t reg, uint8_t *buf, uint16_t size);
	uint16_t (*check_ready_pin)(void); /* for external interrupt */

	/* Raw measurement data */
	struct mpu_measdata raw_result;

	/* Optional acceleration & gyro zero point calibration */
	struct mpu_zeropoint *zero_point;

	/* Optional real values storing struct */
	struct mpu_real_values *real_values;

	/* Configuration */
	uint8_t sample_rate_hz;
	enum mpu_gyro gyro_scale;
	enum mpu_acc acc_scale;
	uint8_t filter_order;
};


uint8_t mpu6050_init(struct mpu_conf *, void (*)(void));
uint8_t mpu6050_get_result(struct mpu_conf *);
void mpu6050_zero_cal(struct mpu_conf *);

#endif /* MPU6050_H */
