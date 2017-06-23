/* VERSION 4.00 */
/*  10.05.2017  */
/* STM8 Version */

#include "nrf24l01.h"           //main header

RF_InitTypeDef * RF_InitStruct;

nRF_ERROR_TypeDef RF_Send_Cmd (nRF_RegTypeDef reg, char cmd);
char RF_Read_Cmd(nRF_RegTypeDef adrs);
void RF_Send_Adr (nRF_RegTypeDef adrs, char * data);
void RF_IRQ_CLEAR (char cmd);
void RF_Flush (char cmd);

/**
  * @brief  Initialise NRF24L01 
  * @param  RF_InitStruct: pointer to an RF_InitTypeDef structure that contains
  *   the configuration information for the specified parameters.
  * @retval STATUS REG content (0x0E by default, 0x00 or 0xFF are impossible values)
  */
nRF_ERROR_TypeDef RF_Init (RF_InitTypeDef * InitStruct)
{
    // Assign the pointer to init structure
    RF_InitStruct = InitStruct;
  
    nRF_ERROR_TypeDef res =         RF_Send_Cmd (nRF_CONFIG_REG, 
                                    (char)RF_InitStruct->nRF_RX_IRQ_Config | 
                                    (char)RF_InitStruct->nRF_TX_IRQ_Config |
                                    (char)RF_InitStruct->nRF_RT_IRQ_Config |
                                    (char)RF_InitStruct->nRF_Power | 
                                    (char)RF_InitStruct->nRF_CRC_Mode |
                                    (char)RF_InitStruct->nRF_Mode);
    
    if (res == 0x00 || res == 0xFF) return nRF_ERROR_CHIP_NOT_RESPONDING;
    
    RF_Send_Cmd (nRF_EN_AA_REG, (char)RF_InitStruct->nRF_Pipes_ACK_Enable);
    RF_Send_Cmd (nRF_EN_RXADDR_REG, (char)RF_InitStruct->nRF_Pipes_Enable);
    RF_Send_Cmd (nRF_SETUP_AW_REG, (char)RF_InitStruct->nRF_AddressLen);
    RF_Send_Cmd (nRF_SETUP_RETR_REG, (RF_InitStruct->nRF_Auto_Retransmit_Count & 0x0F)|((RF_InitStruct->nRF_Auto_Retransmit_Delay & 0x0F) << 4));
    RF_Send_Cmd (nRF_RF_CH_REG, (RF_InitStruct->nRF_Channel & 0x7F));
    RF_Send_Cmd (nRF_RF_SETUP_REG, (0x01 | (char)RF_InitStruct->nRF_TX_Power | (char)RF_InitStruct->nRF_Data_Rate));
    RF_Send_Adr (nRF_RX_ADDR_P0_REG, RF_InitStruct->nRF_RX_Adress_Pipe0);
    RF_Send_Adr (nRF_RX_ADDR_P1_REG, RF_InitStruct->nRF_RX_Adress_Pipe1);
    RF_Send_Cmd (nRF_RX_ADDR_P2_REG, RF_InitStruct->nRF_RX_Adress_Pipe2);
    RF_Send_Cmd (nRF_RX_ADDR_P3_REG, RF_InitStruct->nRF_RX_Adress_Pipe3);
    RF_Send_Cmd (nRF_RX_ADDR_P4_REG, RF_InitStruct->nRF_RX_Adress_Pipe4);
    RF_Send_Cmd (nRF_RX_ADDR_P5_REG, RF_InitStruct->nRF_RX_Adress_Pipe5);
    RF_Send_Adr (nRF_TX_ADDR_REG, RF_InitStruct->nRF_TX_Adress);
    RF_Send_Cmd (nRF_RX_PW_P0_REG, RF_InitStruct->nRF_Payload_Size_Pipe0);
    RF_Send_Cmd (nRF_RX_PW_P1_REG, RF_InitStruct->nRF_Payload_Size_Pipe1);
    RF_Send_Cmd (nRF_RX_PW_P2_REG, RF_InitStruct->nRF_Payload_Size_Pipe2);
    RF_Send_Cmd (nRF_RX_PW_P3_REG, RF_InitStruct->nRF_Payload_Size_Pipe3);
    RF_Send_Cmd (nRF_RX_PW_P4_REG, RF_InitStruct->nRF_Payload_Size_Pipe4);
    RF_Send_Cmd (nRF_RX_PW_P5_REG, RF_InitStruct->nRF_Payload_Size_Pipe5);
    
    ANT_ENABLE;
    return res;			 
}

