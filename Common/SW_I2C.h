#ifndef SW_I2C_H
#define SW_I2C_H

#include "GPIO.h"
#include <stdint.h>

typedef struct
{
  /* Functions */
	void (* delay_func) (uint16_t ms);
	
  /* GPIO for I2C pins */
  GPIO_TypeDef * SDA_GPIO;
  GPIO_TypeDef * SCL_GPIO;
  uint16_t SDA_PIN;
  uint16_t SCL_PIN;
  
  /* Delay counter value */
  uint16_t DelayValue;
  
  /* I2C address, Register or memory pointer define */
  uint8_t I2C_Address;
  
  /* Register address */
  uint8_t * Reg_Address;
	
  /* Register address lenght of pointer or Register address depends of reading sequence choosed */
  uint8_t Reg_AddressOrLen;
  
  uint8_t * pBuffer;
  uint8_t pBufferSize;
}I2C_SW_InitStructTypeDef;

typedef enum {
    I2C_SUCCESS = 0,
    I2C_TIMEOUT,
    I2C_ERROR,
    I2C_BUS_BUSY,
    I2C_ACK_OK,
    I2C_ACK_NOT_OK,
    I2C_ADD_NOT_EXIST,
		I2C_VERIFY_ERROR
} I2C_Result;

I2C_Result I2C__Start (I2C_SW_InitStructTypeDef * I2C_Struct);
void I2C__ReStart (I2C_SW_InitStructTypeDef * I2C_Struct);
void I2C__Stop (I2C_SW_InitStructTypeDef * I2C_Struct);
uint16_t I2C__Clock(I2C_SW_InitStructTypeDef * I2C_Struct);
uint16_t I2C__Write(I2C_SW_InitStructTypeDef * I2C_Struct, uint8_t data);
uint16_t I2C__Read (I2C_SW_InitStructTypeDef * I2C_Struct, uint8_t Acknowledgment);

I2C_Result SW_I2C_Read_Reg 		(I2C_SW_InitStructTypeDef * I2C_Struct);
I2C_Result SW_I2C_Read_Page 	(I2C_SW_InitStructTypeDef * I2C_Struct);
I2C_Result SW_I2C_Write_Reg 	(I2C_SW_InitStructTypeDef * I2C_Struct);
I2C_Result SW_I2C_Write_Page	(I2C_SW_InitStructTypeDef * I2C_Struct);
I2C_Result SW_I2C_Write_Byte (I2C_SW_InitStructTypeDef * I2C_Struct, unsigned char Reg, unsigned char Value);
I2C_Result SW_I2C_Check_Bus (I2C_SW_InitStructTypeDef * I2C_Struct);

#endif
