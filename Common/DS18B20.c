#include "DS18B20.h"

/*dev Commands list */
#define DS18B20_READ_ROM			0x33
#define DS18B20_SKIP_ROM			0xCC
#define DS18B20_SEARCH_ROM			0xF0
#define DS18B20_MATCH_ROM			0x55

#define DS18B20_CONVERT_T_CMD			0x44
#define DS18B20_WRITE_STRATCHPAD_CMD		0x4E
#define DS18B20_READ_STRATCHPAD_CMD		0xBE
#define DS18B20_COPY_STRATCHPAD_CMD		0x48
#define DS18B20_RECALL_E_CMD			0xB8
#define DS18B20_READ_POWER_SUPPLY_CMD		0xB4

#define DS18B20_STRATCHPAD_SIZE			0x09
#define DefaultResolution			Res_12bit

static uint8_t dallas_crc(uint8_t *buf, uint8_t len)
{
	uint8_t i, crc = 0;

	while (len--) {
		crc ^= *buf++;
		for (i = 0; i < 8; i++)
			crc = crc & 0x01 ? (crc >> 1) ^ 0x8C : crc >> 1;
	}

	return crc;
}

static void find_sensor(struct ds18b20_t *dev)
{
	uint8_t i;
	dev->interface->write(DS18B20_MATCH_ROM);

	for (i = 0; i != 8; i++) 
		dev->interface->write(dev->SN[i]);
}

/**
  * @brief  Perform writing data configuration to configuration register
  * @param  DS18B20: Sensor to configure
  * @retval One_Wire_Success or One wire error
  */
uint8_t ds18b20_init(struct ds18b20_t *dev)
{
	uint8_t ret = dev->interface->reset();
	if (ret)
		return ret;

	find_sensor(dev);

	dev->interface->write(DS18B20_WRITE_STRATCHPAD_CMD);
	dev->interface->write(dev->Th);
	dev->interface->write(dev->Tl);
	dev->interface->write((dev->res << 5) | 0x1F);

	return ret;
}

/**
  * @brief  Send start conversion command to corresponding sensor
  * @param  DS18B20: Sensor to start the conversion
  * @retval One_Wire_Success or One wire error
  */
uint8_t ds18b20_start(struct ds18b20_t *dev)
{
	uint8_t ret = dev->interface->reset();
	if (ret)
		return ret;

	find_sensor(dev);

	dev->interface->write(DS18B20_CONVERT_T_CMD);

	return ret;
}

/**
  * @brief  Gets conversion result and store it to sensor structure
  * @param  DS18B20: Sensor to read
  * @retval One_Wire_Success or One wire error
  */
uint8_t ds18b20_get_result(struct ds18b20_t *dev)
{
	uint8_t i;
	uint8_t buf[DS18B20_STRATCHPAD_SIZE];
	uint8_t ret = dev->interface->reset();
	if (ret)
		return ret;

	find_sensor(dev);

	dev->interface->write (DS18B20_READ_STRATCHPAD_CMD);

	for (i = 0; i != DS18B20_STRATCHPAD_SIZE; i++)
		buf[i] = dev->interface->read();

	ret = dallas_crc(buf, DS18B20_STRATCHPAD_SIZE);
	if (!ret) {
		dev->temp = buf[0] | (buf[1] << 8);
		dev->Th = buf[2];
		dev->Tl = buf[3];
		dev->res = (enum ds18b20_resolution) ((buf[4] << 5) & 0x60);
	}
	return ret;
}

/**
  * @brief  Simply starts the conversion on one sensor connected to bus
  * @param  DS18B20: Sensor to start conversion (single sensor)
  * @retval One_Wire_Success or One wire error
  */
uint8_t ds18b20_start_single(struct ds18b20_s *dev)
{
	uint8_t ret = dev->interface->reset();
	if (ret)
		return ret;
	
	/* Start conversion skipping ROM */
	dev->interface->write(DS18B20_SKIP_ROM);
	dev->interface->write(DS18B20_CONVERT_T_CMD);

	return ret;
}

/**
  * @brief  Simply reads the conversion result from one sensor connected to bus
  * @param  DS18B20: Sensor to read (single sensor)
  * @retval One_Wire_Success or One wire error
  */
uint8_t ds18b20_read_single(struct ds18b20_s *dev)
{
	uint8_t buf[DS18B20_STRATCHPAD_SIZE];
	int16_t rawvalue;

	uint8_t ret = dev->interface->reset();
	if (ret)
		return ret;

	dev->interface->write(DS18B20_SKIP_ROM);
	dev->interface->write(DS18B20_READ_STRATCHPAD_CMD);

	for (ret = 0; ret != DS18B20_STRATCHPAD_SIZE; ret++)
		buf[ret] = dev->interface->read();

	if (dallas_crc(buf, sizeof(buf)))
		return DS18B20_ERROR_CRC_MISSMATCH;

	rawvalue = buf[0] | (buf[1] << 8);
	if (rawvalue == 2495)
		return DS18B20_ERROR_RESULT_NOT_READY;

	dev->temp = (double)(rawvalue) / 16.0;

	return 0;
}

/**
  * @brief  Converts result to float value according choosen resolution
  * @param  DS18B20: Sensor to get data from
  * @retval float temperature value
  */
double ds18b20_convert_temp(struct ds18b20_t *dev)
{
	return (double)dev->temp / (1 << (dev->res + 1));
}

/**
  * @brief  Gets delay value in ms needed to perform conversion
  * @param  DS18B20: Sensor to get resolution value
  * @retval delay in us needed to convert temperature
  */
uint16_t ds18b20_delay_value(struct ds18b20_t *dev)
{
	const uint16_t delays[] = {100, 200, 400, 800};
	return delays[dev->res];
}
