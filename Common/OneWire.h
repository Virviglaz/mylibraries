/**
  ******************************************************************************
  * @file    OneWire.h
  * @author  Pavel Nadein
  * @version V1.0
  * @date    10-March-2019
  * @brief   This file contains all the functions/macros to use one-wire (c) bus.
  ******************************************************************************
  */

#ifndef ONEWIRE_H
#define ONEWIRE_H

#define HIGH_PULSE_WAIT_US					40
#define LOW_PULSE_WAIT_US						10

#include <stdint.h>

typedef enum
{
	SUCCESS,
	ERR_NOECHO,
	ERR_BUSLOW,
	ERR_BUSBUSY,
	ERR_CRC,
	ERR_CONV_NOT_STARTED,
	ERR_BAD_DATA,
}OneWireError;

class OneWire
{
	public:
		/* Constructor */
		OneWire(void (*_delay_func)(uint16_t us),
			void (*_set_pin)(uint16_t state),
				uint16_t (*_get_pin)(void))
				{
					delay_func = _delay_func;
					set_pin = _set_pin;
					get_pin = _get_pin;
					
					/* Default values, can be changed by app */
					high_pulse_wait_us = HIGH_PULSE_WAIT_US;
					low_pulse_wait_us = LOW_PULSE_WAIT_US;
				};

		/* Public functions */
		OneWireError reset();
		void write_bit(uint8_t);
		uint8_t read_bit();				
		void write_byte(uint8_t);
		uint8_t read_byte();

		/* Public properties */
		uint8_t high_pulse_wait_us;
		uint8_t low_pulse_wait_us;
				
	private:
		/* Private properties */
		void (*delay_func)(uint16_t);
		void (*set_pin)(uint16_t);
		uint16_t (*get_pin)(void);
};

#endif // ONEWIRE_H
