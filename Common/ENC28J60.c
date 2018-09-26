#include "ENC28J60.h"

/* Definitions */
#define ADDR_MASK        0x1F
#define BANK_MASK        0x60
//--------------------------------------------------
//All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
//--------------------------------------------------
// Bank 0 registers
#define ERDPT           (0x00|0x00)
#define EWRPT           (0x02|0x00)
#define ETXST           (0x04|0x00)
#define ETXND           (0x06|0x00)
#define ERXST           (0x08|0x00)
#define ERXND           (0x0A|0x00)
#define ERXRDPT         (0x0C|0x00)
//--------------------------------------------------
// Bank 1 registers
#define EPMM0           (0x08|0x20)
#define EPMM1           (0x09|0x20)
#define EPMCS   (0x10|0x20)
#define ERXFCON         (0x18|0x20)
#define EPKTCNT   (0x19|0x20)
//--------------------------------------------------
// Bank 2 registers
#define MACON1          (0x00|0x40|0x80)
#define MACON2          (0x01|0x40|0x80)
#define MACON3          (0x02|0x40|0x80)
#define MACON4          (0x03|0x40|0x80)
#define MABBIPG         (0x04|0x40|0x80)
#define MAIPG   (0x06|0x40|0x80)
#define MAMXFL          (0x0A|0x40|0x80)
#define MIREGADR  (0x14|0x40|0x80)
#define MIWR            (0x16|0x40|0x80)
//--------------------------------------------------
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define MISTAT           (0x0A|0x60|0x80)
//--------------------------------------------------
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
//--------------------------------------------------
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
//--------------------------------------------------
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
//------------------------------------------------
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
//????????
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
//--------------------------------------------------
// ENC28J60 ECON1 Register Bit Definitions
#define ECON2_PKTDEC     0x40
//--------------------------------------------------
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
//--------------------------------------------------
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
//--------------------------------------------------
#define MISTAT_BUSY      0x01
//--------------------------------------------------
// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14
//--------------------------------------------------
#define PHCON2_HDLDIS    0x0100
//--------------------------------------------------
// PHLCON
#define PHLCON_LACFG3  0x0800
#define PHLCON_LACFG2  0x0400
#define PHLCON_LACFG1  0x0200
#define PHLCON_LACFG0  0x0100
#define PHLCON_LBCFG3  0x0080
#define PHLCON_LBCFG2  0x0040
#define PHLCON_LBCFG1  0x0020
#define PHLCON_LBCFG0  0x0010
#define PHLCON_LFRQ1  0x0008
#define PHLCON_LFRQ0  0x0004
#define PHLCON_STRCH  0x0002
//--------------------------------------------------
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF
//--------------------------------------------------
#define RXSTART_INIT        0x0000  // start of RX buffer, room for 2 packets
#define RXSTOP_INIT         0x0BFF  // end of RX buffer
//--------------------------------------------------
#define TXSTART_INIT        0x0C00  // start of TX buffer, room for 1 packet
#define TXSTOP_INIT         0x11FF  // end of TX buffer
//--------------------------------------------------
#define MAX_FRAMELEN      1500
#define MAX_RESET_ATMP		1000
//--------------------------------------------------

/* Local driver */
enc28j60_t * enc28j60;

/* Local functions prototypes */
static void enc28j60_writeOp (uint8_t op, uint8_t address, uint8_t data);
static uint8_t enc28j60_readOp (uint8_t op, uint8_t address);
static void enc28j60_readBuf(uint16_t len, uint8_t* data);
static void enc28j60_SetBank (uint8_t addres);
static void enc28j60_writeRegByte(uint8_t addres,uint8_t data);
static void enc28j60_writeBuf(uint16_t len, uint8_t* data);
static uint8_t enc28j60_readRegByte(uint8_t addres);
static void enc28j60_writeReg(uint8_t addres,uint16_t data);
static void enc28j60_writePhy(uint8_t addres, uint16_t data);

/* Local variables */
uint8_t Enc28j60Bank;
int gNextPacketPtr;

