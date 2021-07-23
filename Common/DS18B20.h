#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>

#define DS18B20_ERROR_RESULT_NOT_READY			0xE0
#define DS18B20_ERROR_CRC_MISSMATCH			0xB0

enum ds18b20_resolution {
	Res_9bit,
	Res_10bit,
	Res_11bit,
	Res_12bit,
};

struct one_wire_interface {
	uint8_t (*reset)(void);
	uint8_t (*write)(uint8_t value);
	uint8_t (*read)(void);
};

struct ds18b20_t {
	struct one_wire_interface *interface;
	uint8_t SN[8];
	int16_t temp;
	int8_t Th;
	int8_t Tl;
	enum ds18b20_resolution res;
};

struct ds18b20_s {
	struct one_wire_interface *interface;
	double temp;
};

uint8_t ds18b20_init(struct ds18b20_t *dev);
uint8_t ds18b20_start(struct ds18b20_t *dev);
uint8_t ds18b20_get_result(struct ds18b20_t *dev);
uint8_t ds18b20_start_single(struct ds18b20_s *dev);
uint8_t ds18b20_read_single(struct ds18b20_s *dev);
double ds18b20_convert_temp(struct ds18b20_t *dev);
uint16_t ds18b20_delay_value(struct ds18b20_t *dev);

#endif
