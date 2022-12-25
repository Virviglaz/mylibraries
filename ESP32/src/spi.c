#include "spi.h"
#include "driver/gpio.h"

#define ESP_SPI_DEV		2
#define MOSI_PIN		27
#define MISO_PIN		26
#define MSCK_PIN		14

static void init(void)
{
	static bool init_done = false;
	if (init_done)
		return;

	spi_bus_config_t pincfg = {
		.mosi_io_num = MOSI_PIN,
		.miso_io_num = MISO_PIN,
		.sclk_io_num = MSCK_PIN,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,
		.flags = SPICOMMON_BUSFLAG_MASTER,
		.intr_flags = 0,
	};

	gpio_reset_pin((gpio_num_t)MOSI_PIN);
	gpio_reset_pin((gpio_num_t)MISO_PIN);
	gpio_reset_pin((gpio_num_t)MSCK_PIN);

	ESP_ERROR_CHECK(spi_bus_initialize(ESP_SPI_DEV,
		&pincfg, SPI_DMA_DISABLED));

	gpio_set_pull_mode(MISO_PIN, GPIO_PULLUP_ONLY);

	init_done = true;
}

spi_device_handle_t *spi_dev(
		int cs_pin,
		enum spi_freq freq,
		enum spi_clk_mode clk_mode)
{
	init();

	spi_device_handle_t *handle = malloc(sizeof(spi_device_handle_t));

	spi_device_interface_config_t dev_cfg = {
		.clock_speed_hz = freq,
		.mode = clk_mode,
		.spics_io_num = cs_pin,
		.input_delay_ns = 0,
		.queue_size = 1,
		.pre_cb = NULL,
		.post_cb = NULL,
		.flags = 0,
		.address_bits = 0,
		.command_bits = 0,
		.dummy_bits = 0,
		.duty_cycle_pos = 0,
		.cs_ena_pretrans = 16,
		.cs_ena_posttrans = 16,
	};

	gpio_reset_pin((gpio_num_t)cs_pin);

	ESP_ERROR_CHECK(spi_bus_add_device(ESP_SPI_DEV, &dev_cfg, handle));

	return handle;
}

void spi_send_receive(spi_device_handle_t *handle,
	uint8_t *tx, uint8_t *rx, uint32_t size)
{
	spi_transaction_t msg = {
		.length = size * 8,
		.tx_buffer = tx,
		.rx_buffer = rx,
		.rxlength = 0,
		.addr = 0,
		.flags = 0,
	};

	ESP_ERROR_CHECK(spi_device_polling_transmit(*handle, &msg));
}
