#ifndef LM75_H
#define LM75_H

#include <stdint.h>

typedef struct
{
	/* Variables */
	float T;
	uint8_t I2C_Adrs;
	
	/* Functions */
	uint8_t (*WriteReg)(uint8_t I2C_Adrs, uint8_t Reg, uint8_t Value);
	uint8_t (*ReadReg) (uint8_t I2C_Adrs, uint8_t Reg, uint8_t * buf, uint8_t size);
	
}LM75_StructTypeDef;

typedef enum
{
	LM75_TempReg = 0,
	LM75_ConfigReg = 1,
	LM75_Thyst = 2,
	LM75_Tos = 3
}LM75_RegTypeDef;

typedef enum
{
	LM75_Run = 0,
	LM75_Idle = 1
}LM75_PowerTypeDef;

typedef struct
{
	uint8_t (* Power) 			(LM75_StructTypeDef * LM75_Struct, LM75_PowerTypeDef Enable);
	uint8_t (* GetResult)	(LM75_StructTypeDef * LM75_Struct);
}LM75_ClassTypeDef;

uint8_t LM75_PowerOn (LM75_StructTypeDef * LM75_Struct, LM75_PowerTypeDef Enable);
uint8_t LM75_GetResult (LM75_StructTypeDef * LM75_Struct);

static const LM75_ClassTypeDef LM75 = {LM75_PowerOn, LM75_GetResult};
#endif