void RF_ClassInit (RF_ClassTypeDef * RF_Class)
{
    // Assing the pointer to functions
    RF_Class->SendPayload = RF_SendPayload;
    RF_Class->SendPayloadACK = RF_SendPayloadACK;
    RF_Class->Receive_Data = RF_Receive_Data;
    RF_Class->Sleep = RF_Sleep;
    RF_Class->WakeUp = RF_WakeUp;  
}

/**
  * @brief  Send Payload via NRF24L01 
  * @param  unsigned char * data: pointer to first data memory location
  * @param  unsigned char DataLen: lenth of data array [1..32] for NRF24L01
  * @retval nRF_SUCCESS or nRF_ERROR_CHIP_NOT_RESPONDING if data send time overlimit
  */
nRF_ERROR_TypeDef RF_SendPayload (char * data, char size)
{
  unsigned long DS_Delay = Payload_send_delay;

  RF_InitStruct->WriteReg (RF_SendPayload_CMD, data, size);
  
  ANT_ENABLE;
  
  while (DS_Delay && RF_InitStruct->IO_IRQ_Func()) DS_Delay--;
  
  ANT_DISABLE;
  
  RF_IRQ_CLEAR (RF_TX_DS_IRQ_CLEAR);
  //RF_Flush (RF_Flush_TX_CMD);
  if (DS_Delay == 0) return nRF_ERROR_CHIP_NOT_RESPONDING;
  
  return nRF24L01_SUCCESS;
}

/**
  * @brief  Send Payload via NRF24L01 with Acknowledge
  * @param  unsigned char * data: pointer to first data memory location
  * @param  unsigned char DataLen: lenth of data array [1..32] for NRF24L01
  * @retval nRF_DATA_SEND_NO_ACK_RECEIVED if data send ok but no Acknowledge received 
            nRF_DATA_SEND_ACK_RECEIVED_OK if data send ok and Acknowledge received
            nRF_ERROR_CHIP_NOT_RESPONDING if data send time overlimit
  */
nRF_ERROR_TypeDef RF_SendPayloadACK (char * data, char size) //return 0 if no ACK received, return 0xFF if ERROR
{
  unsigned long DS_Delay = Payload_send_delay;
  nRF_ERROR_TypeDef res = nRF24L01_DATA_SEND_NO_ACK_RECEIVED;

  RF_InitStruct->WriteReg (RF_SendPayload_CMD, data, size);
  
  ANT_ENABLE;
  while(DS_Delay && RF_InitStruct->IO_IRQ_Func()) DS_Delay--;
  ANT_DISABLE;

  if (RF_InitStruct->ReadReg (0xFF, 0, 0) & RF_TX_DS_IRQ_CLEAR) 
    res = nRF24L01_DATA_SEND_ACK_RECEIVED_OK; //IRQ reflected, ACK received
  
  if(DS_Delay == 0) res = nRF_ERROR_CHIP_NOT_RESPONDING;  
  RF_IRQ_CLEAR (RF_MAX_RT_IRQ_CLEAR | RF_TX_DS_IRQ_CLEAR);
  //RF_Flush (RF_Flush_TX_CMD);

  return res; 
}

