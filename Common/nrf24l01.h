#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#define Config_Reg_Reset_Value	        0x0E
#define Max_Adress_Len 			5		//maximum Bytes in Data Adress, typ. 5
#define Payload_send_delay		50000	//maximum delay in cycles for data send ready in case of error
#define PipeBitMask                     0x0E

// NRF24l01 registers define
typedef enum
{
  nRF_CONFIG_REG			= 0x00,
  nRF_EN_AA_REG			        = 0x01,
  nRF_EN_RXADDR_REG		        = 0x02,
  nRF_SETUP_AW_REG		        = 0x03,
  nRF_SETUP_RETR_REG		        = 0x04,
  nRF_RF_CH_REG			        = 0x05,
  nRF_RF_SETUP_REG		        = 0x06,
  nRF_STATUS_REG			= 0x07,
  nRF_OBSERV_TX_REG		        = 0x08,
  nRF_CD_REG				= 0x09,
  nRF_RX_ADDR_P0_REG		        = 0x0A,
  nRF_RX_ADDR_P1_REG		        = 0x0B,
  nRF_RX_ADDR_P2_REG		        = 0x0C,
  nRF_RX_ADDR_P3_REG		        = 0x0D,
  nRF_RX_ADDR_P4_REG		        = 0x0E,
  nRF_RX_ADDR_P5_REG		        = 0x0F,
  nRF_TX_ADDR_REG			= 0x10,
  nRF_RX_PW_P0_REG		        = 0x11,
  nRF_RX_PW_P1_REG		        = 0x12,
  nRF_RX_PW_P2_REG		        = 0x13,
  nRF_RX_PW_P3_REG		        = 0x14,
  nRF_RX_PW_P4_REG		        = 0x15,
  nRF_RX_PW_P5_REG		        = 0x16,
  nRF_FIFO_STATUS_REG		        = 0x17,
}nRF_RegTypeDef;

/* NRF24l01 some bits definition */
#define RF_Config_PWR_UP_Bit            (1<<1)
#define RF_RX_FIFO_EMPTY_Bit		(1<<0)
#define RF_TX_FIFO_EMPTY_Bit		(1<<4)
#define RF_RX_FIFO_FULL_Bit		(1<<1)
#define RF_TX_FIFO_FULL_Bit		(1<<5)

/* NRF24l01 commands define */
#define RF_SendPayload_CMD              0xA0
#define RF_Sleep_State_CMD              0x50
#define RF_Flush_TX_CMD                 0xE1
#define RF_Flush_RX_CMD                 0xE2
#define RF_RX_PAYLOAD_CMD		0x61

/* Error handling */
typedef enum
{
	nRF24L01_SUCCESS = 0,
	nRF24L01_DATA_SEND_NO_ACK_RECEIVED = 1,
	nRF24L01_DATA_SEND_ACK_RECEIVED_OK = 2,
	nRF_ERROR_CHIP_NOT_RESPONDING = 3,
	nRF24L01_NO_DATA_RECEIVED = 0xFF
}nRF_ERROR_TypeDef;

typedef enum
{
  nRF_Power_Off			        = 0x00,
  nRF_Power_On			        = 0x02,				  
}nRF_PowerTypeDef;

typedef enum
{
  nRF_Config_IRQ_RX_On		        = 0x00,
  nRF_Config_IRQ_RX_Off	                = 0x40,
}nRF_RX_IRQ_Config_TypeDef;

typedef enum
{
  nRF_Config_IRQ_TX_On		        = 0x00,
  nRF_Config_IRQ_TX_Off	                = 0x20,
}nRF_TX_IRQ_Config_TypeDef;

typedef enum
{
  nRF_Config_IRQ_Max_RT_On              = 0x00,
  nRF_Config_IRQ_Max_RT_Off             = 0x10,
}nRF_RT_IRQ_Config_TypeDef;

typedef enum
{
  nRF_CRC_Off                           = 0x00,
  nRF_CRC8                              = 0x08,
  nRF_CRC16                             = 0x0C,
}nRF_CRC_Mode_TypeDef;

typedef enum
{
  nRF_Mode_TX                           = 0x00,
  nRF_Mode_RX                           = 0x01,
}nRF_Mode_TypeDef;

typedef enum
{
  nRF_Pipe_All_ACK_Disable              = 0x00,
  nRF_Pipe_All_ACK_Enable	        = 0x3F,
  nRF_Pipe0_ACK_Enable		        = 0x01,
  nRF_Pipe1_ACK_Enable		        = 0x02,
  nRF_Pipe2_ACK_Enable		        = 0x04,
  nRF_Pipe3_ACK_Enable		        = 0x08,
  nRF_Pipe4_ACK_Enable		        = 0x10,
  nRF_Pipe5_ACK_Enable		        = 0x20,
}nRF_Pipes_ACK_Enable_TypeDef;

typedef enum
{
  nRF_Pipe_All_Enable		        = 0x3F,
  nRF_Pipe_All_Disable		        = 0x00,
  nRF_Pipe0_Enable			= 0x01,
  nRF_Pipe1_Enable			= 0x02,
  nRF_Pipe2_Enable			= 0x04,
  nRF_Pipe3_Enable			= 0x08,
  nRF_Pipe4_Enable			= 0x10,
  nRF_Pipe5_Enable			= 0x20,
}nRF_Pipes_Enable_TypeDef;
  
typedef enum
{
  nRF_Setup_3_Byte_Adress               = 0x01,
  nRF_Setup_4_Byte_Adress               = 0x02, 
  nRF_Setup_5_Byte_Adress               = 0x03,
}nRF_AddressLen_TypeDef;

