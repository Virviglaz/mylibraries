#ifndef LM75_H
#define LM75_H

typedef struct
{
	/* Variables */
	float T;
	char I2C_Adrs;
	
	/* Functions */
	char (*WriteReg)(char I2C_Adrs, char Reg, char Value);
	char (*ReadReg) (char I2C_Adrs, char Reg, char * buf, char size);
	
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
	char (* Power) 			(LM75_StructTypeDef * LM75_Struct, LM75_PowerTypeDef Enable);
	char (* GetResult)	(LM75_StructTypeDef * LM75_Struct);
}LM75_ClassTypeDef;

char LM75_PowerOn (LM75_StructTypeDef * LM75_Struct, LM75_PowerTypeDef Enable);
char LM75_GetResult (LM75_StructTypeDef * LM75_Struct);

static const LM75_ClassTypeDef LM75 = {LM75_PowerOn, LM75_GetResult};
#endif
