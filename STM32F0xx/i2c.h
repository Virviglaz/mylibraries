#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "stm32f0xx.h"                  // Device header


typedef enum
{
	I2C_OK,
	I2C_NOACK,
	I2C_BUSBUSY
}I2C_ErrorTypeDef;

#endif
