#ifndef LM75_H
#define LM75_H

#include <stdint.h>
#include <stdbool.h>

struct lm75_t {
	/* Variables */
	double temp;
	uint8_t i2c_addr;
	
	/* Functions */
	uint8_t (*wr)(uint8_t addr, uint8_t reg, uint8_t value);
	uint8_t (*rd)(uint8_t addr, uint8_t reg, uint8_t *buf, uint8_t size);
	
};

#ifdef __cplusplus
 extern "C" {
#endif

uint8_t lm75_init(struct lm75_t *dev, bool enable);
uint8_t lm75_get_result(struct lm75_t *dev);

#ifdef __cplusplus
}
#endif

#endif
