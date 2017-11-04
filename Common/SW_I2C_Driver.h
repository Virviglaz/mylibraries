#ifndef SW_I2C_DRIVER_H
#define SW_I2C_DRIVER_H

#include <stdint.h>

typedef struct
{
  /* Delay counter value */
  uint16_t DelayValue;
	
  /* Functions */
	void (* Delay_func) (uint16_t ms);
	void (* IO_SCL_Write)(uint8_t state);
	void (* IO_SDA_Write)(uint8_t state);
	uint16_t (* IO_SDA_Read) (void);
	
}SW_I2C_DriverStructTypeDef;

typedef enum {
    I2C_SUCCESS = 0,
    I2C_TIMEOUT,
    I2C_ERROR,
    I2C_BUS_BUSY,
    I2C_ACK_OK,
    I2C_ACK_NOT_OK,
    I2C_ADD_NOT_EXIST,
		I2C_VERIFY_ERROR,
		I2C_INTERFACE_ERROR,
} I2C_Result;

I2C_Result SW_I2C_ASSIGN (SW_I2C_DriverStructTypeDef * SW_I2C_DriverToAssign);
SW_I2C_DriverStructTypeDef * SW_I2C_GetDriver (void);
void SW_I2C_RESET_BUS (void);
I2C_Result SW_I2C_WR (uint8_t address, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
I2C_Result SW_I2C_RD (uint8_t address, uint8_t * reg, uint8_t reglen, uint8_t * buf, uint16_t size);
uint8_t SW_I2C_WriteWithFlagPooling (uint8_t address, uint8_t reg, uint8_t * value, uint8_t attempts, uint8_t flagToPooling);
#endif
