/**
  ******************************************************************************
  * @file    OneWireSensor.h
  * @author  Pavel Nadein
  * @version V1.0
  * @date    10-March-2019
  * @brief   This file contains all the functions/macros to use DS1822/DS18B20.
  ******************************************************************************
  */

#ifndef ONEWIRESENSOR_H
#define ONEWIRESENSOR_H

#include <string.h>
#include <stdbool.h>
#include "OneWire.h"

typedef enum
{
	RESOLUTION_9_BIT,
	RESOLUTION_10_BIT,
	RESOLUTION_11_BIT,
	RESOLUTION_12_BIT,
}Resolution;

class OneWireSensor
{
	public:
		/* Constructor */
		OneWireSensor(OneWire *_interface,
			Resolution _resolution,
			uint8_t (*_crc)(uint8_t *buf, uint8_t size))
		{
			interface = _interface;
			resolution = _resolution;
			memset(sn, 0, 8);
			crc = _crc;
			is_initialized = false;
			is_conv_started = false;
			last_valid.is_valid = false;
			last_valid.temperature = -1.0;
		};

	/* Public properties */
	uint8_t (*crc)(uint8_t *buf, uint8_t size);
	OneWire *interface;
	Resolution resolution;
	uint8_t sn[8];
	uint8_t Tl, Th;
	float temperature;
	bool is_initialized, is_conv_started;
	struct {
		bool is_valid;
		float temperature;
	} last_valid;
		
	/* Public functions */
	OneWireError init ();
	OneWireError start_conversion();
	OneWireError start_conversion_skip_rom();
	OneWireError get_result();
	OneWireError get_result_skip_rom();
	uint16_t get_conv_time_ms();
	const char *error_desc(OneWireError err);
};

#endif // ONEWIRESENSOR_H
