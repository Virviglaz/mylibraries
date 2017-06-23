#include "MFRC631.h"

/* Local Variables */
MFRC631_StructTypeDef * MFRC631_Struct;

const u8 SettingsTypeA_ID2_106kBps[] = {0x8A,0x08,0x21,0x1A,0x18,0x18,0x0F,0x27,0x00,0xC0,0x12,0xCF,0x00,0x04,0x90,0x32,0x12,0x0A,0x20,0x04,0x50,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x02,0x00,0x00,0x01,0x00,0x08,0x80,0xF0};
const u8 SettingsTypeA_ID2_212kBps[] = {0x8E,0x12,0x11,0x06,0x18,0x18,0x0F,0x10,0x00,0xC0,0x12,0xCF,0x00,0x05,0x90,0x3F,0x12,0x02,0x20,0x05,0x50,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x22,0x00,0x00,0x00,0x00,0x0D,0x80,0xB2};
const u8 SettingsTypeA_ID2_424kBps[] = {0x8E,0x12,0x11,0x06,0x18,0x18,0x0F,0x08,0x00,0xC0,0x12,0xCF,0x00,0x06,0x90,0x3F,0x12,0x0A,0x20,0x06,0x50,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x22,0x00,0x00,0x00,0x00,0x0D,0x80,0xB2};
const u8 SettingsTypeA_ID2_848kBps[] = {0x8F,0xDB,0x11,0x06,0x18,0x18,0x0F,0x02,0x00,0xC0,0x12,0xCF,0x00,0x07,0x90,0x3F,0x12,0x02,0x20,0x07,0x50,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x22,0x00,0x00,0x00,0x00,0x0D,0x80,0xB2};

const u8 SettingsTypeB_ID2_106kBps[] = {0x8F,0xCC,0x01,0x06,0x7B,0x7B,0x08,0x00,0x00,0x01,0x00,0x05,0x00,0x34,0x90,0x3F,0x12,0x0a,0x09,0x04,0x08,0x04,0x00,0x03,0x00,0x01,0x00,0x00,0xAB,0x00,0x00,0x08,0x00,0x04,0x00,0x00,0x00,0x02,0x0D,0x80,0xB2};
const u8 SettingsTypeB_ID2_212kBps[] = {0};//TBD
const u8 SettingsTypeB_ID2_424kBps[] = {0};//TBD
const u8 SettingsTypeB_ID2_848kBps[] = {0};//TBD
	
/* Internal functions declaration */
void MFRC_WriteReg (MFRC631_RegisterTypeDef reg, u8 value);
u8 MFRC_ReadReg (MFRC631_RegisterTypeDef reg);
u16 MFRC631_ISO14443a_WUPA_REQA (u8 instruction);

/* FIFO */
void MFRC631_Flush_Fifo (void);
void MFRC631_Write_Fifo (u8 * data, u16 len);
void MFRC631_Read_Fifo (u8 * data, u16 len);
u16 MFRC631_Fifo_Length (void);

/* Timer control */
void MFRC631_Activate_Timer (u8 timer, u8 active);
void MFRC631_Timer_Set_Control (u8 timer, u8 value);
void MFRC631_Timer_Set_Reload (u8 timer, u16 value);
void MFRC631_Timer_Set_Value (u8 timer, u16 value);
u16 MFRC631_Timer_Get_Value (u8 timer);

/* IRQ control */
void MFRC631_Clear_IRQ0 (void);
void MFRC631_Clear_IRQ1 (void);
u8 MFRC631_IRQ0 (void);
u8 MFRC631_IRQ1 (void);

/* Commands */
//void MFRC631_Cmd_Idle (void);
void MFRC631_SendCMD (MFRC631_CommandTypeDef CMD);
void MFRC631_Cmd_Transceive (u8 * data, u8 len) ;

/* Utility functions */
void MFRC631_Cmd_Auth (u8 key_type, u8 block_address, const u8 * card_uid);
void MFRC631_AN11145_start_IQ_measurement	(void);

