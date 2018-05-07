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
	OPENFAILED,
	NOACCESS,
	READERROR,
	NOTOPENED,
	ALREADYOPENED,
	NODRIVER
}I2C_ErrorTypeDef;

I2C_ErrorTypeDef I2C_Init (const char * driver);
I2C_ErrorTypeDef I2C_Read (uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
I2C_ErrorTypeDef I2C_Write (uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);

#endif
