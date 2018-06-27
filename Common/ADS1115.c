#include "ADS1115.h"
#include <stddef.h>

#define CONV_REG 0
#define CONF_REG 1
#define LO_TRES  2
#define HI_TRES  3

ads1115_t * local_driver = NULL;
static void adcdefaultinit (void);
static void adcstart (ads1115_multconf_t channel);
static int16_t adcread (void);
static bool adcbusy (void);

ads1115_t * ads1115_init (ads1115_t * driver)
{  
  if (driver) //assign driver if specified
    local_driver = driver;
  else //just return assigned before driver
    return local_driver;
  
  if (local_driver->initfinished) //used for fast reassign if many drivers used
    return local_driver;
  
  local_driver->initfinished = true;
  
  //assign functions
  driver->read = adcread;
  driver->busy = adcbusy;
  driver->start = adcstart;
  
  if (driver->usedefaultconfig) //init with default settings
    adcdefaultinit();
  
  driver->config_value = driver->latching_comparator ? (1 << 2) : 0;
  driver->config_value = driver->comparator_queue;
  driver->config_value |= driver->comp_polarity << 3;
  driver->config_value |= driver->comparator << 4;
  driver->config_value |= driver->sample_rate << 5;
  driver->config_value |= driver->mode << 8;
  driver->config_value |= driver->gain << 9;
  driver->config_value |= driver->analog_multiplexer_conf << 12;
  driver->interface_error = driver->write_func(CONF_REG, (uint8_t*)&driver->config_value, 2);
  
  return local_driver;
}

static void adcdefaultinit (void)
{
  if (local_driver == NULL) return;
  
  local_driver->analog_multiplexer_conf = AINP_AIN0andAINN_GND;
  local_driver->gain = FSR_1_024V;
  local_driver->mode = Single_shot_mode;
  local_driver->sample_rate = Rate_8_SPS;
  local_driver->comparator = traditional_comparator;
  local_driver->comp_polarity = active_low;
  local_driver->comparator_queue = Assert_after_one_conversion;
  local_driver->latching_comparator = false;
  local_driver->initfinished = false;
}

static void adcstart (ads1115_multconf_t channel)
{
  if (local_driver->channel != channel)
  {
    local_driver->channel = channel;
    local_driver->initfinished = false;
    local_driver->usedefaultconfig = false;
    ads1115_init(local_driver);
  }
  
  local_driver->config_value |= 0x8000;
  local_driver->interface_error = local_driver->write_func(CONF_REG, (uint8_t*)&local_driver->config_value, 2);
  local_driver->config_value &= ~0x8000;
}

static bool adcbusy (void)
{
  uint16_t res;
  
  local_driver->interface_error = local_driver->read_func(CONF_REG, (uint8_t*)&res, sizeof(res));
  
  return res & 0x8000 ? false : true;
}

static int16_t adcread (void)
{
  int16_t res;
  
  local_driver->interface_error = local_driver->read_func(CONV_REG, (uint8_t*)&res, sizeof(res));
  
  return res;
}