/* Public functions */
void MFRC631_Init (MFRC631_StructTypeDef * MFRC631_InitStruct)
{
	/* Allign pointers to make structure available localy */
	MFRC631_Struct = MFRC631_InitStruct;
	
	/* Perform reset sequence */
	MFRC631_SendCMD(MFRC631_CMD_SOFTRESET);
  // Should sleep here... for 50ms... can do without.
	MFRC631_Struct->Delay_Func(50);	
  MFRC631_SendCMD(MFRC631_CMD_IDLE);
	
	/* Default settings load */
	//MFRC631_AN1102_Recommended_Registers(MFRC631_Struct->MFRC631_Protocol);
	
	MFRC631_Struct->WriteData((u8)Reg_DrvMod << 1, (u8*)SettingsTypeB_ID2_106kBps, sizeof(SettingsTypeB_ID2_106kBps));
	
	
	//MFRC_WriteReg(Reg_WaterLevel, 100);	
	
	//MFRC_WriteReg(Reg_DrvMod, 0x8F);	
  //MFRC_WriteReg(Reg_TxAmp, 30);
  //MFRC_WriteReg(Reg_DrvCon, 0x11);
  //MFRC_WriteReg(Reg_Txl, 0x06);
	
	/*MFRC_WriteReg(0x28, 0x8E);
  MFRC_WriteReg(0x29, 0x15);
  MFRC_WriteReg(0x2A, 0x11);
  MFRC_WriteReg(0x2B, 0x06);*/
}

u8 MFRC631_MIFARE_Auth (const u8 * uid, u8 key_type, u8 block) 
{
	u8 value, irq1_value = 0, status;
  // Enable the right interrupts.

  // configure a timeout timer.
  u8 timer_for_timeout = 0;  // should match the enabled interupt.

  // According to datashet Interrupt on idle and timer with MFAUTHENT, but lets
  // include ERROR as well.
	value = MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN;
  MFRC_WriteReg(Reg_IRQ0En, value);
	
	value = MFRC630_IRQ1EN_TIMER0_IRQEN;
  MFRC_WriteReg(Reg_IRQ1En, value);  // only trigger on timer for irq1

  // Set timer to 221 kHz clock, start at the end of Tx.
  MFRC631_Timer_Set_Control (timer_for_timeout, MFRC630_TCONTROL_CLK_211KHZ | MFRC630_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

  MFRC631_Timer_Set_Reload(timer_for_timeout, 2000);  // 2000 ticks of 5 usec is 10 ms.
  MFRC631_Timer_Set_Value(timer_for_timeout, 2000);

  MFRC631_Clear_IRQ0();  // clear irq0
  MFRC631_Clear_IRQ1();  // clear irq1

  // start the authentication procedure.
  MFRC631_Cmd_Auth(key_type, block, uid);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) 
	{
    irq1_value = MFRC631_IRQ1();
    if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) 
      break;  // stop polling irq1 and quit the timeout loop.  
  }

  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;  // we have no authentication
  }

  // status is always valid, it is set to 0 in case of authentication failure.
  status = MFRC_ReadReg(Reg_Status);
  return (status & MFRC630_STATUS_CRYPTO1_ON);
}

