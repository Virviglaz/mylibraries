#include <stm32f10x.h>

typedef enum
{
	SPI_BaudRate_2       	=  0x0000,
	SPI_BaudRate_4       	=  0x0008,
	SPI_BaudRate_8       	=  0x0010,
	SPI_BaudRate_16      	=  0x0018,
	SPI_BaudRate_32      	=  0x0020,
	SPI_BaudRate_64      	=  0x0028,
	SPI_BaudRate_128     	=  0x0030,
	SPI_BaudRate_256     	=  0x0038
}SPI_BaudRateTypeDef;

typedef enum
{
	SPI_CLK_IDLE_LOW 			= 0x00,
	SPI_CLK_IDLE_HIGH			=	0x03
}SPI_CLK_IDLE_TypeDef;

typedef struct
{
	SPI_TypeDef * SPIx;
	FunctionalState SPI_Enable;
	SPI_BaudRateTypeDef SPI_BaudRate;
	SPI_CLK_IDLE_TypeDef SPI_CLK_Idle;
	IRQn_Type InterruptVector;
	volatile char result;
	volatile unsigned short TimeOut;
	
	volatile char * buffer;
	unsigned short size;
	volatile unsigned short i;
	void (*SPI_IT_Handler) (void);
}SPI_IT_InitStructTypeDef;

void SPI_IT_Init (SPI_IT_InitStructTypeDef * SPI_IT_InitStruct);
void SPI_IT_Handler (SPI_IT_InitStructTypeDef * SPI_IT_InitStruct);
char SPI_IT_RW (SPI_IT_InitStructTypeDef * SPI_IT_InitStruct, GPIO_TypeDef * GPIOx, u16 PINx, char reg, char * buffer, unsigned short size);
