#include "ADS1115.h"
#include <stddef.h>

#define CONV_REG 0
#define CONF_REG 1
#define LO_TRES  2
#define HI_TRES  3

ads1115_t * local_driver = NULL;
static void start (ads1115_multconf_t channel);
static int16_t read (void);
static bool busy (void);

ads1115_t * ads1115_init (ads1115_t * driver)
{  
  if (driver) //assign driver if specified
    local_driver = driver;
  else //just return assigned before driver
    return local_driver;
 
  //assign functions
  driver->read = read;
  driver->busy = busy;
  driver->start = start;
  
  //default configuration
  driver->config_value = 0x8583; 

  return local_driver;
}

uint16_t adc1115_config (adc1115_conf_t * config)
{
  uint16_t res = config->latching_comparator ? (1 << 2) : 0;
  res |= config->comparator_queue;
  res |= config->comp_polarity << 3;
  res |= config->comparator << 4;
  res |= config->sample_rate << 5;
  res |= config->mode << 8;
  res |= config->gain << 9;
  res |= config->analog_multiplexer_conf << 12;

  local_driver->interface_error = local_driver->write_func(CONF_REG, (uint8_t*)&res, 2);
  return res;
}

static void start (ads1115_multconf_t channel)
{
  local_driver->config_value &= ~0x7000; //analog multiplexer default value
  
  //start conversion bit 15 and analog multiplexer switch
  local_driver->config_value |= 0x8000 | (channel << 12);
  
  //configure adc using interface function
  local_driver->interface_error = local_driver->write_func(CONF_REG, (uint8_t*)&local_driver->config_value, 2);
}

static bool busy (void)
{
  uint16_t res;
  
  local_driver->interface_error = local_driver->read_func(CONF_REG, (uint8_t*)&res, sizeof(res));
  
  return res & 0x8000 ? false : true;
}

static int16_t read (void)
{
  int16_t res;
  
  local_driver->interface_error = local_driver->read_func(CONV_REG, (uint8_t*)&res, sizeof(res));
  
  return res;
}