u8 MFRC631_MIFARE_Read_Block(u8 block_address, u8 * dest) 
{
	u8 send_req[2], timer_for_timeout, irq0_value = 0, irq1_value = 0;
	u8 buffer_length, rx_len;
  MFRC631_Flush_Fifo();

  MFRC_WriteReg(Reg_TxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
  MFRC_WriteReg(Reg_RxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);

	send_req[0] = MFRC630_MF_CMD_READ;
	send_req[1] = block_address;

  // configure a timeout timer.
  timer_for_timeout = 0;  // should match the enabled interupt.

  // enable the global IRQ for idle, errors and timer.
  MFRC_WriteReg(Reg_IRQ0En, MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
  MFRC_WriteReg(Reg_IRQ1En, MFRC630_IRQ1EN_TIMER0_IRQEN);


  // Set timer to 221 kHz clock, start at the end of Tx.
  MFRC631_Timer_Set_Control (timer_for_timeout, MFRC630_TCONTROL_CLK_211KHZ | MFRC630_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms
  MFRC631_Timer_Set_Reload (timer_for_timeout, 2000);  // 2000 ticks of 5 usec is 10 ms.
  MFRC631_Timer_Set_Value (timer_for_timeout, 2000);

  MFRC631_Clear_IRQ0();  // clear irq0
  MFRC631_Clear_IRQ1();  // clear irq1

  // Go into send, then straight after in receive.
  MFRC631_Cmd_Transceive (send_req, 2);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = MFRC631_IRQ1();
    if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }
  MFRC631_SendCMD(MFRC631_CMD_IDLE);

  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;
  }

  irq0_value = MFRC631_IRQ0();
  if (irq0_value & MFRC630_IRQ0_ERR_IRQ) {
    // some error
    return 0;
  }

  // all seems to be well...
  buffer_length = MFRC631_Fifo_Length();
  rx_len = (buffer_length <= 16) ? buffer_length : 16;
  MFRC631_Read_Fifo(dest, rx_len);
  return rx_len;
}

u8 MFRC631_MIFARE_Write_Block (u8 block_address, u8 * source) 
{
	u8 timer_for_timeout = 0; // should match the enabled interupt.
	u8 irq0_value = 0, irq1_value = 0, res, send_req[2], buffer_length;
  MFRC631_Flush_Fifo();

  // set appropriate CRC registers, only for Tx
  MFRC_WriteReg(Reg_TxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
  MFRC_WriteReg(Reg_RxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_OFF);

  // enable the global IRQ for idle, errors and timer.
  MFRC_WriteReg(Reg_IRQ0En, MFRC630_IRQ0EN_IDLE_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
  MFRC_WriteReg(Reg_IRQ1En, MFRC630_IRQ1EN_TIMER0_IRQEN);

  // Set timer to 221 kHz clock, start at the end of Tx.
  MFRC631_Timer_Set_Control(timer_for_timeout, MFRC630_TCONTROL_CLK_211KHZ | MFRC630_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms
  MFRC631_Timer_Set_Reload(timer_for_timeout, 2000);  // 2000 ticks of 5 usec is 10 ms.
  MFRC631_Timer_Set_Value(timer_for_timeout, 2000);

	send_req[0] = MFRC630_MF_CMD_WRITE;
	send_req[1] = block_address;

  MFRC631_Clear_IRQ0();  // clear irq0
  MFRC631_Clear_IRQ1();  // clear irq1

  // Go into send, then straight after in receive.
  MFRC631_Cmd_Transceive(send_req, 2);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = MFRC631_IRQ1();
    if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }
  MFRC631_SendCMD(MFRC631_CMD_IDLE);

  // check if the first stage was successful:
  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;
  }
  irq0_value = MFRC631_IRQ0();
  if (irq0_value & MFRC630_IRQ0_ERR_IRQ) {
    // some error
    return 0;
  }
  buffer_length = MFRC631_Fifo_Length();
  if (buffer_length != 1) {
    return 0;
  }
  MFRC631_Read_Fifo(&res, 1);
  if (res != MFRC630_MF_ACK) {
    return 0;
  }

  MFRC631_Clear_IRQ0();  // clear irq0
  MFRC631_Clear_IRQ1();  // clear irq1

  // go for the second stage.
  MFRC631_Cmd_Transceive(source, 16);

  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = MFRC631_IRQ1();
    if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) {
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }

  MFRC631_SendCMD(MFRC631_CMD_IDLE);

  if (irq1_value & (1 << timer_for_timeout)) {
    // this indicates a timeout
    return 0;
  }
  irq0_value = MFRC631_IRQ0();
  if (irq0_value & MFRC630_IRQ0_ERR_IRQ) {
    // some error
    return 0;
  }

  buffer_length = MFRC631_Fifo_Length();
  if (buffer_length != 1) {
    return 0;
  }
  MFRC631_Read_Fifo(&res, 1);
  if (res == MFRC630_MF_ACK) {
    return 16;  // second stage was responded with ack! Write successful.
  }

  return 0;
}

void MFRC631_AN1102_Recommended_Registers (MFRC631_ProtocolTypeDef protocol) 
{
	const u8 Protocol1[] = MFRC630_RECOM_14443A_ID1_106;
	const u8 Protocol2[] = MFRC630_RECOM_14443A_ID1_212;
	const u8 Protocol3[] = MFRC630_RECOM_14443A_ID1_424;
	const u8 Protocol4[] = MFRC630_RECOM_14443A_ID1_848;
	
	u8 * ProtPointer = 0;
	
  switch (protocol)
	{
    case MFRC630_PROTO_ISO14443A_106_MILLER_MANCHESTER:
        ProtPointer = (u8*)Protocol1; break;     

    case MFRC630_PROTO_ISO14443A_212_MILLER_BPSK:
        ProtPointer = (u8*)Protocol2; break;     

    case MFRC630_PROTO_ISO14443A_424_MILLER_BPSK:
        ProtPointer = (u8*)Protocol3; break;     

    case MFRC630_PROTO_ISO14443A_848_MILLER_BPSK:
        ProtPointer = (u8*)Protocol4; break;
  }
	if (* ProtPointer)	
		MFRC631_Struct->WriteData((u8)Reg_DrvMod << 1, ProtPointer, sizeof(Protocol1));
	
}

/* Internal functions */
void MFRC_WriteReg (MFRC631_RegisterTypeDef reg, u8 value)
{
	MFRC631_Struct->WriteData(reg << 1, &value, 1);
}

u8 MFRC_ReadReg (MFRC631_RegisterTypeDef reg)
{
	u8 res;
	MFRC631_Struct->ReadData((reg << 1) | 1, &res, 1);
	return res;
}

void MFRC631_Write_Fifo (u8 * data, u16 len) 
{
	MFRC631_Struct->WriteData((u8)Reg_FIFOData << 1, data, len);
}

void MFRC631_Read_Fifo (u8 * data, u16 len) 
{
	MFRC631_Struct->ReadData(((u8)Reg_FIFOData << 1) | 0x01, data, len);
}

// ---------------------------------------------------------------------------
// Timer functions
// ---------------------------------------------------------------------------
void MFRC631_Activate_Timer (u8 timer, u8 active) 
{
  MFRC_WriteReg (Reg_TControl, ((active << timer) << 4) | (1 << timer));
}

void MFRC631_Timer_Set_Control (u8 timer, u8 value) 
{
  MFRC_WriteReg ((MFRC631_RegisterTypeDef)(Reg_T0Control + 5 * timer), value);
}

void MFRC631_Timer_Set_Reload (u8 timer, u16 value) 
{
  MFRC_WriteReg((MFRC631_RegisterTypeDef)(Reg_T0ReloadHi + 5 * timer), value >> 8);
  MFRC_WriteReg((MFRC631_RegisterTypeDef)(Reg_T0ReloadLo + 5 * timer), value & 0xFF);
}

void MFRC631_Timer_Set_Value (u8 timer, u16 value) 
{
  MFRC_WriteReg((MFRC631_RegisterTypeDef)(Reg_T0CounterValHi + 5 * timer), value >> 8);
  MFRC_WriteReg((MFRC631_RegisterTypeDef)(Reg_T0CounterValLo + 5 * timer), value & 0xFF);
}

u16 MFRC631_Timer_Get_Value (u8 timer) 
{
  u16 res = MFRC_ReadReg ((MFRC631_RegisterTypeDef)(Reg_T0CounterValHi + 5 * timer)) << 8;
  res |= MFRC_ReadReg((MFRC631_RegisterTypeDef)(Reg_T0CounterValLo + 5 * timer));
  return res;
}

// ---------------------------------------------------------------------------
// IRQ functions
// ---------------------------------------------------------------------------
void MFRC631_Clear_IRQ0 (void) 
{
  MFRC_WriteReg (Reg_IRQ0, (u8) ~(1<<7));
}
void MFRC631_Clear_IRQ1 (void)
{
  MFRC_WriteReg(Reg_IRQ1, (u8) ~(1<<7));
}
u8 MFRC631_IRQ0 (void)
{
  return MFRC_ReadReg (Reg_IRQ0);
}
u8 MFRC631_IRQ1 (void)
{
  return MFRC_ReadReg(Reg_IRQ1);
}

// ---------------------------------------------------------------------------
// COMMANDS
// ---------------------------------------------------------------------------
void MFRC631_SendCMD (MFRC631_CommandTypeDef CMD)
{
	MFRC_WriteReg(Reg_Command, (u8)CMD);
}

void MFRC631_Cmd_Transceive (u8 * data, u8 len) 
{
  MFRC631_SendCMD(MFRC631_CMD_IDLE);
  MFRC631_Flush_Fifo();
  MFRC631_Write_Fifo(data, len);
  MFRC631_SendCMD(MFRC631_CMD_TRANSCEIVE);
}


// ---------------------------------------------------------------------------
// Utility functions.
// ---------------------------------------------------------------------------

void MFRC631_Flush_Fifo (void) 
{
  MFRC_WriteReg(Reg_FIFOControl, 1 << 4);
}

u16 MFRC631_Fifo_Length (void) 
{
  u16 result;
	result = (MFRC_ReadReg(Reg_FIFOControl) & 0x03) << 8;
	result |= MFRC_ReadReg(Reg_FIFOLength);
  return result;
}

void MFRC631_Cmd_Auth (u8 key_type, u8 block_address, const u8 * card_uid) 
{
	u8 parameters[6];
	parameters[0] = key_type;
	parameters[1] = block_address;
	parameters[2] = card_uid[0];
	parameters[3] = card_uid[1];
	parameters[4] = card_uid[2];
	parameters[5] = card_uid[3];
  MFRC631_SendCMD(MFRC631_CMD_IDLE); 
  MFRC631_Flush_Fifo();
  MFRC631_Write_Fifo (parameters, 6);
	MFRC631_SendCMD(MFRC631_CMD_MFAUTHENT);
}

// ---------------------------------------------------------------------------
// ISO 14443A
// ---------------------------------------------------------------------------

u16 MFRC631_ISO14443a_REQA (void) 
{
  return MFRC631_ISO14443a_WUPA_REQA(MFRC630_ISO14443_CMD_REQA);
}
u16 MFRC631_ISO4443a_WUPA (void)
{
  return MFRC631_ISO14443a_WUPA_REQA(MFRC630_ISO14443_CMD_WUPA);
}

u16 MFRC631_ISO14443a_WUPA_REQA (u8 instruction) 
{
  u16 res;
  static u8 timer_for_timeout = 0, irq1_value = 0, irq0, rx_len;
  MFRC631_SendCMD(MFRC631_CMD_IDLE);	//0x00 0x00
  MFRC631_Flush_Fifo();								//0x04 0x10

  // Set register such that we sent 7 bits, set DataEn such that we can send
  // data.
  MFRC_WriteReg(Reg_TxDataNum, 7 | MFRC630_TXDATANUM_DATAEN); //0x5C 0x0F

  // disable the CRC registers.
  MFRC_WriteReg(Reg_TxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_OFF);		//0x58	0x18
  MFRC_WriteReg(Reg_RxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_OFF);		//0x5A	0x18

  MFRC_WriteReg(Reg_RxBitCtrl, 0);	//0x18	0x00

  // clear interrupts
  MFRC631_Clear_IRQ0();  // clear irq0	0x0C 0x7F
  MFRC631_Clear_IRQ1();  // clear irq1	0x0E 0x7F

  // enable the global IRQ for Rx done and Errors.
  MFRC_WriteReg(Reg_IRQ0En, MFRC630_IRQ0EN_RX_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);	//0x10 0x06
  MFRC_WriteReg(Reg_IRQ1En, MFRC630_IRQ1EN_TIMER0_IRQEN);  // only trigger on timer for irq1	0x12 0x01

  // Set timer to 221 kHz clock, start at the end of Tx.
  MFRC631_Timer_Set_Control(timer_for_timeout, MFRC630_TCONTROL_CLK_211KHZ | MFRC630_TCONTROL_START_TX_END);	//0x1E 0x11
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

  MFRC631_Timer_Set_Reload(timer_for_timeout, 1000);  // 1000 ticks of 5 usec is 5 ms.	0x20 0x03 0x22 0xE8
  MFRC631_Timer_Set_Value(timer_for_timeout, 1000);	//0x24 0x03 0x26 0xE8

  // Go into send, then straight after in receive.
  MFRC631_Cmd_Transceive(&instruction, 1); //0x00 0x00, 0x04 0x10, 0x0A 0x26, 0x00 0x07
	
  // block until we are done
  while (!(irq1_value & (1 << timer_for_timeout))) {
    irq1_value = MFRC631_IRQ1();
    if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) {  // either ERR_IRQ or RX_IRQ
      break;  // stop polling irq1 and quit the timeout loop.
    }
  }
  MFRC631_SendCMD(MFRC631_CMD_IDLE); 

  // if no Rx IRQ, or if there's an error somehow, return 0
  irq0 = MFRC631_IRQ0();
  if ((!(irq0 & MFRC630_IRQ0_RX_IRQ)) || (irq0 & MFRC630_IRQ0_ERR_IRQ))
    return 0;
  
  rx_len = MFRC631_Fifo_Length();

  if (rx_len == 2) {  // ATQA should answer with 2 bytes.
    MFRC631_Read_Fifo((u8 *) &res, rx_len);
    return res;
  }
  return 0;
}

