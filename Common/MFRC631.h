#include "DataTypes.h"

typedef enum
{
	MFRC630_PROTO_ISO14443A_106_MILLER_MANCHESTER,
	MFRC630_PROTO_ISO14443A_212_MILLER_BPSK,
	MFRC630_PROTO_ISO14443A_424_MILLER_BPSK,
	MFRC630_PROTO_ISO14443A_848_MILLER_BPSK
}MFRC631_ProtocolTypeDef;

/* Init structure */
typedef struct
{
	/* Functions */
	void (*WriteData)(u8 reg, u8 * buf, u16 size);
	void (*ReadData) (u8 reg, u8 * buf, u16 size);
	void (*Delay_Func)(u16 ms);
	//u8 (*ReadIRQ)(void);
	
	/* Product Version Number */
	u8 Version;
	
	MFRC631_ProtocolTypeDef MFRC631_Protocol;
	
}MFRC631_StructTypeDef;

typedef enum
{
	Reg_Command							= 0x00,
	Reg_HostCtrl            = 0x01,
	Reg_FIFOControl         = 0x02,
	Reg_WaterLevel          = 0x03,
	Reg_FIFOLength          = 0x04,
	Reg_FIFOData            = 0x05,
	Reg_IRQ0                = 0x06,
	Reg_IRQ1                = 0x07,
	Reg_IRQ0En              = 0x08,
	Reg_IRQ1En              = 0x09,
	Reg_Error               = 0x0A,
	Reg_Status              = 0x0B,
	Reg_RxBitCtrl           = 0x0C,
	Reg_RxColl              = 0x0D,
	Reg_TControl            = 0x0E,
	Reg_T0Control           = 0x0F,
	Reg_T0ReloadHi          = 0x10,
	Reg_T0ReloadLo          = 0x11,
	Reg_T0CounterValHi      = 0x12,
	Reg_T0CounterValLo      = 0x13,
	Reg_T1Control						= 0x14,
	Reg_T1ReloadHi          = 0x15,
	Reg_T1ReloadLo          = 0x16,
	Reg_T1CounterValHi      = 0x17,
	Reg_T1CounterValLo      = 0x18,
	Reg_T2Control           = 0x19,
	Reg_T2ReloadHi          = 0x1A,
	Reg_T2ReloadLo          = 0x1B,
	Reg_T2CounterValHi      = 0x1C,
	Reg_T2CounterValLo      = 0x1D,
	Reg_T3Control           = 0x1E,
	Reg_T3ReloadHi          = 0x1F,
	Reg_T3ReloadLo          = 0x20,
	Reg_T3CounterValHi      = 0x21,
	Reg_T3CounterValLo      = 0x22,
	Reg_T4Control           = 0x23,
	Reg_T4ReloadHi          = 0x24,
	Reg_T4ReloadLo          = 0x25,
	Reg_T4CounterValHi      = 0x26,
	Reg_T4CounterValLo      = 0x27,
	Reg_DrvMod              = 0x28,
	Reg_TxAmp               = 0x29,
	Reg_DrvCon              = 0x2A,
	Reg_Txl                 = 0x2B,
	Reg_TxCrcPreset         = 0x2C,
	Reg_RxCrcPreset         = 0x2D,
	Reg_TxDataNum           = 0x2E,
	Reg_TxModWidth          = 0x2F,
	Reg_TxSym10BurstLen     = 0x30,
	Reg_TXWaitCtrl          = 0x31,
	Reg_TxWaitLo            = 0x32,
	Reg_FrameCon            = 0x33,
	Reg_RxSofD              = 0x34,
	Reg_RxCtrl              = 0x35,
	Reg_RxWait              = 0x36,
	Reg_RxThreshold         = 0x37,
	Reg_Rcv                 = 0x38,
	Reg_RxAna               = 0x39,
	Reg_RFU                 = 0x3A,
	Reg_SerialSpeed         = 0x3B,
	Reg_LFO_Trimm           = 0x3C,
	Reg_PLL_Ctrl						= 0x3D,
	Reg_PLL_DivOut          = 0x3E,
	Reg_LPCD_QMin           = 0x3F,
	Reg_LPCD_QMax           = 0x40,
	Reg_LPCD_IMin           = 0x41,
	Reg_LPCD_I_Result       = 0x42,
	Reg_LPCD_Q_Result       = 0x43,
	Reg_PadEn               = 0x44,
	Reg_PadOut              = 0x45,
	Reg_PadIn               = 0x46,
	Reg_SigOut              = 0x47,
	Reg_Version             = 0x7F	
}MFRC631_RegisterTypeDef;

