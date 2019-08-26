#ifndef SPI_DEV_H
#define SPI_DEV_H

#include <stdint.h>
#include <stdbool.h>

struct spi_dev {
	const char *name;
	bool is_allocated;
	int fd;
	int (*dbg) (const char *format, ...);
	uint8_t mode;
	uint8_t bits_per_word;
	uint32_t freq;

	/* Public functions */
	int (*send) (uint8_t *data, uint16_t size);
	int (*recv) (uint8_t *data, uint16_t size);
	int (*send_reg) (uint8_t reg, uint8_t *data, uint16_t size);
	int (*recv_reg) (uint8_t reg, uint8_t *data, uint16_t size);
};

struct spi_dev *spi_init (struct spi_dev *driver, const char *name);
void spi_deinit(void);

#endif // !SPI_DEV_H
