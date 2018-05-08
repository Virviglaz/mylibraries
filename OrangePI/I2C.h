#ifndef _ORANGEPI_I2C_H_
#define _ORANGEPI_I2C_H_

#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

typedef enum
{
	SUCCESS = 0,
	READERROR,
	WRITEERROR,
	NODRIVER,
	NOTFOUND
}I2C_ErrorTypeDef;

I2C_ErrorTypeDef I2C_Read (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
I2C_ErrorTypeDef I2C_Write (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);

static const struct
{
	I2C_ErrorTypeDef (* Read) (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
	I2C_ErrorTypeDef (* Write) (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
}I2C = {I2C_Read, I2C_Write};

#endif
