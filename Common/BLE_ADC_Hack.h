#ifndef BLE_ADC_Hack_H
#define BLE_ADC_Hack_H

#include "nrf24l01.h"

void NRF24L01_ConfgureForBLE (RF_InitTypeDef * RF_InitStruct, uint8_t * mac);
void NRF24L01_BLE_Encode (uint8_t * packet, uint8_t len, uint8_t * name, int32_t * data, uint8_t isAndroidDevice);
void NRF24L01_BLE_Decode (uint8_t * packet);
uint8_t NRF24L01_BLE_GetFreqChannel (void);

static const struct
{
  void (* Confgure) (RF_InitTypeDef * RF_InitStruct, uint8_t * mac);
  void (* Encode) (uint8_t * packet, uint8_t len, uint8_t * name, int32_t * data, uint8_t isAndroidDevice);
  void (* Decode) (uint8_t * packet);
  uint8_t (* GetFreqChannel) (void);
}BLE = { NRF24L01_ConfgureForBLE, NRF24L01_BLE_Encode, NRF24L01_BLE_Decode, NRF24L01_BLE_GetFreqChannel };

#endif
