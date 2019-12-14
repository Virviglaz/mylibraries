#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif
#include <errno.h>
#include <string.h>
#include "i2c.h"

static struct i2c_dev *last = NULL;

static uint8_t write_reg(struct i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
	uint8_t buf[size + 1];
	struct i2c_msg msg[] = {{ .addr = addr, .flags = 0, .len = size + 1, .buf = buf}};
	struct i2c_rdwr_ioctl_data msgset[1] = {{ .msgs = msg, .nmsgs = 1 }};

	buf[0] = reg;
	memcpy(buf + 1, data, size);
	return ioctl(dev ? dev->fd : last->fd, I2C_RDWR, &msgset) < 0 ? 1 : 0;
}

static uint8_t read_reg(struct i2c_dev *dev, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t size)
{
	struct i2c_msg msg[] = {
		{ addr, 0, 1, &reg },
		{ addr, I2C_M_RD | I2C_M_NOSTART, size, data }};
	struct i2c_rdwr_ioctl_data msgset[1] = {{ .msgs = msg, .nmsgs = 2 }};

	return ioctl(dev ? dev->fd : last->fd, I2C_RDWR, &msgset) < 0 ? 1 : 0;
}

int i2c_init(struct i2c_dev *dev, const char *name)
{
	last = dev;
	
	dev->name = name ? name : "/dev/i2c-0";
	dev->fd = open(dev->name, O_RDWR);
	if (dev->fd < 0)
		return -ENODEV;

	dev->wr_reg = write_reg;
	dev->rd_reg = read_reg;
	
	return dev->fd < 0 ? dev->fd : 0;
}

void i2c_close(struct i2c_dev *dev)
{
	close(dev->fd);
}