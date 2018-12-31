#ifndef MAX44009_H
#define MAX44009_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	uint8_t(*rd)(uint8_t address, uint8_t reg);
	void(*wr)(uint8_t address, uint8_t reg, uint8_t value);

	uint8_t address;

}max44009_t;

max44009_t * MAX44009_Init(max44009_t * new_driver, bool continuous_mode);
double MAX44009_Read(max44009_t * device);

#endif // MAX44009_H
