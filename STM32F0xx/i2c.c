#include "i2c.h"

I2C_ErrorTypeDef I2c_Write (I2C_TypeDef * I2Cx, uint8_t I2C_BusAddress, 
			uint8_t * Address, uint8_t AddressLen, uint8_t * Data, uint8_t DataLen)
{
	uint32_t temp;
	
	//Wait until I2C isn't busy
	while (I2Cx->ISR & I2C_ISR_BUSY);
	
	temp = I2Cx->CR2;
	temp &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	temp |= (I2C_BusAddress & I2C_CR2_SADD) | ((AddressLen << 16) & I2C_CR2_NBYTES) | I2C_CR2_RELOAD | I2C_CR2_START;
	I2Cx->CR2 = temp;
	
	while (AddressLen--)
	{
		while (!(I2Cx->ISR & I2C_ISR_TXIS));
		I2Cx->TXDR = (uint8_t) * Address++;
	}

	//Ensure that the transfer complete reload flag is set, essentially a standard TC flag	
	while (!(I2Cx->ISR & I2C_ISR_TCR));
	
	temp = I2Cx->CR2;
	temp &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	temp |= (I2C_BusAddress & I2C_CR2_SADD) | ((DataLen << 16) & I2C_CR2_NBYTES) | I2C_CR2_AUTOEND;
	I2Cx->CR2 = temp;	

	while (DataLen--)
	{
		while (!(I2Cx->ISR & I2C_ISR_TXIS));
		I2Cx->TXDR = (uint8_t) * Address++;
	}
	
	while (!(I2Cx->ISR & I2C_ISR_STOPF));
	
	I2Cx->ICR = I2C_ISR_STOPF;
	
	return I2C_OK;
}
