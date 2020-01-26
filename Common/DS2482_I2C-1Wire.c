#include "DS2482_I2C-1Wire.h"

#define DS2482_CONFIG_REG		0xC3
#define DS2482_BUS_RESET		0xB4
#define DS2482_STATUS_REG			0xF0
#define DS2482_DATA_REG			0xE1
#define DS2482_DRST			0xF0
#define DS2482_WCFG			0xD2
#define DS2482_1WWB			0xA5
#define DS2482_1WRB			0x96

// Status register bit definitions
#define STATUS_1WB			0x01
#define STATUS_PPD			0x02
#define STATUS_SD			0x04
#define STATUS_LL			0x08
#define STATUS_RST			0x10
#define STATUS_SBR			0x20
#define STATUS_TSB			0x40
#define STATUS_DIR			0x80

uint8_t ds2482_init(struct ds2482_t *dev)
{
	uint8_t config = dev->active_pullup | dev->bus_speed |
			dev->strong_pullup | (1 << 5);

	if (dev->write(DS2482_DRST, 0, 0))
		return ONE_WIRE_INTERFACE_ERROR;

	/* Write configuration */
	dev->write(DS2482_WCFG, &config, 1);

	return ONE_WIRE_SUCCESS;
}

uint8_t ds2482_one_wire_reset(struct ds2482_t *dev)
{
	uint8_t value = DS2482_BUS_RESET;
	uint8_t ret = dev->pooling_write(0, &value, dev->attemps, STATUS_PPD);
	return ret == ONE_WIRE_SUCCESS ? ONE_WIRE_SUCCESS : value &
		STATUS_PPD ? ONE_WIRE_SUCCESS : ONE_WIRE_NO_ECHO;
}

uint8_t ds2482_one_wire_write(struct ds2482_t *dev, uint8_t value)
{
	return dev->pooling_write(DS2482_1WWB, &value, dev->attemps, STATUS_1WB);
}

uint8_t ds2482_one_wire_read(struct ds2482_t *dev)
{
	const uint8_t data[2] = { DS2482_DATA_REG, DS2482_DATA_REG };
	uint8_t res;

	dev->pooling_write(DS2482_1WRB, 0, dev->attemps, STATUS_1WB);

	dev->read((uint8_t*)data, sizeof(data), &res, 1);

	return res;
}

