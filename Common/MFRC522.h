#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

typedef enum
{
	RX_Gain18dB = 0x20,
	RX_Gain23dB = 0x30,
	RX_Gain33dB = 0x40,
	RX_Gain38dB = 0x50,
	RX_Gain43dB = 0x60,
	RX_Gain48dB = 0x70,
	RX_Gain
}MFRC522_RXGainTypeDef;

typedef struct
{
	/* Settings */
	MFRC522_RXGainTypeDef MFRC522_RXGain;
	
	/* Functions */
	void (*WriteData)(char reg, char * buf, char size);
	void (*ReadData) (char reg, char * buf, char size);
	void (*Delay_Func)(uint32_t us);
} MFRC522_StructTypeDef;

typedef enum
{
	MI_OK,
	MI_NOTAGERR,
	MI_ERR,
	MI_NOCARD,
	MI_COLLISION,
	MI_WRONG_CARD,
	MI_AUTH_ERROR,
	MI_READ_ERROR,
	MI_WRITE_ERROR,
	MI_WRONG_DATA,
	MI_WRONG_PARAM,
}MFRC522_StatusTypeDef;

typedef enum
{
	MifareErr = 0x00,
	Mifare_1k = 0x08,
	Mifare_4k = 0x18,
}MFRC522_CardTypeTypeDef;

typedef enum
{
	PCD_IDLE				=											0x00,   // NO action; Cancel the current command
	PCD_AUTHENT 		=											0x0E,   // Authentication Key	
	PCD_RECEIVE			=											0x08,   // Receive Data	
	PCD_TRANSMIT		=											0x04,   // Transmit data	
	PCD_TRANSCEIVE	=											0x0C,   // Transmit and receive data,	
	PCD_RESETPHASE	=											0x0F,   // Reset	
	PCD_CALCCRC			=											0x03,   // CRC Calculate	
}MFRC522_CommandsTypeDef;

typedef enum
{
	PICC_AUTHENT1A = 0x60,
	PICC_AUTHENT1B = 0x61,
}MFRC522_AuthTypeDef;

typedef enum
{
	CARD_READ,
	CARD_WRITE,
	CARD_RW,
	CARD_RW_ANYWAY,
}MFRC522_ToDoTypeDef;

// Mifare_One card command word
#define PICC_REQIDL											0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL											0x52   // find all the cards antenna area
#define PICC_ANTICOLL										0x93   // anti-collision
#define PICC_SElECTTAG									0x93   // election card
#define PICC_READ												0x30   // Read Block
#define PICC_WRITE											0xA0   // write block
#define PICC_DECREMENT									0xC0   // debit
#define PICC_INCREMENT									0xC1   // recharge
#define PICC_RESTORE										0xC2   // transfer block data to the buffer
#define PICC_TRANSFER										0xB0   // save the data in the buffer
#define PICC_HALT												0x50   // Sleep

