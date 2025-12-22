#include "gpio.h"

/* Timing definition */
#define Time_Pulse_Delay_Low 10
#define Time_Pulse_Delay_High 40
#define Time_Reset_Low 500
#define Time_Before_Reset 500
#define Time_After_Reset 350
#define Time_Hold_Down 10

/* Error handling */
typedef enum
{
	One_Wire_Success = 0x00,
	One_Wire_Error_No_Echo = 0x01,
	One_Wire_Bus_Low_Error = 0x02,
	One_Wire_Device_Busy = 0x03,
	One_Wire_CRC_Error = 0x04
} One_Wire_ErrorTypeDef;

/* 1-Wire low level definitions */
typedef struct
{
#ifdef DoubleWire
	/* TX pin definition */
	GPIO_TypeDef *GPIO_Pin_Tx;
	unsigned int PIN_Tx;

	/* RX pin definition */
	GPIO_TypeDef *GPIO_Pin_Rx;
	unsigned int PIN_Rx;
#else
	/* Single pin definition */
	GPIO_TypeDef *GPIO_Pin;
	unsigned int PIN;
#endif
	void (*Delay_Func)(unsigned int us);
} One_Wire_StructTypeDef;

One_Wire_ErrorTypeDef One_Wire_Reset(One_Wire_StructTypeDef *One_Wire_Bus);
void One_Wire_Write_Bit(One_Wire_StructTypeDef *One_Wire_Bus, unsigned char Bit);
void One_Wire_Write_Byte(One_Wire_StructTypeDef *One_Wire_Bus, unsigned char Byte);
unsigned char One_Wire_Read_Bit(One_Wire_StructTypeDef *One_Wire_Bus);
unsigned char One_Wire_Read_Byte(One_Wire_StructTypeDef *One_Wire_Bus);
