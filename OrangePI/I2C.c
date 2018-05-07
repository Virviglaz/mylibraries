#include "I2C.h"

static char * i2c_driver = null;

I2C_ErrorTypeDef I2C_Init (const char * driver)
{
	if (driver == null) return NODRIVER;
	
	if (i2c_driver) return ALREADYOPENED;
	
	if (open(driver, O_RDWR) < 0) return OPENFAILED;
	
	i2c_driver = driver;
	
	return SUCCESS;
}

I2C_ErrorTypeDef I2C_Read (uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	if (i2c_driver == null) return NODRIVER;
	
	if (ioctl(i2c_driver, I2C_SLAVE, addr) < 0)
		return NOACCESS;
	
	if (write(i2c_driver, reg, reglen) != reglen)
		return READERROR;
	
	if (read(i2c_driver, buf, size) != size)
		return READERROR;
	
	return SUCCESS;
}

I2C_ErrorTypeDef I2C_Write (uint8_t addr, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size)
{
	if (i2c_driver == null) return NODRIVER;
	
	if (ioctl(i2c_driver, I2C_SLAVE, addr) < 0)
		return NOACCESS;
	
	if (write(i2c_driver, reg, reglen) != reglen)
		return READERROR;
	
	if (write(i2c_driver, buf, size) != size)
		return READERROR;
	
	return SUCCESS;
}
