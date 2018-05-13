#include "I2C.h"

I2C_ErrorTypeDef i2c_read (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	I2C_ErrorTypeDef result;
	int file;
	struct i2c_msg messages[2] = {{ addr, 0, reglen, reg }, { addr, I2C_M_RD, size, buf }};
	struct i2c_rdwr_ioctl_data packets = { messages, 2 };

	if (driver == NULL) return NODRIVER;
	file = open(driver, O_RDWR);
	if (file < 0) return WRONGDRIVER;
	result = (ioctl(file, I2C_RDWR, &packets) < 0) ? READERROR : SUCCESS;

	close(file);
	return result;
}

I2C_ErrorTypeDef i2c_write (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	I2C_ErrorTypeDef result;
	int file;
	struct i2c_msg messages[1] = {{ addr, 0, reglen, reg }};
	struct i2c_rdwr_ioctl_data packets = { messages, 1 };

	if (driver == NULL) return NODRIVER;
	file = open(driver, O_WRONLY);
	if (file < 0) return WRONGDRIVER;

	if (buf && size)
	{
		uint8_t * fullbuf = malloc(size + reglen);
		memcpy(fullbuf, reg, reglen);
		memcpy(fullbuf + reglen, buf, size);
		messages[0].buf = fullbuf;
		messages[0].len = reglen + size;
		result = (ioctl(file, I2C_RDWR, &packets) < 0) ? WRITEERROR : SUCCESS;
		free(fullbuf);
	}
	else
		result = (ioctl(file, I2C_RDWR, &packets) < 0) ? READERROR : SUCCESS;

	close(file);
	return result;
}


I2C_ErrorTypeDef i2c_write2 (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	I2C_ErrorTypeDef result = WRITEERROR;
	int file;

	if (driver == NULL) return NODRIVER;

	file = open(driver, O_RDWR);

	if (file < 0) return WRONGDRIVER;

	if (ioctl(file, I2C_SLAVE, addr) >= 0)
		{
			if (write(file, reg, reglen) == reglen)
				if (write(file, buf, size) == size)
					result = SUCCESS;
		}
		else
			result = NOTFOUND;

	close(file);

	return result;
}

char * i2c_GetError (I2C_ErrorTypeDef errornum)
{
	const char * errors[] = {"Success", "Reading error", "Writing error",
			"No drives", "Driver name wrong", "No devices found"};
	return (char*)errors[(uint8_t)errornum];
}


