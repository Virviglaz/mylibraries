/**
  ******************************************************************************
  * @file    OneWireSensor.cpp
  * @author  Pavel Nadein
  * @version V1.0
  * @date    10-March-2019
  * @brief   This file contains all the functions/macros to use DS1822/DS18B20.
  ******************************************************************************
  */

#include "OneWireSensor.h"

/* DS18B20 Commands list */
#define READ_ROM									0x33
#define SKIP_ROM									0xCC
#define SEARCH_ROM								0xF0
#define MATCH_ROM									0x55

#define CONVERT_T_CMD							0x44
#define WRITE_SCRATCHPAD_CMD			0x4E
#define READ_SCRATCHPAD_CMD				0xBE
#define COPY_SCRATCHPAD_CMD				0x48
#define RECALL_E_CMD							0xB8
#define READ_POWER_SUPPLY_CMD			0xB4

#define DS18B20_SCRATCHPAD_SIZE		0x09

static void find_sensor(OneWire *interface, uint8_t *sn);
static float convert_t(uint8_t *buf, Resolution resolution);
static Resolution convert_resolution(uint8_t value);
static OneWireError read_scratchpad(OneWire *interface,
	uint8_t *buf, uint8_t (*crc)(uint8_t *buf, uint8_t size));

/**
  * @brief  Initialize with thresholds and resolution
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO
  */
OneWireError OneWireSensor::init()
{
	/* Send RESET pulse -> WRITE SCRATCHPAD */
	OneWireError res = interface->reset();
	
	if (res)
		return res;
	
	/* Find sensor */
	find_sensor(interface, sn);
	
	/* Set resolutuin */
	interface->write_byte(WRITE_SCRATCHPAD_CMD);
	interface->write_byte(Th);
	interface->write_byte(Tl);
	interface->write_byte(resolution << 5);

	is_initialized = true;
	is_conv_started = false;

	/* Send RESET pulse -> READ SCRATCHPAD */
	res = interface->reset();
	
	if (res)
		return res;
	
	/* Find sensor */
	find_sensor(interface, sn);
	
	uint8_t buf[DS18B20_SCRATCHPAD_SIZE];
	res = read_scratchpad(interface, buf, crc);

	/* TODO: verify content */

	/* Send RESET pulse -> COPY SCRATCHPAD TO EEPROM */
	res = interface->reset();
	
	if (res)
		return res;
	
	/* Find sensor */
	find_sensor(interface, sn);

	/* Copy ram to eeprom */
	interface->write_byte(COPY_SCRATCHPAD_CMD);
	
	/* Wait for eeprom update */
	for (uint8_t i = 0; i != 100; i++)
		if (interface->read_bit())
			break;
	
	return res;
}

/**
  * @brief  Starting the conversion using sensor's serial number
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO
  */
OneWireError OneWireSensor::start_conversion()
{
	OneWireError res;
	
	if (!is_initialized)
	{
		res = init();

		if (res)
			return res;
	}

	/* Send RESET pulse */	
	res = interface->reset();

	if (res)
		return res;

	/* Find sensor */
	find_sensor(interface, sn);

	/* Sens start conversion command */
	interface->write_byte(CONVERT_T_CMD);
	
	is_conv_started = true;

	return res;
}

/**
  * @brief  Starting the conversion (only one sensor on bus)
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO
  */
OneWireError OneWireSensor::start_conversion_skip_rom()
{
	/* Send RESET pulse */	
	OneWireError res = interface->reset();
	
	if (res)
		return res;

	/* Start conversion skipping ROM */
	interface->write_byte(SKIP_ROM);
	interface->write_byte(CONVERT_T_CMD);
	
	is_conv_started = true;
	
	return res;
}

/**
  * @brief  Get conversion result using sensor's serial number
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO, ERR_CONV_NOT_STARTED
  */
OneWireError OneWireSensor::get_result()
{
	OneWireError res;
	
	if (!is_initialized)
	{
		res = init();

		if (res)
			return res;
	}
	
	if (!is_conv_started)
		return ERR_CONV_NOT_STARTED;

	/* Send RESET pulse */	
	res = interface->reset();
	
	if (res)
		return res;

	/* Find sensor */
	find_sensor(interface, sn);

	/* Read sensor's buffer */
	uint8_t buf[DS18B20_SCRATCHPAD_SIZE];
	res = read_scratchpad(interface, buf, crc);

	/* Update resolution and thresholds */
	resolution = convert_resolution(buf[4]);
	Th = buf[2];
	Tl = buf[3];
	
	temperature = convert_t(buf, resolution);

	/* Save last valid result */
	if (res == SUCCESS)
	{
		last_valid.is_valid = true;
		last_valid.temperature = temperature;
	}
	else
		if (last_valid.is_valid == true)
			temperature = last_valid.temperature;

	return res;
}

/**
  * @brief  Get conversion result (only one sensor on bus)
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO, ERR_CONV_NOT_STARTED
  */
