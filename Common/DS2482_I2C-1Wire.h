#ifndef DS2482_H
#define DS2482_H

#include <stdint.h>

enum ds2482_active_pullup {
	ACT_PULLUP_ENABLE = 0x01,
	ACT_PULLUP_DISABLE = 0x10,
};

enum ds2482_strong_pullup {
	STR_PULLUP_ENABLE = 0x04,
	STR_PULLUP_DISABLE = 0x40,
};

enum ds2482_bus_speed {
	BUS_FAST = 0x08,
	BUS_NORM = 0x80,
};

#ifndef ONE_WIRE_SUCCESS
#define ONE_WIRE_SUCCESS		0x00
#define ONE_WIRE_NO_ECHO		0x01
#define ONE_WIRE_BUS_LOW		0x02
#define ONE_WIRE_BUS_BUSY		0x03
#define ONE_WIRE_CRC_WRONG		0x04
#define ONE_WIRE_INTERFACE_ERROR	0x05
#endif

struct ds2482_t {
	enum ds2482_active_pullup active_pullup;
	enum ds2482_strong_pullup strong_pullup;
	enum ds2482_bus_speed bus_speed;
	uint8_t attemps; //10..100

	/* I2C Interface Functions */
	uint8_t (*write)(uint8_t reg, uint8_t *buf, uint8_t size);
	uint8_t (*read)(uint8_t *reg, uint8_t regsize,
		uint8_t *buf, uint8_t size);
	uint8_t (*pooling_write) (uint8_t reg, uint8_t *value,
		uint8_t attempts, uint8_t flagToPooling);
};

uint8_t ds2482_init(struct ds2482_t *dev);
uint8_t ds2482_one_wire_reset(struct ds2482_t *dev);
uint8_t ds2482_one_wire_write(struct ds2482_t *dev, uint8_t value);
uint8_t ds2482_one_wire_read(struct ds2482_t *dev);

#endif /* DS2482_H */
