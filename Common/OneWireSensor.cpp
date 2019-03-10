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
#define WRITE_STRATCHPAD_CMD			0x4E
#define READ_STRATCHPAD_CMD				0xBE
#define COPY_STRATCHPAD_CMD				0x48
#define RECALL_E_CMD							0xB8
#define READ_POWER_SUPPLY_CMD			0xB4

#define DS18B20_STRATCHPAD_SIZE		0x09

static void find_sensor(OneWire *interface, uint8_t *sn);
static float convert_t(uint8_t *buf, Resolution resolution);
static Resolution convert_resolution(uint8_t value);

/**
  * @brief  Initialize with thresholds and resolution
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO
  */
OneWireError OneWireSensor::init()
{
	/* Send RESET pulse */
	OneWireError res = interface->reset();
	
	if (res)
		return res;
	
	/* Find sensor */
	find_sensor(interface, sn);
	
	/* Set resolutuin */
	interface->write_byte(WRITE_STRATCHPAD_CMD);
	interface->write_byte(Th);
	interface->write_byte(Tl);
	interface->write_byte((resolution << 5) | 0x1F);

	is_initialized = true;
	is_conv_started = false;
	
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

	/* Send READ command */
	interface->write_byte(READ_STRATCHPAD_CMD);

	/* Read sensor's buffer */
	uint8_t buf[DS18B20_STRATCHPAD_SIZE];
	for (uint8_t cnt = 0; cnt != DS18B20_STRATCHPAD_SIZE; cnt++)
		buf[cnt] = interface->read_byte();

	/* Check CRC if needed */
	if (crc)
		if (!crc(buf, DS18B20_STRATCHPAD_SIZE))
			res = ERR_CRC;

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
	interface->write_byte(READ_STRATCHPAD_CMD);
	
	/* Read sensor's buffer */
	uint8_t buf[DS18B20_STRATCHPAD_SIZE];
	for (uint8_t cnt = 0; cnt != DS18B20_STRATCHPAD_SIZE; cnt++)
		buf[cnt] = interface->read_byte();
	
	/* Update resolution */
	resolution = convert_resolution(buf[4]);
	
	if (resolution > RESOLUTION_12_BIT)
		return ERR_BAD_DATA;
	
	/* Check CRC if needed */
	if (crc)
		if (crc(buf, DS18B20_STRATCHPAD_SIZE))
			res = ERR_CRC;
	
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
  * @retval value in ms
  */
uint16_t OneWireSensor::get_conv_time_ms()
{
	if (resolution > 3) 
		resolution = RESOLUTION_12_BIT;
	
	const uint16_t conv_time_ms[] = { 100, 200, 400, 800 };
	return conv_time_ms[resolution];
}

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

static void find_sensor(OneWire *interface, uint8_t *sn)
{
	interface->write_byte(MATCH_ROM);

	for (uint8_t cnt = 0; cnt != 8; cnt++) 
		interface->write_byte(sn[cnt]);
}

static float convert_t(uint8_t *buf, Resolution resolution)
{
	int16_t raw_value = buf[0] | buf[1] << 8;
	return (float)raw_value / (1 << (resolution + 1));
}

static Resolution convert_resolution(uint8_t value)
{
	return (Resolution)((value & 0x60) >> 5);
}