// MFRC522 Registers
typedef enum
{
	 MFRC522_REG_RESERVED00						=	0x00,
	 MFRC522_REG_COMMAND							=	0x01,
	 MFRC522_REG_COMM_IE_N						=	0x02,
	 MFRC522_REG_DIV1_EN							=	0x03,
	 MFRC522_REG_COMM_IRQ							=	0x04,
	 MFRC522_REG_DIV_IRQ							=	0x05,
	 MFRC522_REG_ERROR								=	0x06,
	 MFRC522_REG_STATUS1							=	0x07,
	 MFRC522_REG_STATUS2							=	0x08,
	 MFRC522_REG_FIFO_DATA						=	0x09,
	 MFRC522_REG_FIFO_LEVEL						=	0x0A,
	 MFRC522_REG_WATER_LEVEL					=	0x0B,
	 MFRC522_REG_CONTROL							=	0x0C,
	 MFRC522_REG_BIT_FRAMING					=	0x0D,
	 MFRC522_REG_COLL									=	0x0E,
	 MFRC522_REG_RESERVED01						=	0x0F,
	 // Page 1: Command
	 MFRC522_REG_RESERVED10						=	0x10,
	 MFRC522_REG_MODE									=	0x11,
	 MFRC522_REG_TX_MODE							=	0x12,
	 MFRC522_REG_RX_MODE							=	0x13,
	 MFRC522_REG_TX_CONTROL						=	0x14,
	 MFRC522_REG_TX_AUTO							=	0x15,
	 MFRC522_REG_TX_SELL							=	0x16,
	 MFRC522_REG_RX_SELL							=	0x17,
	 MFRC522_REG_RX_THRESHOLD					=	0x18,
	 MFRC522_REG_DEMOD								=	0x19,
	 MFRC522_REG_RESERVED11						=	0x1A,
	 MFRC522_REG_RESERVED12						=	0x1B,
	 MFRC522_REG_MIFARE								=	0x1C,
	 MFRC522_REG_RESERVED13						=	0x1D,
	 MFRC522_REG_RESERVED14						=	0x1E,
	 MFRC522_REG_SERIALSPEED					=	0x1F,
	 // Page 2: CFG
	 MFRC522_REG_RESERVED20						=	0x20,
	 MFRC522_REG_CRC_RESULT_M					=	0x21,
	 MFRC522_REG_CRC_RESULT_L					=	0x22,
	 MFRC522_REG_RESERVED21						=	0x23,
	 MFRC522_REG_MOD_WIDTH						=	0x24,
	 MFRC522_REG_RESERVED22						=	0x25,
	 MFRC522_REG_RF_CFG								=	0x26,
	 MFRC522_REG_GS_N									=	0x27,
	 MFRC522_REG_CWGS_PREG						=	0x28,
	 MFRC522_REG__MODGS_PREG					=	0x29,
	 MFRC522_REG_T_MODE								=	0x2A,
	 MFRC522_REG_T_PRESCALER					=	0x2B,
	 MFRC522_REG_T_RELOAD_H						=	0x2C,
	 MFRC522_REG_T_RELOAD_L						=	0x2D,
	 MFRC522_REG_T_COUNTER_VALUE_H		=	0x2E,
	 MFRC522_REG_T_COUNTER_VALUE_L		=	0x2F,
	 // Page 3:TestRegister
	 MFRC522_REG_RESERVED30						=	0x30,
	 MFRC522_REG_TEST_SEL1						=	0x31,
	 MFRC522_REG_TEST_SEL2						=	0x32,
	 MFRC522_REG_TEST_PIN_EN					=	0x33,
	 MFRC522_REG_TEST_PIN_VALUE				=	0x34,
	 MFRC522_REG_TEST_BUS							=	0x35,
	 MFRC522_REG_AUTO_TEST						=	0x36,
	 MFRC522_REG_VERSION							=	0x37,
	 MFRC522_REG_ANALOG_TEST					=	0x38,
	 MFRC522_REG_TEST_ADC1						=	0x39,
	 MFRC522_REG_TEST_ADC2						=	0x3A,
	 MFRC522_REG_TEST_ADC0						=	0x3B,
	 MFRC522_REG_RESERVED31						=	0x3C,
	 MFRC522_REG_RESERVED32						=	0x3D,
	 MFRC522_REG_RESERVED33						=	0x3E,
	 MFRC522_REG_RESERVED34						=	0x3F,
}MRFC522_RegTypeDef;

#define MFRC522_DUMMY										0x00			// Dummy byte
#define MFRC522_MAX_LEN									16				// Buf len byte

