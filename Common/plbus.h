#ifndef __PLBUS_H__
#define __PLBUS_H__

#include <stdint.h>
#include <stdbool.h>

#define PLBUS_INIT(address, rx_dst) {	.own_address = address, \
										.rx_buffer = rx_dst, \
										.rx_buffer_size = sizeof(rx_dst), \
										.internal = { 0 } }

#define PLBUS_CRC_ERROR					(1 << 1)

typedef struct {
	uint16_t own_address;
	uint8_t *rx_buffer;
	uint16_t rx_buffer_size;
	void *private_data;
	struct {
		volatile uint16_t rx_cnt;
		bool in_sync;
		uint16_t crc_err;
	} internal;
} plbus_t;

#ifdef __cplusplus
    extern "C" {
#endif

typedef uint16_t (*plbus_cb_t)(uint8_t *data, uint16_t size);

void plbus_init(plbus_t *bus);

void plbus_put(plbus_t *bus, uint8_t byte);

uint16_t plbus_poll(plbus_t *bus, plbus_cb_t on_receive);

uint16_t plbus_send(plbus_t *bus,
					uint16_t dst,
					uint8_t *data,
					uint16_t size,
					plbus_cb_t on_transmit);

#ifdef __cplusplus
    }
#endif

#endif /* __PLBUS_H__ */
