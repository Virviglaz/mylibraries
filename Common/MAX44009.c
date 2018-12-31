#include "MAX44009.h"
#include <math.h>

static max44009_t * sensor;

max44009_t * MAX44009_Init(max44009_t * new_driver, bool continuous_mode)
{
	if (new_driver)
		sensor = new_driver;

	sensor->wr(sensor->address, 0x02, continuous_mode ? 0x80 : 0x00);

	return sensor;
}

double MAX44009_Read(max44009_t * device)
{
	uint16_t raw;
	if (device)
		sensor = device;

	raw = sensor->rd(sensor->address, 0x03) << 8;
	raw |= sensor->rd(sensor->address, 0x04);
	
	return pow(2, raw >> 12) * 0.72 * (double)(((0x0F00 & raw) >> 4) | (0x000F & raw));
}