OneWireError OneWireSensor::get_result_skip_rom()
{
	OneWireError res;
		
	if (!is_conv_started)
		return ERR_CONV_NOT_STARTED;

	/* Send RESET pulse */	
	res = interface->reset();
	
	if (res)
		return res;

	interface->write_byte(SKIP_ROM);
	
	uint8_t buf[DS18B20_SCRATCHPAD_SIZE];
	res = read_scratchpad(interface, buf, crc);
	
	/* Update resolution */
	resolution = convert_resolution(buf[4]);
	
	if (resolution > RESOLUTION_12_BIT)
		return ERR_BAD_DATA;
	
	temperature = convert_t(buf, resolution);
	
	/* Save last valid result */
	if (res == SUCCESS)
	{
		last_valid.is_valid = true;
		last_valid.temperature = temperature;
	}
	else
		if (last_valid.is_valid == true)
			temperature = last_valid.temperature;
		
	return res;
}

/**
  * @brief  Get time, needed to perform conversion
  * @retval Value in ms
  */
uint16_t OneWireSensor::get_conv_time_ms()
{
	if (resolution > 3) 
		resolution = RESOLUTION_12_BIT;
	
	const uint16_t conv_time_ms[] = { 100, 200, 400, 800 };
	return conv_time_ms[resolution];
}

/**
  * @brief  Get text error description
  * @retval Pointer to text
  */
const char *OneWireSensor::error_desc(OneWireError err)
{
	if (err > ERR_BAD_DATA)
		return "Value out of range";
	
	const char * const err_list[] = {
		"Success",
		"No echo from device",
		"Bus in low state",
		"Bus busy",
		"CRC mismatch",
		"Data not ready",
		"Bad data received",
	};
	
	return err_list[err];
}

/**
  * @brief  Search devices over one reference interface
  * @retval Error code
  */
OneWireError OneWireSensors::search_sensors()
{
	uint32_t path = 0, next, pos;                     /* decision markers */                              
 	uint8_t bit, chk, cnt_num = 0;                    /* bit values */
	OneWireError res;
	
	do
	{
		/* Send RESET pulse */	
		res = interface->reset();
		
		if (res)
			return res;

		/* issue the 'ROM search' command */
		interface->write_byte(SEARCH_ROM);
		
		next = 0;	/* next path to follow */
		pos = 1;	/* path bit pointer */ 
		
		for (uint8_t cnt_byte = 0; cnt_byte != 8; cnt_byte++)
		{
			sensors_list[cnt_num].sn[cnt_byte] = 0;
			for (uint8_t cnt_bit = 0; cnt_bit != 8; cnt_bit++)
			{
				/* each bit of the ROM value */
 				/* read two bits, 'bit' and 'chk', from the 1-wire bus */
				bit = interface->read_bit();
				chk = interface->read_bit();
				
				if(!bit && !chk)
				{
						/* collision, both are zero */
						if (pos & path) 
							/* if we've been here before */
							bit = 1;
						else
							/* else, new branch for next */
							next = (path & (pos - 1)) | pos;
						pos <<= 1;
				}
				
				/* write 'bit' to the 1-wire bus */
				interface->write_bit(bit);

				/* save this bit as part of the current ROM value */
				if (bit) sensors_list[cnt_num].sn[cnt_byte] |= (1 << cnt_bit);
			}
		}

		/* output the just-completed ROM value */
		path = next;
		cnt_num++;
	}while(path && cnt_num < max_devices);
	
	devices_found = cnt_num;
	
	return res;
}

/**
  * @brief  Get devices amount
  * @retval Amount of devices on reference interface
  */
uint8_t OneWireSensors::get_devices_found()
{
	return devices_found;
}

/**
  * @brief	Get a pointer to devices found
	* @param	Number of object
  * @retval Pointer to device object
  */
OneWireSensor *OneWireSensors::get_sensor(uint8_t num)
{
	return num > devices_found ? NULL : &sensors_list[num];
}

static void find_sensor(OneWire *interface, uint8_t *sn)
{
	interface->write_byte(MATCH_ROM);

	for (uint8_t cnt = 0; cnt != 8; cnt++) 
		interface->write_byte(sn[cnt]);
}

static float convert_t(uint8_t *buf, Resolution resolution)
{
	return (float)(buf[0] | buf[1] << 8) / (1 << (1 + resolution));
}

static Resolution convert_resolution(uint8_t value)
{
	return (Resolution)((value & 0x60) >> 5);
}

static OneWireError read_scratchpad(OneWire *interface,
	uint8_t *buf, uint8_t (*crc)(uint8_t *buf, uint8_t size))
{
	/* Send READ command */
	interface->write_byte(READ_SCRATCHPAD_CMD);

	/* Read sensor's buffer */
	for (uint8_t cnt = 0; cnt != DS18B20_SCRATCHPAD_SIZE; cnt++)
		buf[cnt] = interface->read_byte();

	/* Check CRC if needed */
	if (crc)
		if (crc(buf, DS18B20_SCRATCHPAD_SIZE))
			return ERR_CRC;
	
	return SUCCESS;
}
