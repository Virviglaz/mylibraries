/**
  ******************************************************************************
  * @file    OneWire.cpp
  * @author  Pavel Nadein
  * @version V1.0
  * @date    10-March-2019
  * @brief   This file contains all the functions/macros to use one-wire (c) bus.
  ******************************************************************************
  */

#include "OneWire.h"

/* This settings defines the timings */
#define TIME_BEFORE_RESET_US				480
#define TIME_AFTER_RESET_US					350
#define RESET_LOW_TIME_US						480
#define READ_PIN_WAIT_US						10

#define LOW													0
#define HIGH												1

/**
  * @brief  Send RESET pulse to initiate transfer
  * @retval SUCCESS, ERR_BUSLOW, ERR_NOECHO
  */
OneWireError OneWire::reset()
{
	/* Wait some time before send RESET pulse */
	delay_func(TIME_BEFORE_RESET_US);

	/* Check that bus is in high state before RESET */
	if (!get_pin())
		return ERR_BUSLOW;
	
	/* Force TX pin low - begin of RESET sequencs */
	set_pin(LOW);
		
	/* Keep TX pin low for ~480us (typical) */
	delay_func(RESET_LOW_TIME_US);

	/* Release TX pin */
	set_pin(HIGH);
		
	/* Wait some time before read RESPONCE status */
	delay_func(high_pulse_wait_us);
	
	/* If bus is high that means that no RESPONCE obtained */
	if (get_pin())
		return ERR_NOECHO;
	
	/* Wait some time before transmisson started */
	delay_func(TIME_AFTER_RESET_US);

	return SUCCESS;
}

/**
  * @brief  Send one bit of data to bus
  * @retval none
  */
void OneWire::write_bit(uint8_t value)
{
	set_pin(LOW);
	
	/* Write 1 to 1-wire device, 0/1 depends of pulse width */
	delay_func(value ? low_pulse_wait_us : high_pulse_wait_us);
	set_pin(HIGH);
	delay_func(value ? high_pulse_wait_us : low_pulse_wait_us);
}

/**
  * @brief  Read one bit of data from bus
  * @retval 0 or !0
  */
uint8_t OneWire::read_bit()
{
		uint8_t res;
	
		/* Initiate the trasfer */
		set_pin(LOW);
		delay_func(READ_PIN_WAIT_US);
	
		set_pin(HIGH);
		delay_func(low_pulse_wait_us);

		res = get_pin();
			
		delay_func(high_pulse_wait_us);
	
		return res;
}

/**
  * @brief  Send one byte of data to bus
  * @retval none
  */
void OneWire::write_byte(uint8_t value)
{
	for (uint8_t cnt = 0; cnt != 8; cnt++) 
		OneWire::write_bit(value & (1 << cnt));
}

/**
  * @brief  Read one byte of data from bus
  * @retval byte value
  */
uint8_t OneWire::read_byte()
{
	uint8_t res = 0;
	
	for (uint8_t cnt = 0; cnt != 8; cnt++)
		if (OneWire::read_bit())	res |= (1 << cnt);
	
	delay_func(high_pulse_wait_us);
	
	return res;
}
