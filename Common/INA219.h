#ifndef INA219_H
#define INA219_H

#include <stdint.h>

typedef enum
{
  FSR_16V = 0,
  FSR_32V = 1 << 13,
}bus_voltage_range_t;

typedef enum
{
  SVR_40mV = 0,
  SVR_80mV = 1 << 11,
  SVR_160mV = 2 < 11,
  SVR_320mV = 3 < 11,
}shunt_voltage_range_t;

typedef enum
{
  adc_9bit_84uS = 0,
  adc_10bit_148uS = 1 << 3,
  adc_11bit_276uS = 2 << 3,
  adc_12bit_532uS = 3 << 3,
  adc_12bit_1ms = 9 << 3,
  adc_12bit_2ms = 10 << 3,
  adc_12bit_4ms = 11 << 3,
  adc_12bit_8ms = 12 << 3,
  adc_12bit_17ms = 13 << 3,
  adc_12bit_34ms = 14 << 3,
  adc_12bit_68ms = 15 << 3,
}adc_config_t;

typedef enum
{
  power_down,
  shunt_voltage_triggered,
  bus_voltage_triggered,
  shunt_and_bus_triggered,
  adc_off,
  shunt_voltage_continuous,
  bus_voltage_continuous,
  shunt_and_bus_continuous,
}operating_mode_t;

typedef struct
{
  bus_voltage_range_t bus_voltage_range;
  shunt_voltage_range_t shunt_voltage_range;
  uint8_t adc_number_of_samples_for_bus_voltage;
  adc_config_t adc_config;
  operating_mode_t operating_mode;
}ina219_config_t;

typedef struct
{
  void (*rd) (uint8_t reg, uint8_t * buf, uint16_t len);
  void (*wr) (uint8_t reg, uint8_t * buf, uint16_t len);
  ina219_config_t * config;
}ina219_t;

ina219_t * ina219_init (ina219_t * this);

#endif
