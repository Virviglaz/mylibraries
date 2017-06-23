#include "MFRC522.h"

/* Local pointer to existing outside configuration structure */
MFRC522_StructTypeDef * MFRC522_InitStruct;

/* Local used functions */
static void MFRC522_WriteRegister(uint8_t addr, uint8_t val);
static uint8_t MFRC522_ReadRegister(uint8_t addr);
static void MFRC522_EncodeData (uint32_t value, uint8_t * data);
static MFRC522_StatusTypeDef MFRC522_DecodeData (uint8_t * data, uint32_t * value);
static void MFRC522_SetBitMask(MRFC522_RegTypeDef reg, uint8_t mask);
static void MFRC522_ClearBitMask(MRFC522_RegTypeDef reg, uint8_t mask);

/**
  * @brief  High Level. Perform reset and initialize chip.
  * @param  InitStruct: pointer to init structure.
  * @retval None
  */
void MFRC522_Init(MFRC522_StructTypeDef * InitStruct) 
{
	MFRC522_InitStruct = InitStruct;
	MFRC522_Reset();
	MFRC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);
	MFRC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);
	MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);           
	MFRC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);
	MFRC522_WriteRegister(MFRC522_REG_RF_CFG, InitStruct->MFRC522_RXGain);			// 48dB gain	
	MFRC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
	MFRC522_WriteRegister(MFRC522_REG_MODE, 0x3D);
	MFRC522_AntennaOff();
}

/**
  * @brief  High Level. Read and decode data from one sector.
  * @param  SN: pointer to serial number array.
  * @param  Sector: Sector number.
  * @param  KEY: pointer to access key array.
  * @param  EvenHandler: Handler, that receive 32 bit value from card.
  * @param  Task: CARD_READ, CARD_WRITE, CARD_RW or CARD_RW_ANYWAY in case data format error.
  * @retval None
  */
MFRC522_StatusTypeDef MFRC522_Operate32 (uint8_t * SN, uint8_t Sector,
								uint8_t * KEY, void (*KeyGenFunc)(uint8_t * sn, uint8_t * key) ,void (*EvenHandler)(uint8_t * SN, uint32_t * data), MFRC522_ToDoTypeDef Task)
{
	static MFRC522_StatusTypeDef res;
	static uint8_t data[16]; 
	static uint32_t value;
	static uint8_t UID[2];
	
	res = MI_OK;
	
	/* Check params */
	if (Sector > 39) return MI_WRONG_PARAM;
	
	/* Check card presense */
	if (MFRC522_Request(PICC_REQIDL, UID) != MI_OK) return MI_NOCARD;
	
	/* Check link for collision */
	if (MFRC522_Anticoll(SN) != MI_OK) return MI_COLLISION;
	
	/* Check cart type. Only 1k supported for now */
	if (MFRC522_SelectTag(SN) != Mifare_1k) {MFRC522_Init(MFRC522_InitStruct); return MI_WRONG_CARD;}
	
	/* Generate KEY */
	KeyGenFunc(SN, KEY);
	
	/* Authorise with provided key */
	if (MFRC522_Auth(PICC_AUTHENT1A, Sector, KEY, SN) != MI_OK) {MFRC522_Init(MFRC522_InitStruct); return MI_AUTH_ERROR;}
	
	/* Read EEPROM data */
	if (Task != CARD_WRITE)
	{
		if (MFRC522_Read(Sector, data) != MI_OK) {MFRC522_Init(MFRC522_InitStruct); return MI_READ_ERROR;}
	
		/* Check data is valid */
		res = MFRC522_DecodeData(data, &value);
	}

	/* Execute event handler */
	EvenHandler(SN, &value);
	
	/* Return data, no write */
	if (Task == CARD_READ){MFRC522_Init(MFRC522_InitStruct);	return res;}

	/* Check what to do. Procced writing anyway, but return error */
	if (res != MI_OK && Task != CARD_RW_ANYWAY) {MFRC522_Init(MFRC522_InitStruct);	return MI_WRONG_DATA;}
	
	/* Encode data */
	MFRC522_EncodeData(value, data);
	
	/* Write data back to card */
	if (MFRC522_Write(Sector, data) != MI_OK) {MFRC522_Init(MFRC522_InitStruct);	return MI_WRITE_ERROR;}
	
	/* Reset chip */
	MFRC522_Init(MFRC522_InitStruct);
	
	return res;
}

