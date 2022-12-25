#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include "driver/spi_master.h"

enum spi_freq {
	SPI_CLK_8MHZ	= SPI_MASTER_FREQ_8M,
	SPI_CLK_9MHZ	= SPI_MASTER_FREQ_9M,
	SPI_CLK_10MHZ	= SPI_MASTER_FREQ_10M,
	SPI_CLK_11MHZ	= SPI_MASTER_FREQ_11M,
	SPI_CLK_13MHZ	= SPI_MASTER_FREQ_13M,
	SPI_CLK_16MHZ	= SPI_MASTER_FREQ_16M,
	SPI_CLK_20MHZ	= SPI_MASTER_FREQ_20M,
	SPI_CLK_26MHZ	= SPI_MASTER_FREQ_26M,
	SPI_CLK_40MHZ	= SPI_MASTER_FREQ_40M,
	SPI_CLK_80MHZ	= SPI_MASTER_FREQ_80M,
};

enum spi_clk_mode {
	SPI_IDLE_CLOCK_LOW	= 0,
	SPI_IDLE_CLOCK_HIGH	= 3,
};

spi_device_handle_t *spi_dev(
		int cs_pin,
		enum spi_freq freq,
		enum spi_clk_mode clk_mode);

void spi_send_receive(spi_device_handle_t *handle,
	uint8_t *tx, uint8_t *rx, uint32_t size);

#endif /* __SPI_H__ */
