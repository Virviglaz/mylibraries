#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  AINP_AIN0andAINN_AIN1,
  AINP_AIN0andAINN_AIN3,
  AINP_AIN1andAINN_AIN3,
  AINP_AIN2andAINN_AIN3,
  AINP_AIN0andAINN_GND,
  AINP_AIN1andAINN_GND,
  AINP_AIN2andAINN_GND,
  AINP_AIN3andAINN_GND,
}ads1115_multconf_t;

typedef enum
{
  FSR_6_144V,
  FSR_4_096V,
  FSR_2_048V,
  FSR_1_024V,
  FSR_0_512V,
  FSR_0_256V,
}ads1115_gain_t;

typedef enum
{
  Continuous_mode,
  Single_shot_mode,
}ads1115_conv_mode_t;

typedef enum
{
  Rate_8_SPS,
  Rate_16_SPS,
  Rate_32_SPS,
  Rate_64_SPS,
  Rate_128_SPS,
  Rate_250_SPS,
  Rate_475_SPS,
  Rate_860_SPS,
}ads1115_SampleRate_t;

typedef enum
{
  traditional_comparator,
  window_comparator,
}ads1115_comparator_t;

typedef enum
{
  active_low,
  active_high,
}ads1115_comparator_polarity_t;

typedef enum
{
  Assert_after_one_conversion,
  Assert_after_two_conversions,
  Assert_after_four_conversions,
  Disable_comparator,
}ads1115_comparator_queue_t;

typedef struct
{
  uint8_t (* write_func) (uint8_t reg, uint8_t * buf, uint16_t size);
  uint8_t (* read_func) (uint8_t reg, uint8_t * buf, uint16_t size);
  void (* start) (ads1115_multconf_t channel);
  int16_t (* read) (void);
  bool (* busy) (void);
  uint8_t interface_error;
  uint16_t config_value;
}ads1115_t;

typedef struct
{
  ads1115_multconf_t analog_multiplexer_conf;
  ads1115_multconf_t channel;
  ads1115_gain_t gain;
  ads1115_conv_mode_t mode;
  ads1115_SampleRate_t sample_rate;
  ads1115_comparator_t comparator;
  ads1115_comparator_polarity_t comp_polarity;
  bool latching_comparator;
  ads1115_comparator_queue_t comparator_queue;
}adc1115_conf_t;

ads1115_t * ads1115_init (ads1115_t * driver);
uint16_t adc1115_config (adc1115_conf_t * config);

#endif