// IRQ0EN register fields
#define MFRC630_IRQ0EN_IRQ_INV          		(1<<7)
#define MFRC630_IRQ0EN_HIALERT_IRQEN    		(1<<6)
#define MFRC630_IRQ0EN_LOALERT_IRQEN    		(1<<5)
#define MFRC630_IRQ0EN_IDLE_IRQEN       		(1<<4)
#define MFRC630_IRQ0EN_TX_IRQEN         		(1<<3)
#define MFRC630_IRQ0EN_RX_IRQEN         		(1<<2)
#define MFRC630_IRQ0EN_ERR_IRQEN        		(1<<1)
#define MFRC630_IRQ0EN_RXSOF_IRQEN      		(1<<0)

// IRQ1EN register fields.
#define MFRC630_IRQ1EN_IRQ_PUSHPULL        	(1<<7)
#define MFRC630_IRQ1EN_IRQ_PINEN           	(1<<6)
#define MFRC630_IRQ1EN_LPCD_IRQEN          	(1<<5)
#define MFRC630_IRQ1EN_TIMER4_IRQEN        	(1<<4)
#define MFRC630_IRQ1EN_TIMER3_IRQEN        	(1<<3)
#define MFRC630_IRQ1EN_TIMER2_IRQEN        	(1<<2)
#define MFRC630_IRQ1EN_TIMER1_IRQEN        	(1<<1)
#define MFRC630_IRQ1EN_TIMER0_IRQEN        	(1<<0)

// TIMER CONTROL
#define MFRC630_TCONTROL_STOPRX           	(1<<7)
#define MFRC630_TCONTROL_START_NOT        	(0x00<<4)
#define MFRC630_TCONTROL_START_TX_END     	(0x01<<4)
#define MFRC630_TCONTROL_START_LFO_WO     	(0x02<<4)
#define MFRC630_TCONTROL_START_LFO_WITH   	(0x03<<4)
#define MFRC630_TCONTROL_AUTO_RESTART     	(1<<3)
#define MFRC630_TCONTROL_CLK_13MHZ        	(0x00)
#define MFRC630_TCONTROL_CLK_211KHZ       	(0x01)
#define MFRC630_TCONTROL_CLK_UF_TA1       	(0x02)
#define MFRC630_TCONTROL_CLK_UF_TA2       	(0x03)

// IRQ0 register fields
#define MFRC630_IRQ0_SET                		(1<<7)
#define MFRC630_IRQ0_HIALERT_IRQ        		(1<<6)
#define MFRC630_IRQ0_LOALERT_IRQ        		(1<<5)
#define MFRC630_IRQ0_IDLE_IRQ           		(1<<4)
#define MFRC630_IRQ0_TX_IRQ             		(1<<3)
#define MFRC630_IRQ0_RX_IRQ             		(1<<2)
#define MFRC630_IRQ0_ERR_IRQ            		(1<<1)
#define MFRC630_IRQ0_RXSOF_IRQ          		(1<<0)

// IRQ1 register fields
#define MFRC630_IRQ1_SET                		(1<<7)
#define MFRC630_IRQ1_GLOBAL_IRQ         		(1<<6)
#define MFRC630_IRQ1_LPCD_IRQ           		(1<<5)
#define MFRC630_IRQ1_TIMER4_IRQ         		(1<<4)
#define MFRC630_IRQ1_TIMER3_IRQ         		(1<<3)
#define MFRC630_IRQ1_TIMER2_IRQ         		(1<<2)
#define MFRC630_IRQ1_TIMER1_IRQ         		(1<<1)
#define MFRC630_IRQ1_TIMER0_IRQ         		(1<<0)

// Status register values
#define MFRC630_STATUS_STATE_IDLE           0x00       //!< Status register; Idle
#define MFRC630_STATUS_STATE_TXWAIT         0x01       //!< Status register; Tx wait
#define MFRC630_STATUS_STATE_TRANSMITTING   0x03       //!< Status register; Transmitting.
#define MFRC630_STATUS_STATE_RXWAIT         0x05       //!< Status register; Rx wait.
#define MFRC630_STATUS_STATE_WAIT_FOR_DATA  0x06       //!< Status register; Waiting for data.
#define MFRC630_STATUS_STATE_RECEIVING      0x07       //!< Status register; Receiving data.
#define MFRC630_STATUS_STATE_NOT_USED       0x04       //!< Status register; Not used.
#define MFRC630_STATUS_CRYPTO1_ON           (1<<5)      //!< Status register; Crypto1 (MIFARE authentication) is on.

