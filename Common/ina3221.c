#include "ina3221.h"

#define be16toword(a) ((((a) >> 8) & 0xff) | (((a) << 8) & 0xff00))

// local driver pointer
static int3221_t * ina3221 = 0; 
static float convert_from_raw_data (int16_t raw_value);
static int16_t convert_from_float (float value);

/**
 * Install driver and initialize chip
 * If driver already installed, NULL can be used
 */
int3221_t * INA3221_Init (int3221_t * driver)
{
	if (driver) ina3221 = driver;

#ifdef ORDER_BIG_ENDIAN
	uint8_t * config_value = (uint8_t*)&ina3221->config;
#else	
	uint8_t config_value[2] = { ((uint8_t*)&ina3221->config)[1], ((uint8_t*)&ina3221->config)[0] };
#endif		
	
	ina3221->wr(ina3221->i2c_address, INA3221_CONFIG_REG, (void*)&config_value, sizeof(uint16_t));
	
	return ina3221;
}

/**
 * Search chip over I2C bus with 127 possible addresses
 * If driver already installed, NULL can be used
 */
uint8_t INA3221_SearchOnBus (int3221_t * driver)
{
	if (driver) ina3221 = driver;

	uint16_t manufacture_id;
	
	for (uint8_t i = 2; i < 0xFE; i += 2)
		if (ina3221->rd(i, INA3221_MANUFACTURE_ID, (void*)&manufacture_id, sizeof(uint16_t)) == 0) //found
			if (manufacture_id == INA3221_MANUFACTURE_ID_VALUE) return (ina3221->i2c_address = i);
		
	return 0;
}

/**
 * Initialize structure with default (reset) values
 * If driver already installed, NULL can be used
 */
int3221_t * INA3221_StructInit (int3221_t * driver, float shunt_resistance)
{
	if (driver) ina3221 = driver;
	
	ina3221->i2c_address = 0x80;
	
	driver->config.INA3221_Reset = false;
	driver->config.INA3221_CH1_EN = true;
	driver->config.INA3221_CH2_EN = true;
	driver->config.INA3221_CH3_EN = true;
	driver->config.INA3221_Averaging = 0;
	driver->config.INA3221_BusVoltageConvTime = 4;
	driver->config.INA3221_ShuntVoltageConvTime = 4;
	driver->config.INA3221_Mode = INA3221_SHUNT_AND_BUS_VOLTAGE_CONT_DEFAULT;
	
	driver->shunt[0] = shunt_resistance;
	driver->shunt[1] = shunt_resistance;
	driver->shunt[2] = shunt_resistance;
	
	return ina3221;
}

/**
 * Reading bus voltage [V]
 * Channel (0, 1 or 2) == INA3221_CHx
 */
float INA3221_ReadBusVoltage (uint8_t channel)
{
	const uint8_t channels[] = { INA3221_CH1_BUS_V, INA3221_CH2_BUS_V, INA3221_CH3_BUS_V };
	
	int16_t raw_value;
	
	ina3221->rd(ina3221->i2c_address, channels[channel > 2 ? 0 : channel], (void*)&raw_value, sizeof(uint16_t));

	return convert_from_raw_data(raw_value) * 8 / 1000;
}

/**
 * Reading load current [A]
 * Channel (0, 1 or 2) == INA3221_CHx
 */
float INA3221_ReadShuntCurrent (uint8_t channel)
{
	const uint8_t channels[] = { INA3221_CH1_SHUNT_I, INA3221_CH2_SHUNT_I, INA3221_CH3_SHUNT_I };

	int16_t raw_value;
	
	ina3221->rd(ina3221->i2c_address, channels[channel > 2 ? 0 : channel], (void*)&raw_value, sizeof(uint16_t));
	
	return convert_from_raw_data(raw_value) * 40 / 1000000 / ina3221->shunt[channel];
}

/**
 * Set critial-alert limit [A]
 * Channel (0, 1 or 2) == INA3221_CHx
 */
void INA3221_SetCriticalAlertCurrent (float current, uint8_t channel)
{
	const uint8_t channels[] = { INA3221_CH1_CRIT_I, INA3221_CH2_CRIT_I, INA3221_CH3_CRIT_I };
	
	int16_t raw_value = convert_from_float(current / 40 * 1000000 * ina3221->shunt[channel]);
	
	ina3221->wr(ina3221->i2c_address, channels[channel > 2 ? 0 : channel], (void*)&raw_value, sizeof(uint16_t));
}

/**
 * Set warning-alert limit [A]
 * Channel (0, 1 or 2) == INA3221_CHx
 */
void INA3221_SetWarningAlertCurrent (float current, uint8_t channel)
{
	const uint8_t channels[] = { INA3221_CH1_WARN_I, INA3221_CH2_WARN_I, INA3221_CH3_WARN_I };
	
	int16_t raw_value = convert_from_float(current / 40 * 1000000 * ina3221->shunt[channel]);
	
	ina3221->wr(ina3221->i2c_address, channels[channel > 2 ? 0 : channel], (void*)&raw_value, sizeof(uint16_t));
}

/**
 * Set power valid limits [V]
 */
void INA3221_SetPowerValidLimits (float hi_lim, float lo_lim)
{
	int16_t raw_value = convert_from_float(hi_lim / 8 * 1000);
	
	ina3221->wr(ina3221->i2c_address, INA3221_POWER_UPPER_LIM, (void*)&raw_value, sizeof(uint16_t));
	
	raw_value = convert_from_float(lo_lim / 8 * 1000);
	
	ina3221->wr(ina3221->i2c_address, INA3221_POWER_LOWER_LIM, (void*)&raw_value, sizeof(uint16_t));
}

static float convert_from_raw_data (int16_t raw_value)
{
#ifdef ORDER_BIG_ENDIAN
	return (float)(raw_value / 8);
#else
	return (float)(be16toword(raw_value) / 8);
#endif
}

static int16_t convert_from_float (float value)
{
#ifdef ORDER_BIG_ENDIAN
	return (int16_t)value * 8;
#else
	return be16toword((int16_t)value * 8);
#endif	
}
