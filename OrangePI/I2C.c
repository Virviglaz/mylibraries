#include "I2C.h"

I2C_ErrorTypeDef I2C_Read (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	I2C_ErrorTypeDef result = READERROR;

	int handler;
	
	if (driver == NULL) return NODRIVER;
	
	handler = open(driver, O_RDWR);
	
	if (ioctl(handler, I2C_SLAVE, addr) > 0)
	{
		if (write(handler, reg, reglen) == reglen)
			if (read(handler, buf, size) == size)
				result = SUCCESS;
	}
	else
		result = NOTFOUND;

	close(handler);
	
	return result;
}

I2C_ErrorTypeDef I2C_Write (char * driver, uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	I2C_ErrorTypeDef result = WRITEERROR;

	int handler;
	
	if (driver == NULL) return NODRIVER;
	
	handler = open(driver, O_RDWR);
	
	if (ioctl(handler, I2C_SLAVE, addr) > 0)
	{
		if (write(handler, reg, reglen) == reglen)
			if (write(handler, buf, size) == size)
				result = SUCCESS;
	}
	else
		result = NOTFOUND;

	close(handler);
	
	return result;
}