typedef struct
{
	MFRC522_StatusTypeDef (* Check)			(uint8_t * id);
	MFRC522_StatusTypeDef (* Compare)		(uint8_t * CardID, uint8_t * CompareID);
	void (* Init)					(MFRC522_StructTypeDef * InitStruct);
	MFRC522_StatusTypeDef (* Operate32) (uint8_t * SN, uint8_t Sector,
									uint8_t * KEY, void (*KeyGenFunc)(uint8_t * sn, uint8_t * key), void (*EvenHandler)(uint8_t * SN, uint32_t * data), MFRC522_ToDoTypeDef Task);
	MFRC522_StatusTypeDef (* Operate) (uint8_t * SN, uint8_t Sector,
									uint8_t * KEY, void (*KeyGenFunc)(uint8_t * sn, uint8_t * key), void (*EvenHandler)(uint8_t * SN, uint8_t * Value), MFRC522_ToDoTypeDef Task);
	void (* AntennaOn)	(void);
	void (* AntennaOff)	(void);
	void (* Reset) 			(void);
	MFRC522_StatusTypeDef (* Request)			(uint8_t reqMode, uint8_t * TagType);
	MFRC522_StatusTypeDef (* ToCard)			(MFRC522_CommandsTypeDef command, uint8_t * sendData, uint8_t sendLen, uint8_t * backData, uint16_t * backLen);
	MFRC522_StatusTypeDef (* Anticoll)		(uint8_t * serNum);
	MFRC522_CardTypeTypeDef (* SelectTag)	(uint8_t * serNum);
	MFRC522_StatusTypeDef (* Auth)				(MFRC522_AuthTypeDef authMode, uint8_t BlockAddr, uint8_t * Sectorkey, uint8_t * serNum);
	MFRC522_StatusTypeDef (* Read)				(uint8_t blockAddr, uint8_t * recvData);
	MFRC522_StatusTypeDef (* Write)				(uint8_t blockAddr, uint8_t * writeData);
	void (* Halt)		(void);
	char (* HostCodeKey)	(  unsigned char *uncoded, unsigned char *coded);	
}MFRC522_ClassTypeDef;

MFRC522_StatusTypeDef MFRC522_Check(uint8_t * id);
MFRC522_StatusTypeDef MFRC522_Compare(uint8_t * CardID, uint8_t * CompareID);
void MFRC522_Init(MFRC522_StructTypeDef * InitStruct);
MFRC522_StatusTypeDef MFRC522_Operate32 (uint8_t * SN, uint8_t Sector,
								uint8_t * KEY, void (*KeyGenFunc)(uint8_t * sn, uint8_t * key), void (*EvenHandler)(uint8_t * SN, uint32_t * data), MFRC522_ToDoTypeDef Task);
MFRC522_StatusTypeDef MFRC522_Operate (uint8_t * SN, uint8_t Sector,
								uint8_t * KEY, void (*KeyGenFunc)(uint8_t * sn, uint8_t * key), void (*EvenHandler)(uint8_t * SN, uint8_t * Value), MFRC522_ToDoTypeDef Task);
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
void MFRC522_Reset(void);
MFRC522_StatusTypeDef MFRC522_Request(uint8_t reqMode, uint8_t * TagType);
MFRC522_StatusTypeDef MFRC522_ToCard(MFRC522_CommandsTypeDef command, uint8_t * sendData, uint8_t sendLen, uint8_t * backData, uint16_t * backLen);
MFRC522_StatusTypeDef MFRC522_Anticoll(uint8_t * serNum);
MFRC522_CardTypeTypeDef MFRC522_SelectTag(uint8_t * serNum);
MFRC522_StatusTypeDef MFRC522_Auth(MFRC522_AuthTypeDef authMode, uint8_t BlockAddr, uint8_t * Sectorkey, uint8_t * serNum);
MFRC522_StatusTypeDef MFRC522_Read(uint8_t blockAddr, uint8_t * recvData);
MFRC522_StatusTypeDef MFRC522_Write(uint8_t blockAddr, uint8_t * writeData);
void MFRC522_Halt(void);
char Mf500HostCodeKey(  unsigned char *uncoded, unsigned char *coded);

static const MFRC522_ClassTypeDef RFID = {MFRC522_Check, MFRC522_Compare, MFRC522_Init, MFRC522_Operate32, 
	MFRC522_Operate, MFRC522_AntennaOn, MFRC522_AntennaOff, MFRC522_Reset, MFRC522_Request, MFRC522_ToCard,
	MFRC522_Anticoll, MFRC522_SelectTag, MFRC522_Auth, MFRC522_Read, MFRC522_Write, MFRC522_Halt, Mf500HostCodeKey};
#endif