/**
  * @brief  Receive data from NRF24L01
  * @param  unsigned char * data: pointer to first data memory location
  * @param  unsigned char DataLen: lenth of data array [1..32] for NRF24L01
  * @retval unsigned char PipeNum - PIPE number of data received
    nRF_NO_DATA_RECEIVED if no data in FIFO and IRQ pin not reflected
  */
nRF_ERROR_TypeDef RF_Receive_Data (char * data, char size)
{
  nRF_ERROR_TypeDef res;
  if (RF_InitStruct->IO_IRQ_Func()) return nRF24L01_NO_DATA_RECEIVED;
    
  res = (nRF_ERROR_TypeDef) ((RF_InitStruct->ReadReg (RF_RX_PAYLOAD_CMD, data, size) & PipeBitMask) >> 1);
  
  if (RF_Read_Cmd(nRF_FIFO_STATUS_REG) & RF_RX_FIFO_EMPTY_Bit)	//check available data in RX FIFO
    RF_IRQ_CLEAR(RF_RX_DR_IRQ_CLEAR);	//if NO DATA in RX FIFO, clear IRQ
  return res;	
}

/* INTERNAL FUNCTIONS DEFINITIONS */
/* NOTHING INTERESTING HERE */

nRF_ERROR_TypeDef RF_Send_Cmd (nRF_RegTypeDef reg, char cmd) //write data to register procedure
{	        
  return (nRF_ERROR_TypeDef)RF_InitStruct->WriteReg ((0x1F & reg) | (1 << 5), &cmd, 1);
}
char RF_Read_Cmd (nRF_RegTypeDef adrs)										//read data from register
{	
  char res;	        
  RF_InitStruct->ReadReg (0x1F & (char)adrs, &res, 1);
  return res;
}

void RF_Send_Adr (nRF_RegTypeDef adrs, char * data)
{	  
  RF_InitStruct->WriteReg ((char)adrs | (1 << 5), data, Max_Adress_Len);
}

unsigned char RF_Carrier_Detect (void)										//returns 1 if Carrier Detected on current channel
{
  return (RF_Read_Cmd(nRF_CD_REG) & 0x01);
}

unsigned char RF_Count_Lost_Packets (void)									//returns num of Lost Packatets 
{
  return (((RF_Read_Cmd(nRF_OBSERV_TX_REG)) & 0xF0) >> 4);
}

unsigned char RF_Count_Resend_Packets (void)								//returns nut of Resend Packets
{
  return ((RF_Read_Cmd(nRF_OBSERV_TX_REG)) & 0x0F);
}

void RF_IRQ_CLEAR (char cmd)											//Clears IRQ
{
  RF_Send_Cmd(nRF_STATUS_REG, cmd);
} 

void RF_Flush (char cmd)
{
  RF_InitStruct->WriteReg (cmd, 0, 0);
}

unsigned char RF_Mode_Set (char Mode)
{
	char pMode = RF_Read_Cmd (nRF_CONFIG_REG);
	RF_Send_Cmd (nRF_CONFIG_REG, (pMode & 0xFE) | (Mode & 1));
	return pMode;
}

nRF_ERROR_TypeDef RF_Sleep (void)
{
 return RF_Send_Cmd (nRF_CONFIG_REG, RF_Read_Cmd (nRF_CONFIG_REG) & ~nRF_Power_On);  
}

nRF_ERROR_TypeDef RF_WakeUp (void)
{
 return RF_Send_Cmd (nRF_CONFIG_REG, RF_Read_Cmd (nRF_CONFIG_REG) | nRF_Power_On); 
}

nRF_ERROR_TypeDef RF_FastTX_Address_Confirure (char * Address)
{  
  return (nRF_ERROR_TypeDef)RF_InitStruct->WriteReg (nRF_TX_ADDR_REG | (1 << 5), Address, Max_Adress_Len);
}

RF_InitTypeDef * RF_InitStructGet (void)
{
  return RF_InitStruct;
}

unsigned short RF_GetChannelFreqInMHz (void)
{
  return 2400 + RF_InitStruct->nRF_Channel;
}