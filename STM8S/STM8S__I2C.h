#include "stm8s.h"

#define ACK     0
#define NACK    1

typedef enum {
    I2C_SUCCESS = 0,
    I2C_TIMEOUT,
    I2C_ERROR,
    I2C_BUS_BUSY,
    I2C_ACK_OK,
    I2C_ACK_NOT_OK,
    I2C_ADD_NOT_EXIST
} I2C_Result;

void I2C__Init (unsigned long F_OSC_hz, unsigned long F_i2c_hz);
unsigned char I2c_Read_Page_1 (unsigned char I2c_Address, unsigned char Mem_Address, unsigned char * pBuffer, unsigned char pBufferSize);
unsigned char I2c_Write_Register (unsigned char I2c_Address, unsigned char Reg, unsigned char data);