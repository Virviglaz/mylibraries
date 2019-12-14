#ifndef I2C_DEV_H
#define I2C_DEV_H

#include <stdint.h>
#include <stdbool.h>

struct i2c_dev {
	const char *name;
	int fd;

	/* Public functions */
	uint8_t(*wr_reg)(struct i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size);
	uint8_t(*rd_reg)(struct i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size);
};

int i2c_init(struct i2c_dev *dev, const char *name);
void i2c_close(struct i2c_dev *dev);

#endif // !I2C_DEV_H