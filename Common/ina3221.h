#ifndef INA3221_H
#define INA3221_H

#include <stdint.h>
#include <stdbool.h>

#define INA3221_CONFIG_REG			0x00
#define INA3221_CH1_SHUNT_V			0x01
#define INA3221_CH1_BUS_V				0x02
#define INA3221_CH2_SHUNT_V			0x03
#define INA3221_CH2_BUS_V				0x04
#define INA3221_CH3_SHUNT_V			0x05
#define INA3221_CH3_BUS_V				0x06
#define INA3221_CH1_CRIT_V			0x07
#define INA3221_CH1_WARN_V			0x08
#define INA3221_CH2_CRIT_V			0x09
#define INA3221_CH2_WARN_V			0x0A
#define INA3221_CH3_CRIT_V			0x0B
#define INA3221_CH3_WARN_V			0x0C
#define INA3221_SHUNT_V_SUM			0x0D
#define INA3221_SHUNT_V_SUM_LIM	0x0E
#define INA3221_MASK_ENABLE			0x0F
#define INA3221_POWER_UPPER_LIM 0x10
#define INA3221_POWER_LOWER_LIM	0x11
#define INA3221_MANUFACTURE_ID	0xFE
#define INA3221_DIE_ID					0xFF

#define INA3221_MANUFACTURE_ID_VALUE	0x4954

#define INA3221_POWER_DOWN 														0
#define INA3221_SHUNT_VOLTAGE_SINGLE_SHOT							1
#define INA3221_BUS_VOLTAGE_SINGLE_SHOT								2
#define INA3221_SHUNT_AND_BUS_VOLTAGE_SINGLE_SHOT			3
#define INA3221_SHUNT_VOLTAGE_CONT										5
#define INA3221_BUS_VOLTAGE_CONT											6
#define INA3221_SHUNT_AND_BUS_VOLTAGE_CONT_DEFAULT		7

typedef struct
{
	uint16_t INA3221_Mode: 3;
	uint16_t INA3221_ShuntVoltageConvTime : 3;
	uint16_t INA3221_BusVoltageConvTime : 3;
	uint16_t INA3221_Averaging : 3;
	bool INA3221_CH3_EN : 1;
	bool INA3221_CH2_EN : 1;
	bool INA3221_CH1_EN : 1;	
	bool INA3221_Reset : 1;
}int3221_config_t;

typedef struct
{
	/* Interface */
	uint8_t (*wr) (uint8_t i2c_adrs, uint8_t reg, uint8_t * buf, uint8_t len);
	uint8_t (*rd) (uint8_t i2c_adrs, uint8_t reg, uint8_t * buf, uint8_t len);
	
	/* Settings */
	int3221_config_t config;
	
	/* I2c address */
	uint8_t i2c_address;
	
	/* Shunt resistors */
	float shunt[3];
}int3221_t;

int3221_t * INA3221_Init (int3221_t * driver);
uint8_t INA3221_SearchOnBus (int3221_t * driver);
int3221_t * INA3221_StructInit (int3221_t * driver, float shunt_resistance);
float INA3221_ReadBusVoltage (uint8_t channel);
float INA3221_ReadShuntCurrent (uint8_t channel);
#endif //INA3221_H
