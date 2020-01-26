#include "LM75.h"

#define LM75_TEMP_REG		0x00
#define LM75_CONFIG_REG		0x01
#define LM75_THYST_REG		0x02
#define LM75_TOS_REG		0x03

uint8_t lm75_init(struct lm75_t *dev, bool enable)
{
	return dev->wr(dev->i2c_addr, LM75_CONFIG_REG, enable ? 0 : 1);
}

uint8_t lm75_get_result(struct lm75_t *dev)
{
	uint8_t result;
	int8_t tmp[2];

	result = dev->rd(dev->i2c_addr, LM75_TEMP_REG,
			(uint8_t*)tmp, sizeof(tmp));

	dev->temp = (float)(tmp[0]);
	if (tmp[1] & 0x80)
		dev->temp += 0.5;

	return result;
}
