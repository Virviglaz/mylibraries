#include "DS2482_I2C-1Wire.h"

#define DS2482_CONFIG_REG		0xC3
#define DS2482_BUS_RESET		0xB4
#define DS2482_STATUS_REG		0xF0
#define DS2482_DATA_REG			0xE1
#define DS2482_DRST			0xF0
#define DS2482_WCFG			0xD2
#define DS2482_1WWB			0xA5
#define DS2482_1WRB			0x96
#define DS2482_SRP			0xE1

// Status register bit definitions
#define STATUS_1WB			0x01
#define STATUS_PPD			0x02
#define STATUS_SD			0x04
#define STATUS_LL			0x08
#define STATUS_RST			0x10
#define STATUS_SBR			0x20
#define STATUS_TSB			0x40
#define STATUS_DIR			0x80

/*
 * The whole reset-respond cycle takes not more than 1ms.
 * Single i2c read cycle at 100kHz takes 0.3ms.
 * Therefore, 3 attempts is enough to detect
 * slave device presents on the bus.
 * Use 5 to be safe.
 */
#define I2C_MAX_READ_ATTEMPS		5

static uint8_t ds2482_reset(const struct ds2482_t *dev)
{
	uint8_t rst_cmd = DS2482_DRST;
	uint8_t config_cmd[] = {
		DS2482_WCFG, dev->active_pullup | \
			     dev->bus_speed | \
			     dev->strong_pullup | (1 << 5) };
	if (dev->write(&rst_cmd, sizeof(rst_cmd)))
		return ONE_WIRE_INTERFACE_ERROR;

	/* Write configuration */
	if (dev->write(config_cmd, sizeof(config_cmd)))
		return ONE_WIRE_INTERFACE_ERROR;

	return ONE_WIRE_SUCCESS;
}

uint8_t ds2482_one_wire_reset(const struct ds2482_t *dev)
{
	uint8_t reg = DS2482_BUS_RESET;
	uint8_t attmp = I2C_MAX_READ_ATTEMPS;

	if (ds2482_reset(dev))
		return ONE_WIRE_INTERFACE_ERROR;

	if (dev->write(&reg, sizeof(reg)))
		return ONE_WIRE_INTERFACE_ERROR;

	/* Repeat until STATUS_PPD is set */
	do {
		uint8_t status;
		if (dev->read(&status, sizeof(status)))
			return ONE_WIRE_INTERFACE_ERROR;

		if (status & STATUS_PPD)
			return ONE_WIRE_SUCCESS;
	} while (attmp--);

	return ONE_WIRE_NO_ECHO;
}

uint8_t ds2482_one_wire_write(const struct ds2482_t *dev, uint8_t value)
{
	uint8_t attmp = I2C_MAX_READ_ATTEMPS;
	uint8_t reg[] = { DS2482_1WWB, value };

	if (dev->write(reg, sizeof(reg)))
		return ONE_WIRE_INTERFACE_ERROR;

	/* Repeat until STATUS_1WB is reset */
	do {
		uint8_t status;
		if (dev->read(&status, sizeof(status)))
			return ONE_WIRE_INTERFACE_ERROR;

		if ((status & STATUS_1WB) == 0)
			return ONE_WIRE_SUCCESS;
	} while (attmp--);

	return ONE_WIRE_INTERFACE_ERROR;
}

uint8_t ds2482_one_wire_read(const struct ds2482_t *dev)
{
	uint8_t attmp = I2C_MAX_READ_ATTEMPS;
	uint8_t read_cmd = DS2482_1WRB;
	uint8_t write_cmd[] = { DS2482_SRP, DS2482_DATA_REG };
	uint8_t status;

	if (dev->write(&read_cmd, sizeof(read_cmd)))
		return ONE_WIRE_INTERFACE_ERROR;

	/* Repeat until STATUS_1WB is reset */
	do {
		uint8_t status;
		if (dev->read(&status, sizeof(status)))
			return ONE_WIRE_INTERFACE_ERROR;

		if ((status & STATUS_1WB) == 0)
			break;
	} while (attmp--);

	if (dev->read_at(write_cmd, sizeof(write_cmd),
		&read_cmd, sizeof(read_cmd)))
		return ONE_WIRE_INTERFACE_ERROR;

	return read_cmd;
}