// Defines for MIFARE
#define MFRC630_MF_AUTH_KEY_A                   0x60  //!< A key_type for mifare auth.
#define MFRC630_MF_AUTH_KEY_B                   0x61  //!< A key_type for mifare auth.
#define MFRC630_MF_CMD_READ                     0x30  //!< To read a block from mifare card.
#define MFRC630_MF_CMD_WRITE                    0xA0  //!< To write a block to a mifare card.
#define MFRC630_MF_ACK                          0x0A  //!< Sent by cards to acknowledge an operation.

// Defines from ISO14443A
#define MFRC630_ISO14443_CMD_REQA               0x26  //!< request (idle -> ready)
#define MFRC630_ISO14443_CMD_WUPA               0x52  //!< wake up type a (idle / halt -> ready)
#define MFRC630_ISO14443_CAS_LEVEL_1            0x93  //!< Cascade level 1 for select.
#define MFRC630_ISO14443_CAS_LEVEL_2            0x95  //!< Cascade level 2 for select.
#define MFRC630_ISO14443_CAS_LEVEL_3            0x97  //!< Cascade level 3 for select.

// Define  command name                 hex     //  argument ; comment
#define MFRC630_CMD_IDLE                0x00    /*!< (no arguments) ; no action, cancels current command execution. */
#define MFRC630_CMD_LPCD                0x01    /*!< (no arguments) ; low-power card detection. */
#define MFRC630_CMD_LOADKEY             0x02    /*!< (keybyte1), (keybyte2), (keybyte3), (keybyte4), (keybyte5),
                                                     (keybyte6); reads a MIFARE key (size of 6 bytes) from FIFO buffer
                                                     and puts it into Key buffer. */
#define MFRC630_CMD_MFAUTHENT           0x03    /*!< 60h or 61h, (block address), (card serial number byte0), (card
                                                     serial number byte1), (card serial number byte2), (card serial
                                                     number byte3); performs the MIFARE standard authentication. */
#define MFRC630_CMD_RECEIVE             0x05    /*!< (no arguments) ; activates the receive circuit. */
#define MFRC630_CMD_TRANSMIT            0x06    /*!< bytes to send: byte1, byte2, ...;  transmits data from the FIFO
                                                     buffer. */
#define MFRC630_CMD_TRANSCEIVE          0x07    /*!< bytes to send: byte1, byte2, ....;  transmits data from the FIFO
                                                     buffer and automatically activates the receiver after transmission
                                                     finished. */
#define MFRC630_CMD_WRITEE2             0x08    /*!< addressH, addressL, data; gets one byte from FIFO buffer and
                                                     writes it to the internal EEPROM.*/
#define MFRC630_CMD_WRITEE2PAGE         0x09    /*!< (page Address), data0, [data1..data63]; gets up to 64 bytes (one
                                                     EEPROM page) from the FIFO buffer and writes it to the EEPROM. */
#define MFRC630_CMD_READE2              0x0A    /*!< addressH, address L, length; reads data from the EEPROM and copies
                                                     it into the FIFO buffer. */
#define MFRC630_CMD_LOADREG             0x0C    /*!< (EEPROM addressH), (EEPROM addressL), RegAdr, (number of Register
                                                     to be copied); reads data from the internal EEPROM and initializes
                                                     the MFRC630 registers. EEPROM address needs to be within EEPROM
                                                     sector 2. */
#define MFRC630_CMD_LOADPROTOCOL        0x0D    /*!< (Protocol number RX), (Protocol number TX); reads data from the
                                                     internal EEPROM and initializes the MFRC630 registers needed for a
                                                     Protocol change.*/
#define MFRC630_CMD_LOADKEYE2           0x0E    /*!< KeyNr; copies a key from the EEPROM into the key buffer. */
#define MFRC630_CMD_STOREKEYE2          0x0F    /*!< KeyNr, byte1, byte2, byte3, byte4, byte5, byte6; stores a MIFARE
                                                     key (size of 6 bytes) into the EEPROM.*/
#define MFRC630_CMD_READRNR             0x1C    /*!< (no arguments) ; Copies bytes from the Random Number generator
                                                    into the FIFO until the FiFo is full. */
#define MFRC630_CMD_SOFTRESET           0x1F    /*!< (no arguments) ; resets the MFRC630. */

