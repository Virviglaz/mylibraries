#ifndef DS2482_H
#define DS2482_H

#include <stdint.h>

#define DS2482_ConfigReg	0xC3
#define DS2482_BusReset		0xB4
#define DS2482_Status			0xF0
#define DS2482_DataReg		0xE1
#define DS2482_DRST				0xF0
#define DS2482_WCFG				0xD2
#define DS2482_1WWB				0xA5
#define DS2482_1WRB				0x96

// Status register bit definitions
#define Status_1WB				0x01
#define Status_PPD				0x02
#define Status_SD					0x04
#define Status_LL					0x08
#define Status_RST				0x10
#define Status_SBR				0x20
#define Status_TSB				0x40
#define Status_DIR				0x80

typedef enum
{
	Active_PullUp_Enabled = 0x01,
	Active_PullUp_Disable = 0x10,
}Active_PullUpTypeDef;

typedef enum
{
	Strong_PullUp_Enabled = 0x04,
	Strong_PullUp_Disable = 0x40,
}Strong_PullUpTypeDef;

typedef enum
{
	BusSpeed_Fast 	= 0x08,
	BusSpeed_Normal = 0x80,
}OneWireBusSpeedTypeDef;

/* Error handling */
typedef enum
{
	One_Wire_Success					=	0x00,
	One_Wire_Error_No_Echo		=	0x01,
	One_Wire_Bus_Low_Error		=	0x02,	
	One_Wire_Device_Busy 			= 0x03,	
	One_Wire_CRC_Error				=	0x04,
	One_Wire_InterfaceError		= 0x05,
}OneWireErrorTypeDef;

typedef struct
{
	/* Device configuration */
	Active_PullUpTypeDef Active_PullUp;
	Strong_PullUpTypeDef Strong_PullUp;
	OneWireBusSpeedTypeDef BusSpeed;
	uint8_t ReadAttemps; //10..100

	/* I2C Interface Functions */
	uint8_t (*Write)(uint8_t reg, uint8_t * buf, uint8_t size);
	uint8_t (*Read) (uint8_t * reg, uint8_t regsize, uint8_t * buf, uint8_t size);
	uint8_t (*WriteWithFlagPooling) (uint8_t reg, uint8_t * value, uint8_t attempts, uint8_t flagToPooling);
}DS2482_StructTypeDef;

OneWireErrorTypeDef DS2482_Init (DS2482_StructTypeDef * DS2482_InitStruct);
OneWireErrorTypeDef One_Wire_Reset (DS2482_StructTypeDef * DS2482_InitStruct);
OneWireErrorTypeDef One_Wire_WriteByte (DS2482_StructTypeDef * DS2482_InitStruct, uint8_t value);
uint8_t One_Wire_ReadByte (DS2482_StructTypeDef * DS2482_InitStruct);
#endif
