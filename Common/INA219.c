#include "INA219.h"

#define CONFIG_REG      0
#define SHUNT_REG       1
#define BUS_REG         2
#define POWER_REG       3
#define CURRENT_REG     4
#define CAL_REG         5

ina219_t * driver;

ina219_t * ina219_init (ina219_t * this)
{
  uint16_t config = 0x8000;
  if (this) driver = this;
  
  driver->wr(CONFIG_REG, (void*)&config, sizeof(config)); //reset
  if (driver->config)
  {
    config = (uint16_t)driver->config->bus_voltage_range | (uint16_t)driver->config->shunt_voltage_range | 
                   (uint16_t)driver->config->adc_config | (uint16_t)driver->config->operating_mode |                     
                     ((driver->config->adc_number_of_samples_for_bus_voltage & 0x0F) << 7);
    driver->wr(CONFIG_REG, (void*)&config, sizeof(config));
  }
  
  return driver;
}