/**
  * @brief  High Level. Read data from one sector.
  * @param  SN: pointer to serial number array.
  * @param  Sector: Sector number.
  * @param  KEY: pointer to access key array.
  * @param  EvenHandler: Handler, that receive data from card.
  * @param  Task: CARD_READ, CARD_WRITE or CARD_RW.
  * @retval None
  */
MFRC522_StatusTypeDef MFRC522_Operate (uint8_t * SN, uint8_t Sector,
								uint8_t * KEY, void (*KeyGenFunc)(uint8_t * sn, uint8_t * key), void (*EvenHandler)(uint8_t * SN, uint8_t * Value), MFRC522_ToDoTypeDef Task)
{
	static MFRC522_StatusTypeDef res;
	static uint8_t data[16];
	
	res = MI_OK;
	
	/* Check params */
	if (Sector > 39) return MI_WRONG_PARAM;
	
	/* Enable transmitter */
	MFRC522_AntennaOn();
	
	/* Check card presense */
	if (MFRC522_Request(PICC_REQIDL, SN) != MI_OK) {MFRC522_AntennaOff(); return MI_NOCARD;}
	
	/* Check link for collision */
	if (MFRC522_Anticoll(SN) != MI_OK) {MFRC522_AntennaOff(); return MI_COLLISION;}
	
	/* Check cart type. Only 1k supported for now */
	if (MFRC522_SelectTag(SN) != Mifare_1k) {MFRC522_Init(MFRC522_InitStruct); return MI_WRONG_CARD;}
	
	/* Generate KEY */
	KeyGenFunc(SN, KEY);
	
	/* Authorise with provided key */
	if (MFRC522_Auth(PICC_AUTHENT1A, Sector, KEY, SN) != MI_OK) {MFRC522_Init(MFRC522_InitStruct); return MI_AUTH_ERROR;}
	
	/* Read EEPROM data */
	if (Task != CARD_WRITE)
		if (MFRC522_Read(Sector, data) != MI_OK) {MFRC522_Init(MFRC522_InitStruct); return MI_READ_ERROR;}
		
	/* Execute event handler */
	EvenHandler(SN, data);
		
	/* Return data, no write */
	if (Task == CARD_READ){MFRC522_Init(MFRC522_InitStruct);	return res;}

	/* Write data back to card */
	if (MFRC522_Write(Sector, data) != MI_OK) {MFRC522_Init(MFRC522_InitStruct);	return MI_WRITE_ERROR;}
	
	/* Reset chip */
	MFRC522_Init(MFRC522_InitStruct);

	return res;
}


MFRC522_StatusTypeDef MFRC522_Check (uint8_t * id) {
	static MFRC522_StatusTypeDef status;
	status = MFRC522_Request(PICC_REQIDL, id);					// Find cards, return card type
	if (status == MI_OK) status = MFRC522_Anticoll(id);			// Card detected. Anti-collision, return card serial number 4 bytes
	MFRC522_Halt();												// Command card into hibernation 
	return status;
}

MFRC522_StatusTypeDef MFRC522_Compare (uint8_t * CardID, uint8_t * CompareID) {
	static uint8_t i;
	for (i = 0; i < 5; i++) {
		if (CardID[i] != CompareID[i]) return MI_ERR;
	}
	return MI_OK;
}

static void MFRC522_WriteRegister (uint8_t addr, uint8_t val) {
	addr = (addr << 1) & 0x7E;																		// Address format: 0XXXXXX0
	MFRC522_InitStruct->WriteData(addr, (char*)&val, 1);
}

static uint8_t MFRC522_ReadRegister (uint8_t addr) {
	static uint8_t val;

	addr = ((addr << 1) & 0x7E) | 0x80;
	MFRC522_InitStruct->ReadData(addr, (char*)&val, 1);
	return val;	
}