typedef enum
{
  nRF_TX_Power_High		        = 0x06,
  nRF_TX_Power_Medium		        = 0x04,
  nRF_TX_Power_Low			= 0x02,
  nRF_TX_Power_Lowest		        = 0x00,
}nRF_TX_Power_TypeDef;

typedef enum
{
  nRF_Data_Rate_1Mbs		        = 0x00,
  nRF_Data_Rate_2Mbs		        = 0x08, 
}nRF_Data_Rate_TypeDef;

typedef enum 
{
	PTX = 0,
	PRX = 1
}RF_Mode;

typedef struct
{
	nRF_PowerTypeDef                        nRF_Power;	
	nRF_RX_IRQ_Config_TypeDef               nRF_RX_IRQ_Config;
        nRF_TX_IRQ_Config_TypeDef               nRF_TX_IRQ_Config;
        nRF_RT_IRQ_Config_TypeDef               nRF_RT_IRQ_Config;
	nRF_CRC_Mode_TypeDef                    nRF_CRC_Mode;
	nRF_Mode_TypeDef                        nRF_Mode;
	nRF_Pipes_ACK_Enable_TypeDef            nRF_Pipes_ACK_Enable;
	nRF_Pipes_Enable_TypeDef                nRF_Pipes_Enable;
	nRF_AddressLen_TypeDef                  nRF_AddressLen;
	char                                    nRF_Channel;
	nRF_TX_Power_TypeDef                    nRF_TX_Power;   
	nRF_Data_Rate_TypeDef                   nRF_Data_Rate;
	char *                                  nRF_RX_Adress_Pipe0;
	char *                                  nRF_RX_Adress_Pipe1;
	char                                    nRF_RX_Adress_Pipe2;
	char                                    nRF_RX_Adress_Pipe3;
	char                                    nRF_RX_Adress_Pipe4;
	char                                    nRF_RX_Adress_Pipe5;
	char *                                  nRF_TX_Adress;
	char                                    nRF_Payload_Size_Pipe0;
	char                                    nRF_Payload_Size_Pipe1;
	char                                    nRF_Payload_Size_Pipe2;
	char                                    nRF_Payload_Size_Pipe3;
	char                                    nRF_Payload_Size_Pipe4;
	char                                    nRF_Payload_Size_Pipe5;
	char                                    nRF_Auto_Retransmit_Delay;
	char                                    nRF_Auto_Retransmit_Count;

	/* Functions */
        void (*IO_CE_Func ) (char IO_State);
        char (*IO_IRQ_Func) (void);

	char (*WriteReg)(char reg, char * buf, char size);
	char (*ReadReg) (char reg, char * buf, char size);  
}RF_InitTypeDef;

typedef struct
{
        nRF_ERROR_TypeDef (*Init)            (RF_InitTypeDef * InitStruct);
        nRF_ERROR_TypeDef (*SendPayload)        (char * data, char size);
        nRF_ERROR_TypeDef (*SendPayloadACK)     (char * data, char size);
        nRF_ERROR_TypeDef (*Receive_Data)       (char * data, char size);
        nRF_ERROR_TypeDef (*Sleep)      (void);
        nRF_ERROR_TypeDef (*WakeUp)     (void);    
}RF_ClassTypeDef;

#define RF_RX_DR_IRQ_CLEAR			0x40
#define RF_TX_DS_IRQ_CLEAR			0x20
#define RF_MAX_RT_IRQ_CLEAR			0x10

#define ANT_ENABLE              RF_InitStruct->IO_CE_Func (1)
#define ANT_DISABLE             RF_InitStruct->IO_CE_Func (0)

nRF_ERROR_TypeDef RF_Init(RF_InitTypeDef * InitStruct);
nRF_ERROR_TypeDef RF_SendPayload (char * data, char size, char RX_Enable);
nRF_ERROR_TypeDef RF_SendPayloadACK (char * data, char size, char RX_Enable);
nRF_ERROR_TypeDef RF_Receive_Data(char * data, char size);
nRF_ERROR_TypeDef RF_ChangeFreqChannel (char channel);
nRF_ERROR_TypeDef RF_Sleep (void);
nRF_ERROR_TypeDef RF_WakeUp (void);
RF_InitTypeDef * RF_InitStructGet (void);
unsigned short RF_GetChannelFreqInMHz (void);
unsigned char RF_Switch_RX_TX (nRF_Mode_TypeDef Mode);

static const struct
{
  nRF_ERROR_TypeDef (* Init) (RF_InitTypeDef * InitStruct);
  nRF_ERROR_TypeDef (* SendPayload) (char * data, char size, char RX_Enable);
  nRF_ERROR_TypeDef (* SendPayloadACK) (char * data, char size, char RX_Enable);
  nRF_ERROR_TypeDef (* Receive_Data) (char * data, char size);
  nRF_ERROR_TypeDef (* ChangeFreqChannel) (char channel);
  nRF_ERROR_TypeDef (* Sleep) (void);
  nRF_ERROR_TypeDef (* WakeUp) (void);
  RF_InitTypeDef * (* InitStructGet) (void);
  unsigned char (* Switch_RX_TX) (nRF_Mode_TypeDef Mode);
  unsigned short (* GetChannelFreqInMHz) (void);
}nRF24L01 = { RF_Init, RF_SendPayload, RF_SendPayloadACK, RF_Receive_Data, RF_ChangeFreqChannel, RF_Sleep, RF_WakeUp, RF_InitStructGet, RF_Switch_RX_TX, RF_GetChannelFreqInMHz };

#endif