typedef enum
{
	MFRC631_CMD_IDLE = 0x00,
	MFRC631_CMD_LPCD = 0x01,
	MFRC631_CMD_LOADKEY = 0x02,
	MFRC631_CMD_MFAUTHENT = 0x03,
	MFRC631_CMD_RECEIVE = 0x05,
	MFRC631_CMD_TRANSMIT = 0x06,
	MFRC631_CMD_TRANSCEIVE = 0x07,
	MFRC631_CMD_WRITEE2 = 0x08,
	MFRC631_CMD_WRITEE2PAGE = 0x09,
	MFRC631_CMD_READE2 = 0x0A,
	MFRC631_CMD_LOADREG = 0x0C,
	MFRC631_CMD_LOADPROTOCOL = 0x0D,
	MFRC631_CMD_LOADKEYE2 = 0x0E,
	MFRC631_CMD_STOREKEYE2 = 0x0F,
	MFRC631_CMD_READRNR = 0x1C,
	MFRC631_CMD_SOFTRESET = 0x1F
}MFRC631_CommandTypeDef;

// recommended register values from register 0x28 down.
// From AN11022: CLRC663 Quickstart Guide
// All the other protocols are also in there....
//! Correct settings for the CRC registers for ISO14443A data frames.
#define MFRC630_RECOM_14443A_CRC 0x18
//! Recommended register values for ISO1443A at 106 kbit/s with Miller / Manchester modulation.
#define MFRC630_RECOM_14443A_ID1_106 {0x8A, 0x08, 0x21, 0x1A, 0x18, 0x18, 0x0F, 0x27, 0x00, 0xC0, 0x12, 0xCF, 0x00, \
                                      0x04, 0x90, 0x32, 0x12, 0x0A}
//! Recommended register values for ISO1443A at 212 kbit/s with Miller / BPSK modulation.
#define MFRC630_RECOM_14443A_ID1_212 {0x8E, 0x12, 0x11, 0x06, 0x18, 0x18, 0x0F, 0x10, 0x00, 0xC0, 0x12, 0xCF, 0x00, \
                                      0x05, 0x90, 0x3F, 0x12, 0x02}
//! Recommended register values for ISO1443A at 424 kbit/s with Miller / BPSK modulation.
#define MFRC630_RECOM_14443A_ID1_424 {0x8E, 0x12, 0x11, 0x06, 0x18, 0x18, 0x0F, 0x08, 0x00, 0xC0, 0x12, 0xCF, 0x00, \
                                      0x06, 0x90, 0x3F, 0x12, 0x0A}
//! Recommended register values for ISO1443A at 848  kbit/s with Miller / BPSK modulation.
#define MFRC630_RECOM_14443A_ID1_848 {0x8F, 0xDB, 0x11, 0x06, 0x18, 0x18, 0x0F, 0x02, 0x00, 0xC0, 0x12, 0xCF, 0x00, \
                                      0x07, 0x90, 0x3F, 0x12, 0x02}
//! Enable CRC, should be ORed with `#MFRC630_RECOM_14443A_CRC`.
#define MFRC630_CRC_ON            1
//! Disable CRC, should be ORed with `#MFRC630_RECOM_14443A_CRC`.
#define MFRC630_CRC_OFF           0
#define MFRC630_TXDATANUM_DATAEN        (1<<3)

// Error register fields
//! An error appeared during the last EEPROM command.
#define MFRC630_ERROR_EE_ERR            (1<<7)
//! Data was written to FIFO while this shouldn't be done.
#define MFRC630_ERROR_FIFOWRERR         (1<<6)
//! Data was written to the FIFO while it was already full.
#define MFRC630_ERROR_FIFOOVL           (1<<5)
//! A valid SOF was received, but less then 4 bits of data were received.
#define MFRC630_ERROR_MINFRAMEERR       (1<<4)
//! No data available to be sent.
#define MFRC630_ERROR_NODATAERR         (1<<3)
//! A collision occured.
#define MFRC630_ERROR_COLLDET           (1<<2)
//! A protocol error occured.
#define MFRC630_ERROR_PROTERR           (1<<1)
//! A data integrity error occured.
#define MFRC630_ERROR_INTEGERR          (1<<0)
																			
/* Public functions declaration */
void MFRC631_Init (MFRC631_StructTypeDef * MFRC631_InitStruct);
u8 MFRC631_MIFARE_Auth (const u8 * uid, u8 key_type, u8 block);
u8 MFRC631_MIFARE_Read_Block(u8 block_address, u8 * dest);										
u8 MFRC631_MIFARE_Write_Block (u8 block_address, u8 * source);
void MFRC631_AN1102_Recommended_Registers (MFRC631_ProtocolTypeDef protocol);
u16 MFRC631_ISO14443a_REQA (void);
u16 MFRC631_ISO4443a_WUPA (void);
u8 MFRC631_ISO14443A_Select(u8 * uid, u8 * sak);
void MFRC631_EEPROM_Read (u8 * buffer, u16 address, u8 size);
