#ifndef ENC28J60_H
#define ENC28J60_H

#include <stdint.h>

typedef struct
{
  uint8_t (* write_func) (uint8_t reg, uint8_t * buf, uint16_t size);
  uint8_t (* read_func) (uint8_t reg, uint8_t * buf, uint16_t size);
	uint8_t * mac_address;
}enc28j60_t;

/* Public fuctions prototypes */
enc28j60_t * enc28j60_Init (enc28j60_t * this);
uint16_t enc28j60_packetReceive(uint8_t* buf, uint16_t buflen);
void enc28j60_packetSend(uint8_t* buf, uint16_t buflen);
#endif