u8 MFRC631_ISO14443A_Select(u8 * uid, u8 * sak) 
{
	u8 timer_for_timeout = 0, irq0_value, irq1_value = 0, cascade_level, collision_n, rxalign;
	u8 error, coll, collision_pos, rx_len, buf[5], rbx, bcc_val, bcc_calc, sak_len, sak_value;
	
  MFRC631_SendCMD(MFRC631_CMD_IDLE);
  MFRC631_Flush_Fifo();

  // we do not need atqa.
  // Bitshift to get uid_size; 0b00: single, 0b01: double, 0b10: triple, 0b11 RFU
  // uint8_t uid_size = (atqa & (0b11 << 6)) >> 6;
  // uint8_t bit_frame_collision = atqa & 0b11111;

  // enable the global IRQ for Rx done and Errors.
  MFRC_WriteReg(Reg_IRQ0En, MFRC630_IRQ0EN_RX_IRQEN | MFRC630_IRQ0EN_ERR_IRQEN);
  MFRC_WriteReg(Reg_IRQ1En, MFRC630_IRQ1EN_TIMER0_IRQEN);  // only trigger on timer for irq1

  // configure a timeout timer, use timer 0.
  timer_for_timeout = 0;

  // Set timer to 221 kHz clock, start at the end of Tx.
  MFRC631_Timer_Set_Control(timer_for_timeout, MFRC630_TCONTROL_CLK_211KHZ | MFRC630_TCONTROL_START_TX_END);
  // Frame waiting time: FWT = (256 x 16/fc) x 2 FWI
  // FWI defaults to four... so that would mean wait for a maximum of ~ 5ms

  MFRC631_Timer_Set_Reload(timer_for_timeout, 1000);  // 1000 ticks of 5 usec is 5 ms.
  MFRC631_Timer_Set_Value(timer_for_timeout, 1000);

  for (cascade_level = 1; cascade_level <= 3; cascade_level++) 
	{
    u8 cmd = 0;
    u8 known_bits = 0;  // known bits of the UID at this level so far.
    u8 send_req[7] = {0};  // used as Tx buffer.
    u8 * uid_this_level = &(send_req[2]);
    // pointer to the UID so far, by placing this pointer in the send_req
    // array we prevent copying the UID continuously.
    u8 message_length;

    switch (cascade_level) 
		{
      case 1:
        cmd = MFRC630_ISO14443_CAS_LEVEL_1;
        break;
      case 2:
        cmd = MFRC630_ISO14443_CAS_LEVEL_2;
        break;
      case 3:
        cmd = MFRC630_ISO14443_CAS_LEVEL_3;
        break;
    }

    // disable CRC in anticipation of the anti collision protocol
    MFRC_WriteReg(Reg_TxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_OFF);
    MFRC_WriteReg(Reg_RxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_OFF);

    // max 32 loops of the collision loop.
    for (collision_n = 0; collision_n < 32; collision_n++) 
		{
      // clear interrupts
      MFRC631_Clear_IRQ0();
      MFRC631_Clear_IRQ1();

      send_req[0] = cmd;
      send_req[1] = 0x20 + known_bits;
      // send_req[2..] are filled with the UID via the uid_this_level pointer.

      // Only transmit the last 'x' bits of the current byte we are discovering
      // First limit the txdatanum, such that it limits the correct number of bits.
      MFRC_WriteReg(Reg_TxDataNum, (known_bits % 8) | MFRC630_TXDATANUM_DATAEN);

      // set it to insert zeros in case of a collision, (1<<7)
      // We want to shift the bits with RxAlign
      rxalign = known_bits % 8;      
      MFRC_WriteReg(Reg_RxBitCtrl, (1<<7) | (rxalign<<4));


      // then sent the send_req to the hardware,
      // (known_bits / 8) + 1): The ceiled number of bytes by known bits.
      // +2 for cmd and NVB.
      if ((known_bits % 8) == 0) {
        message_length = ((known_bits / 8)) + 2;
      } else {
        message_length = ((known_bits / 8) + 1) + 2;
      }

      MFRC631_Cmd_Transceive(send_req, message_length);

      // block until we are done
      irq1_value = 0;
      while (!(irq1_value & (1 << timer_for_timeout))) {
        irq1_value = MFRC631_IRQ1();
        // either ERR_IRQ or RX_IRQ or Timer
        if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) {
          break;  // stop polling irq1 and quit the timeout loop.
        }
      }
      MFRC631_SendCMD(MFRC631_CMD_IDLE);

      // next up, we have to check what happened.
      irq0_value = MFRC631_IRQ0();
      error = MFRC_ReadReg(Reg_Error);
      coll = MFRC_ReadReg(Reg_RxColl);

      collision_pos = 0;
      if (irq0_value & MFRC630_IRQ0_ERR_IRQ) {  // some error occured.
        // Check what kind of error.
        // error = mfrc630_read_reg(MFRC630_REG_ERROR);
        if (error & MFRC630_ERROR_COLLDET) 
				{					
          // A collision was detected...
          if (coll & (1<<7)) 
					{
						u8 choice_pos, selection;
            collision_pos = coll & (~(1<<7));
            // This be a true collision... we have to select either the address
            // with 1 at this position or with zero
            // ISO spec says typically a 1 is added, that would mean:
            // uint8_t selection = 1;

            // However, it makes sense to allow some kind of user input for this, so we use the
            // current value of uid at this position, first index right byte, then shift such
            // that it is in the rightmost position, ten select the last bit only.
            // We cannot compensate for the addition of the cascade tag, so this really
            // only works for the first cascade level, since we only know whether we had
            // a cascade level at the end when the SAK was received.
            choice_pos = known_bits + collision_pos;
            selection = (uid[((choice_pos + (cascade_level-1) * 3) / 8)] >> ((choice_pos) % 8)) & 1;


            // We just OR this into the UID at the right position, later we
            // OR the UID up to this point into uid_this_level.
            uid_this_level[((choice_pos)/8)] |= selection << ((choice_pos) % 8);
            known_bits++;  // add the bit we just decided.

          } 
					else 
					{
            // Datasheet of mfrc630:
            // bit 7 (CollPosValid) not set:
            // Otherwise no collision is detected or
            // the position of the collision is out of the range of bits CollPos.
            collision_pos = 0x20 - known_bits;
          }
        } 
				else 
				{
          // Can this ever occur?
          collision_pos = 0x20 - known_bits;
        }
      } 
			else if (irq0_value & MFRC630_IRQ0_RX_IRQ) {
        // we got data, and no collisions, that means all is well.
        collision_pos = 0x20 - known_bits;
      } 
			else 
			{
        // We have no error, nor received an RX. No response, no card?
        return 0;
      }

      // read the UID Cln so far from the buffer.
      rx_len = MFRC631_Fifo_Length();

      MFRC631_Read_Fifo(buf, rx_len < 5 ? rx_len : 5);
			
      // move the buffer into the uid at this level, but OR the result such that
      // we do not lose the bit we just set if we have a collision.
      for (rbx = 0; (rbx < rx_len); rbx++) 
			{
        uid_this_level[(known_bits / 8) + rbx] |= buf[rbx];
      }
			
      known_bits += collision_pos;

      if ((known_bits >= 32)) 
			{
        break;  // done with collision loop
      }
    }  // end collission loop

    // check if the BCC matches
    bcc_val = uid_this_level[4];  // always at position 4, either with CT UID[0-2] or UID[0-3] in front.
    bcc_calc = uid_this_level[0]^uid_this_level[1]^uid_this_level[2]^uid_this_level[3];
    if (bcc_val != bcc_calc) 
		{
      //MFRC630_PRINTF("Something went wrong, BCC does not match.\n");
      return 0;
    }

    // clear interrupts
    MFRC631_Clear_IRQ0();
    MFRC631_Clear_IRQ1();

    send_req[0] = cmd;
    send_req[1] = 0x70;
    // send_req[2,3,4,5] // contain the CT, UID[0-2] or UID[0-3]
    send_req[6] = bcc_calc;
    message_length = 7;

    // Ok, almost done now, we reenable the CRC's
    MFRC_WriteReg(Reg_TxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);
    MFRC_WriteReg(Reg_RxCrcPreset, MFRC630_RECOM_14443A_CRC | MFRC630_CRC_ON);

    // reset the Tx and Rx registers (disable alignment, transmit full bytes)
    MFRC_WriteReg(Reg_TxDataNum, (known_bits % 8) | MFRC630_TXDATANUM_DATAEN);
    rxalign = 0;
    MFRC_WriteReg(Reg_RxBitCtrl, (1 << 7) | (rxalign << 4));

    // actually send it!
    MFRC631_Cmd_Transceive(send_req, message_length);

    // Block until we are done...
    irq1_value = 0;
    while (!(irq1_value & (1 << timer_for_timeout))) {
      irq1_value = MFRC631_IRQ1();
      if (irq1_value & MFRC630_IRQ1_GLOBAL_IRQ) {  // either ERR_IRQ or RX_IRQ
        break;  // stop polling irq1 and quit the timeout loop.
      }
    }
    MFRC631_SendCMD(MFRC631_CMD_IDLE);

    // Check the source of exiting the loop.
    irq0_value = MFRC631_IRQ0();
    if (irq0_value & MFRC630_IRQ0_ERR_IRQ) {
      // Check what kind of error.
      error = MFRC_ReadReg(Reg_Error);
      if (error & MFRC630_ERROR_COLLDET) {
        // a collision was detected with NVB=0x70, should never happen.
        return 0;
      }
    }

    // Read the sak answer from the fifo.
    sak_len = MFRC631_Fifo_Length();
    if (sak_len != 1) {
      return 0;
    }

    MFRC631_Read_Fifo(&sak_value, sak_len);

    if (sak_value & (1 << 2)) 
		{
			u8 UIDn;
      // UID not yet complete, continue with next cascade.
      // This also means the 0'th byte of the UID in this level was CT, so we
      // have to shift all bytes when moving to uid from uid_this_level.
      for (UIDn = 0; UIDn < 3; UIDn++) 
			{
        // uid_this_level[UIDn] = uid_this_level[UIDn + 1];
        uid[(cascade_level-1)*3 + UIDn] = uid_this_level[UIDn + 1];
      }
    } 
		else 
		{
      // Done according so SAK!
      // Add the bytes at this level to the UID.
      u8 UIDn;
      for (UIDn = 0; UIDn < 4; UIDn++) {
        uid[(cascade_level-1)*3 + UIDn] = uid_this_level[UIDn];
      }

      // Finally, return the length of the UID that's now at the uid pointer.
      return cascade_level*3 + 1;
    }
  }  // cascade loop
  return 0;  // getting an UID failed.
}