static void MFRC522_SetBitMask (MRFC522_RegTypeDef reg, uint8_t mask) {
	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) | mask);
}

static void MFRC522_ClearBitMask (MRFC522_RegTypeDef reg, uint8_t mask){
	MFRC522_WriteRegister(reg, MFRC522_ReadRegister(reg) & (~mask));
} 

void MFRC522_AntennaOn (void) {
	static uint8_t temp;

	temp = MFRC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if (!(temp & 0x03)) MFRC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_AntennaOff(void) {
	MFRC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}

void MFRC522_Reset(void) {
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

MFRC522_StatusTypeDef MFRC522_Request (uint8_t reqMode, uint8_t * TagType) {
	static MFRC522_StatusTypeDef status;  
	static uint16_t backBits;																				// The received data bits

	MFRC522_WriteRegister (MFRC522_REG_BIT_FRAMING, 0x07);											// TxLastBists = BitFramingReg[2..0]
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	if ((status != MI_OK) || (backBits != 0x10)) status = MI_ERR;
	return status;
}

MFRC522_StatusTypeDef MFRC522_ToCard (MFRC522_CommandsTypeDef command, uint8_t * sendData, uint8_t sendLen, uint8_t * backData, uint16_t * backLen) {
	MFRC522_StatusTypeDef status = MI_ERR;
	static uint8_t irqEn;
	static uint8_t waitIRq;
	static uint8_t lastBits;
	static uint8_t n;
	static uint16_t i;

	irqEn = 0x00;
	waitIRq = 0x00;
	
	switch (command) {
		case PCD_AUTHENT: {
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE: {
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default: /* Not implemented yet */
		break;
	}

	MFRC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	MFRC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i < sendLen; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);

	// Execute the command
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE) MFRC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);					// StartSend=1,transmission of data starts 

	// Waiting to receive data to complete
	i = 2000;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
	do {
		// CommIrqReg[7..0]
		// Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = MFRC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	} while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	MFRC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);												// StartSend=0

	if (i != 0)  {
		if (!(MFRC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)) {
			status = MI_OK;
			if (n & irqEn & 0x01) status = MI_NOTAGERR;
			if (command == PCD_TRANSCEIVE) {
				n = MFRC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
				lastBits = MFRC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
				if (lastBits) *backLen = (n - 1) * 8 + lastBits; else *backLen = n * 8;
				if (n == 0) n = 1;
				if (n > MFRC522_MAX_LEN) n = MFRC522_MAX_LEN;
				for (i = 0; i < n; i++) backData[i] = MFRC522_ReadRegister(MFRC522_REG_FIFO_DATA);		// Reading the received data in FIFO
			}
		} else status = MI_ERR;
	}
	return status;
}

MFRC522_StatusTypeDef MFRC522_Anticoll (uint8_t * serNum) {
	static MFRC522_StatusTypeDef status;
	static uint8_t i;
	static uint8_t serNumCheck;
	static uint16_t unLen;

	serNumCheck = 0;
	MFRC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);												// TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	if (status == MI_OK) {
		// Check card serial number
		for (i = 0; i < 4; i++) serNumCheck ^= serNum[i];
		if (serNumCheck != serNum[i]) status = MI_ERR;
	}
	return status;
} 

static void MFRC522_CalculateCRC (uint8_t *  pIndata, uint8_t len, uint8_t * pOutData) {
	static uint8_t i, n;

	MFRC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);													// CRCIrq = 0
	MFRC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);													// Clear the FIFO pointer

	// Writing data to the FIFO	
	for (i = 0; i < len; i++) MFRC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata+i));
	MFRC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	// Wait CRC calculation is complete
	i = 0xFF;
	do {
		n = MFRC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i!=0) && !(n&0x04));																		// CRCIrq = 1

	// Read CRC calculation result
	pOutData[0] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = MFRC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

MFRC522_CardTypeTypeDef MFRC522_SelectTag (uint8_t * serNum) {
	static uint8_t i;
	static MFRC522_StatusTypeDef status;
	static MFRC522_CardTypeTypeDef CardType;
	static uint16_t recvBits;
	static uint8_t buffer[9]; 

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) buffer[i+2] = *(serNum+i);
	MFRC522_CalculateCRC(buffer, 7, &buffer[7]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	if ((status == MI_OK) && (recvBits == 0x18)) CardType = (MFRC522_CardTypeTypeDef)buffer[0]; else CardType = MifareErr;
	return CardType;
}

