#include "nrf24l01.h"
#include <string.h>

static uint8_t NRF24L01_BLE_PreparePacket (uint8_t * buf, uint8_t * name, int32_t * data);
static inline uint8_t btLeWhitenStart(uint8_t chan);
static void btLeWhiten(uint8_t * data, uint8_t len, uint8_t whitenCoeff);
static uint8_t * BLE_Address (void);
static void btLeCrc(uint8_t * data, uint8_t len, uint8_t * dst);
static uint8_t swapbits(uint8_t a);
uint8_t * nrf_mac;

void NRF24L01_ConfgureForBLE (RF_InitTypeDef * RF_InitStruct, uint8_t * mac)
{
  memset((char*)RF_InitStruct, 0, sizeof(RF_InitTypeDef));
  nrf_mac = mac;
  RF_InitStruct->nRF_Power = nRF_Power_On;
  RF_InitStruct->nRF_Mode = nRF_Mode_TX;
  RF_InitStruct->nRF_CRC_Mode = nRF_CRC_Off;
  RF_InitStruct->nRF_RX_IRQ_Config = nRF_Config_IRQ_RX_On;
  RF_InitStruct->nRF_TX_IRQ_Config = nRF_Config_IRQ_TX_On;
  RF_InitStruct->nRF_RT_IRQ_Config = nRF_Config_IRQ_Max_RT_Off;
  RF_InitStruct->nRF_Pipes_Enable = nRF_Pipe0_Enable;
  RF_InitStruct->nRF_Auto_Retransmit_Count = 0;
  RF_InitStruct->nRF_Auto_Retransmit_Delay = 0;
  RF_InitStruct->nRF_Payload_Size_Pipe0 = 32;
  RF_InitStruct->nRF_AddressLen = nRF_Setup_4_Byte_Adress;
  RF_InitStruct->nRF_TX_Power = nRF_TX_Power_High;
  RF_InitStruct->nRF_Data_Rate = nRF_Data_Rate_1Mbs;
  RF_InitStruct->nRF_RX_Adress_Pipe0 = (char*)BLE_Address();
  RF_InitStruct->nRF_RX_Adress_Pipe1 = 0;
  RF_InitStruct->nRF_TX_Adress = RF_InitStruct->nRF_RX_Adress_Pipe0;
  RF_InitStruct->nRF_Channel = 2;
}

void NRF24L01_BLE_Encode (uint8_t * packet, uint8_t len, uint8_t channel, uint8_t * name, int32_t * data)
{
  //length is of packet, including crc. pre-populate crc in packet with initial crc value!
  uint8_t i, dataLen;
  
  dataLen = NRF24L01_BLE_PreparePacket(packet, name, data) - 3;
  
  btLeCrc(packet, dataLen, packet + dataLen);
  
  for(i = 0; i < 3; i++, dataLen++)
    packet[dataLen] = swapbits(packet[dataLen]);
  
  btLeWhiten(packet, len, btLeWhitenStart(channel));
  
  for(i = 0; i < len; i++) 
    packet[i] = swapbits(packet[i]);
}

static uint8_t NRF24L01_BLE_PreparePacket (uint8_t * buf, uint8_t * name, int32_t * data)
{
  uint8_t L = 0;
  buf[L++] = 0x42;	//PDU type, given address is random
  buf[L++] = data ? 0x11+10 : 0x11;	//17 or 11 bytes of payload
  memcpy(&buf[L], nrf_mac, 6);
  L += 6;

  buf[L++] = 2;		//flags (LE-only, limited discovery mode)
  buf[L++] = 0x01;
  buf[L++] = 0x05;
  
  // name
  buf[L++] = 0x07;
  buf[L++] = 0x08;
  memcpy(&buf[L], name, 6);
  L += 6;
  
  buf[L++] = 9;
  buf[L++] = 0xFF;
  
  //data
  if (data)
  {
    memcpy(&buf[L], (char*)data++, sizeof(int32_t));
    L += sizeof(int32_t);
    memcpy(&buf[L], (char*)data, sizeof(int32_t));
    L += sizeof(int32_t);
  }
  
  buf[L++] = 0x55;	//CRC start value: 0x555555
  buf[L++] = 0x55;
  buf[L++] = 0x55;
  
  return L;
}

static inline uint8_t btLeWhitenStart(uint8_t chan)
{
	//the value we actually use is what BT'd use left shifted one...makes our life easier
	return swapbits(chan) | 2;	
}

static void btLeWhiten(uint8_t * data, uint8_t len, uint8_t whitenCoeff)
{
  uint8_t  m;
  
  while(len--)
  {
    for(m = 1; m; m <<= 1)
    {
      if(whitenCoeff & 0x80)
      {
        whitenCoeff ^= 0x11;
        (*data) ^= m;
      }
      whitenCoeff <<= 1;
    }
    data++;
  }
}

static uint8_t * BLE_Address (void)
{
  static uint8_t ble_address[4];

  ble_address[0] = swapbits(0x8E);
  ble_address[1] = swapbits(0x89);
  ble_address[2] = swapbits(0xBE);
  ble_address[3] = swapbits(0xD6);

  return ble_address;
}

static void btLeCrc(uint8_t * data, uint8_t len, uint8_t * dst)
{
	uint8_t v, t, d;

	while(len--)
	{
		d = *data++;
		for(v = 0; v < 8; v++, d >>= 1)
		{
			t = dst[0] >> 7;
			
			dst[0] <<= 1;
			if(dst[1] & 0x80) dst[0] |= 1;
			dst[1] <<= 1;
			if(dst[2] & 0x80) dst[1] |= 1;
			dst[2] <<= 1;
			
			if(t != (d & 1))
			{
				dst[2] ^= 0x5B;
				dst[1] ^= 0x06;
			}
		}	
	}
}

static uint8_t swapbits (uint8_t a)
{
  uint8_t v = 0;
  
  if(a & 0x80) v |= 0x01;
  if(a & 0x40) v |= 0x02;
  if(a & 0x20) v |= 0x04;
  if(a & 0x10) v |= 0x08;
  if(a & 0x08) v |= 0x10;
  if(a & 0x04) v |= 0x20;
  if(a & 0x02) v |= 0x40;
  if(a & 0x01) v |= 0x80;
  
  return v;
}


