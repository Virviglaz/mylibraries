#ifndef _ORANGEPI_I2C_H_
#define _ORANGEPI_I2C_H_

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

typedef enum
{
	SUCCESS = 0,
	READERROR,
	WRITEERROR,
	NODRIVER,
	WRONGDRIVER,
	NOTFOUND
}I2C_ErrorTypeDef;

I2C_ErrorTypeDef i2c_read  (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
I2C_ErrorTypeDef i2c_write (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
char * i2c_GetError (I2C_ErrorTypeDef errornum);


static const struct
{
	I2C_ErrorTypeDef (* Read) (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
	I2C_ErrorTypeDef (* Write) (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
	char * (* GetError) (I2C_ErrorTypeDef errornum);
}I2C = { i2c_read, i2c_write, i2c_GetError };

#endif