MFRC522_StatusTypeDef MFRC522_Auth (MFRC522_AuthTypeDef authMode, uint8_t BlockAddr, uint8_t * Sectorkey, uint8_t * serNum) {
	static MFRC522_StatusTypeDef status;
	static uint16_t recvBits;
	static uint8_t i;
	static uint8_t buff[12];

	// Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++) buff[i+2] = *(Sectorkey+i);
	for (i=0; i<4; i++) buff[i+8] = *(serNum+i);
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	if ((status != MI_OK) || (!(MFRC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08))) status = MI_ERR;
	return status;
}

MFRC522_StatusTypeDef MFRC522_Read (uint8_t blockAddr, uint8_t * recvData) {
	static MFRC522_StatusTypeDef status;
	static uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	MFRC522_CalculateCRC(recvData,2, &recvData[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	if ((status != MI_OK) || (unLen != 0x90)) status = MI_ERR;
	return status;
}

MFRC522_StatusTypeDef MFRC522_Write(uint8_t blockAddr, uint8_t * writeData) {
	static MFRC522_StatusTypeDef status;
	static uint16_t recvBits;
	static uint8_t i;
	static uint8_t buff[18]; 

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
	if (status == MI_OK) {
		// Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++) buff[i] = *(writeData+i);
		MFRC522_CalculateCRC(buff, 16, &buff[16]);
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) status = MI_ERR;
	}
	return status;
}

void MFRC522_Halt(void) {
	static uint16_t unLen;
	static uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

/* Prepare data according general rule of data storage in MIFARE */
static void MFRC522_EncodeData (uint32_t value, uint8_t * data)
{
	static union { uint32_t v32; uint8_t v8[4]; } V32b;
	V32b.v32 = value;
	
	/* Value */
	data[0] = V32b.v8[0];
	data[1] = V32b.v8[1];
	data[2] = V32b.v8[2];
	data[3] = V32b.v8[3];
	
	/* NOT Value */
	data[4] = ~V32b.v8[0];
	data[5] = ~V32b.v8[1];
	data[6] = ~V32b.v8[2];
	data[7] = ~V32b.v8[3];
	
	/* Value */
	data[8] = V32b.v8[0];
	data[9] = V32b.v8[1];
	data[10] = V32b.v8[2];
	data[11] = V32b.v8[3];
}

static MFRC522_StatusTypeDef MFRC522_DecodeData (uint8_t * data, uint32_t * value)
{
	static MFRC522_StatusTypeDef res = MI_OK;
	static uint8_t cnt;
	static union { uint32_t v32; uint8_t v8[sizeof(uint32_t)]; } V32b;
	
	for (cnt = 0; cnt != sizeof(uint32_t); cnt++)
		if (data[cnt] != data[cnt + 8] /*|| data[cnt] != ~data[cnt + 4]*/) {res = MI_WRONG_DATA; break;}
	
	V32b.v8[0] = data[0];
	V32b.v8[1] = data[1];
	V32b.v8[2] = data[2];
	V32b.v8[3] = data[3];

	* value = V32b.v32;
	return res;
}

///////////////////////////////////////////////////////////////////////
//                      C O D E   K E Y S  													 //
///////////////////////////////////////////////////////////////////////
char Mf500HostCodeKey(  unsigned char *uncoded, // 6 bytes key value uncoded
                     unsigned char *coded)   // 12 bytes key value coded
{
   static uint8_t cnt;
   static uint8_t ln  = 0;     // low nibble
   static uint8_t hn  = 0;     // high nibble
   
   for (cnt = 0; cnt < 6; cnt++)
   {
      ln = uncoded[cnt] & 0x0F;
      hn = uncoded[cnt] >> 4;
      coded[cnt * 2 + 1]     =  (~ln << 4) | ln;
      coded[cnt * 2 ] =  (~hn << 4) | hn;
   }
   return MI_OK;
}