void MFRC631_AN11145_start_IQ_measurement	(void) 
{
  // Part-1, configurate LPCD Mode
  // Please remove any PICC from the HF of the reader.
  // "I" and the "Q" values read from reg 0x42 and 0x43
  // shall be used in part-2 "Detect PICC"
  //  reset CLRC663 and idle
  MFRC_WriteReg(Reg_Command, MFRC630_CMD_SOFTRESET);
  // Should sleep here... for 50ms... can do without.
	MFRC631_Struct->Delay_Func(50);
  MFRC_WriteReg(Reg_Command, 0);
  // disable IRQ0, IRQ1 interrupt sources
  MFRC_WriteReg(Reg_IRQ0, 0x7F);
  MFRC_WriteReg(Reg_IRQ1, 0x7F);
  MFRC_WriteReg(Reg_IRQ0En, 0x00);
  MFRC_WriteReg(Reg_IRQ1En, 0x00);
  MFRC_WriteReg(Reg_FIFOControl, 0xB0);  // Flush FIFO
  // LPCD_config
  MFRC_WriteReg(Reg_LPCD_QMin, 0xC0);  // Set Qmin register
  MFRC_WriteReg(Reg_LPCD_QMax, 0xFF);  // Set Qmax register
  MFRC_WriteReg(Reg_LPCD_IMin, 0xC0);  // Set Imin register
  MFRC_WriteReg(Reg_DrvMod, 0x89);  // set DrvMode register
  // Execute trimming procedure
  MFRC_WriteReg(Reg_T3ReloadHi, 0x00);  // Write default. T3 reload value Hi
  MFRC_WriteReg(Reg_T3ReloadLo, 0x10);  // Write default. T3 reload value Lo
  MFRC_WriteReg(Reg_T4ReloadHi, 0x00);  // Write min. T4 reload value Hi
  MFRC_WriteReg(Reg_T4ReloadLo, 0x05);  // Write min. T4 reload value Lo
  MFRC_WriteReg(Reg_T4Control, 0xF8);  // Config T4 for AutoLPCD&AutoRestart.Set AutoTrimm bit.Start T4.
  MFRC_WriteReg(Reg_LPCD_Q_Result, 0x40);  // Clear LPCD result
  MFRC_WriteReg(Reg_Rcv, 0x52);  // Set Rx_ADCmode bit
  MFRC_WriteReg(Reg_RxAna, 0x03);  // Raise receiver gain to maximum
  MFRC_WriteReg(Reg_Command, 0x01);  // Execute Rc663 command "Auto_T4" (Low power card detection and/or Auto trimming)
}

void MFRC631_EEPROM_Read (u8 * buffer, u16 address, u8 size)
{
	u8 params[3];
	MFRC631_SendCMD(MFRC631_CMD_IDLE);
	MFRC631_Flush_Fifo();
	
	params[0] = (u8)(address >> 8);
	params[1] = (u8)(address);
	params[2] = size;
	MFRC631_Write_Fifo(params, sizeof(params));
	MFRC631_SendCMD(MFRC631_CMD_READE2);
	MFRC631_Struct->Delay_Func(10);
	MFRC631_Read_Fifo(buffer, size);
}
