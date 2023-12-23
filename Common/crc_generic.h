#ifndef __CRC_GENERIC_H__
#define __CRC_GENERIC_H__
#include <stdint.h>

uint32_t crc32(uint8_t *buf, uint32_t len);
uint16_t crc16(uint8_t *buf, uint16_t len);
uint8_t Crc8Dallas(uint8_t *buf, uint16_t len);
uint8_t Crc8(uint8_t *buf, uint16_t len);
#endif /* __CRC_GENERIC_H__ */