/* Public fuctions */
enc28j60_t * enc28j60_Init (enc28j60_t * driver)
{
	if (driver)
		enc28j60 = driver;
	
	if (!enc28j60) return 0;
	
	enc28j60_writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	
	uint32_t atmp = MAX_RESET_ATMP;
	while(!enc28j60_readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY && --atmp){}
	if (atmp == 0) return 0;
		
	enc28j60_writeReg(ERXST, RXSTART_INIT);
	enc28j60_writeReg(ERXRDPT, RXSTART_INIT);
	enc28j60_writeReg(ERXND, RXSTOP_INIT);
	enc28j60_writeReg(ETXST, TXSTART_INIT);
	enc28j60_writeReg(ETXND, TXSTOP_INIT);

	/* Enable Broadcast */
	enc28j60_writeRegByte(ERXFCON, enc28j60_readRegByte(ERXFCON) | ERXFCON_BCEN);	

	/* MAC layer init */
	enc28j60_writeRegByte(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	enc28j60_writeRegByte(MACON2, 0x00);
	enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
	enc28j60_writeReg(MAIPG, 0x0C12);
	enc28j60_writeRegByte(MABBIPG, 0x12);//frame interval
	enc28j60_writeReg(MAMXFL, MAX_FRAMELEN);//max frame size
	enc28j60_writeRegByte(MAADR5, enc28j60->mac_address[0]);//Set MAC addres
	enc28j60_writeRegByte(MAADR4, enc28j60->mac_address[1]);
	enc28j60_writeRegByte(MAADR3, enc28j60->mac_address[2]);
	enc28j60_writeRegByte(MAADR2, enc28j60->mac_address[3]);
	enc28j60_writeRegByte(MAADR1, enc28j60->mac_address[4]);
	enc28j60_writeRegByte(MAADR0, enc28j60->mac_address[5]);
		
	/* PHY layer init */
	enc28j60_writePhy(PHCON2, PHCON2_HDLDIS);//disable loopback
	enc28j60_writePhy(PHLCON, PHLCON_LACFG2 | PHLCON_LBCFG2 | PHLCON_LBCFG1 | PHLCON_LBCFG0 | PHLCON_LFRQ0 | PHLCON_STRCH);
	enc28j60_SetBank(ECON1);
  enc28j60_writeOp(ENC28J60_BIT_FIELD_SET,EIE,EIE_INTIE|EIE_PKTIE);
  enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);//enable package receive
		
	return enc28j60;
}

static void enc28j60_writeOp (uint8_t op, uint8_t address, uint8_t data)
{
	enc28j60->write_func(op | (address & ADDR_MASK), &data, 1);
}

static uint8_t enc28j60_readOp (uint8_t op, uint8_t address)
{
	uint8_t res[2];
	if (address & 0x80)
	{	
		enc28j60->read_func(op | (address & ADDR_MASK), res, 2);
		return res[1];
	}
	enc28j60->read_func(op | (address & ADDR_MASK), res, 1);
	return res[0];
}

static void enc28j60_readBuf(uint16_t len, uint8_t* data)
{
 enc28j60->read_func(ENC28J60_READ_BUF_MEM, data, len);
}

static void enc28j60_writeBuf(uint16_t len, uint8_t* data)
{
	enc28j60->write_func(ENC28J60_WRITE_BUF_MEM, data, len);
}

static void enc28j60_SetBank (uint8_t addres)
{
 if ((addres&BANK_MASK) != Enc28j60Bank)
 {
  enc28j60_writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
  Enc28j60Bank = addres&BANK_MASK;
  enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank >> 5);
 }
}

static void enc28j60_writeRegByte(uint8_t addres,uint8_t data)
{
 enc28j60_SetBank(addres);
 enc28j60_writeOp(ENC28J60_WRITE_CTRL_REG, addres, data);
}

static uint8_t enc28j60_readRegByte(uint8_t addres)
{
	uint8_t res;
 enc28j60_SetBank(addres);
 res = enc28j60_readOp(ENC28J60_READ_CTRL_REG, addres);
	return res;
}

static void enc28j60_writeReg(uint8_t addres,uint16_t data)
{
 enc28j60_writeRegByte(addres, data);
 enc28j60_writeRegByte(addres + 1, data >> 8);
}

static void enc28j60_writePhy(uint8_t addres, uint16_t data)

{
  enc28j60_writeRegByte(MIREGADR, addres);
  enc28j60_writeReg(MIWR, data);
  while(enc28j60_readRegByte(MISTAT)&MISTAT_BUSY){}
}

uint16_t enc28j60_packetReceive(uint8_t* buf, uint16_t buflen) //use 'buf = NULL' if need only amount of bytes received
{
	uint16_t len = 0;
	if(enc28j60_readRegByte(EPKTCNT) > 0)
	{
		//header reading
		struct{
			uint16_t nextPacket;
			uint16_t byteCount;
			uint16_t status;
		} header;
		enc28j60_writeReg(ERDPT, gNextPacketPtr);
		enc28j60_readBuf(sizeof(header), (uint8_t*)&header);		
		len = header.byteCount - 4;//remove the CRC count
		if (!buf) return len;
		gNextPacketPtr = header.nextPacket;
		if(len > buflen) len = buflen;
		if((header.status & 0x80) == 0) len = 0;
		else enc28j60_readBuf(len, buf);		
		buf[len] = 0;
		if(gNextPacketPtr - 1 > RXSTOP_INIT)
			enc28j60_writeReg(ERXRDPT, RXSTOP_INIT);
		else
			enc28j60_writeReg(ERXRDPT, gNextPacketPtr - 1);
		enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	}
	return len;
}

void enc28j60_packetSend(uint8_t* buf, uint16_t buflen)
{
	while(!enc28j60_readOp(ENC28J60_READ_CTRL_REG, ECON1)&ECON1_TXRTS)
	{
		if(enc28j60_readRegByte(EIR)&EIR_TXERIF)
		{
			enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
			enc28j60_writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		}
	}
	
	enc28j60_writeReg(EWRPT, TXSTART_INIT);
	enc28j60_writeReg(ETXND, TXSTART_INIT + buflen);
	enc28j60_writeBuf(1, (uint8_t*)"\x00");
	enc28j60_writeBuf(buflen, buf);
	enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}

uint8_t enc28j60_numPacketsRx (void)
{
	return enc28j60_readRegByte(EPKTCNT);
}
