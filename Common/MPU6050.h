#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

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
struct /*__attribute__((__packed__))*/ mpu_measdata {
	uint8_t status;

	/* raw acceleration data */
	int16_t x, y, z;

	/* Temperature Data */	
	int16_t temp;

	/* Gyro Data */
	int16_t ax, ay, az;
};

/* Optional struct to store zero point calibration */
struct mpu_zeropoint {
	/* raw acceleration zero offset */
	int16_t x_offset, y_offset, z_offset;
	
	/* raw gyrospope zero offset */
	int16_t ax_offset, ay_offset, az_offset;	
};

struct mpu_real_values {
	float x, y, z;
	float temp;
};

struct mpu_conf {
	/* Interface functions */
	uint8_t (*write_reg)(uint8_t reg, uint8_t value);
	uint8_t (*read_reg) (uint8_t reg, uint8_t *buf, uint16_t size);
	uint16_t (*check_ready_pin)(void);
	
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
