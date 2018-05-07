#include "I2C.h"

static int i2c_handler = 0;

I2C_ErrorTypeDef I2C_Init (const char * driver)
{
	if (driver == NULL) return NODRIVER;
	
	if (i2c_handler) return ALREADYOPENED;
	
	if ((i2c_handler = open(driver, O_RDWR)) < 0) return OPENFAILED;
	
	return SUCCESS;
}

I2C_ErrorTypeDef I2C_Read (uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	if (i2c_handler == 0) return NODRIVER;
	
	if (ioctl(i2c_handler, I2C_SLAVE, addr) < 0)
		return NOACCESS;
	
	if (write(i2c_handler, reg, reglen) != reglen)
		return WRITEERROR;
	
	if (read(i2c_handler, buf, size) != size)
		return READERROR;
	
	return SUCCESS;
}

I2C_ErrorTypeDef I2C_Write (uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	if (i2c_handler == 0) return NODRIVER;
	
	if (ioctl(i2c_handler, I2C_SLAVE, addr) < 0)
		return NOACCESS;
	
	if (write(i2c_handler, reg, reglen) != reglen)
		return WRITEERROR;
	
	if (write(i2c_handler, buf, size) != size)
		return WRITEERROR;
	
	return SUCCESS;
}
