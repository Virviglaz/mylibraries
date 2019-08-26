#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "spi_dev.h"

#define DEFAULT_SPI_DEV			"/dev/spidev0.0"

static struct spi_dev *local_driver;

/* Optional debug output function */
static int debug(const char *format, ...)
{
	if (local_driver->dbg) {
		va_list arg;
		va_start(arg, format);
		local_driver->dbg(format, arg);
		va_end(arg);
	}
	return 0;
}

static int send_receive(uint8_t *data, uint16_t size, bool send)
{
	int res;
	struct spi_ioc_transfer xfer;

	memset(&xfer, 0, sizeof(xfer));

	if (send)
		xfer.tx_buf = (unsigned long)data;
	else
		xfer.rx_buf = (unsigned long)data;

	xfer.len = size;

	res = ioctl(local_driver->fd, SPI_IOC_MESSAGE(1), &xfer);
	if (res)
		debug("%s: Error sending data\n", local_driver->name);

	return res;
}

static int send_receive_reg(uint8_t reg, uint8_t *data, uint16_t size, bool send)
{
	int res;
	struct spi_ioc_transfer xfer[2];

	memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (unsigned long)&reg;
	xfer[0].len = sizeof(reg);

	if (send)
		xfer[1].tx_buf = (unsigned long)data;
	else
		xfer[1].rx_buf = (unsigned long)data;

	res = ioctl(local_driver->fd, SPI_IOC_MESSAGE(2), xfer);
	if (res)
		debug("%s: Error sending data\n", local_driver->name);

	return res;
}

static int send(uint8_t *data, uint16_t size)
{
	return send_receive(data, size, true);
}

static int recv(uint8_t *data, uint16_t size)
{
	return send_receive(data, size, false);
}

static int send_reg(uint8_t reg, uint8_t *data, uint16_t size)
{
	return send_receive_reg(reg, data, size, true);
}

static int recv_reg(uint8_t reg, uint8_t *data, uint16_t size)
{
	return send_receive_reg(reg, data, size, false);
}

struct spi_dev *spi_init(struct spi_dev *driver, const char *name)
{
	/* If NULL we allocate the driver by ourselfs */
	if (!driver) {
		driver = malloc(sizeof(struct spi_dev));
		if (!driver) {
			debug("%s: Error! No memory\n", name);
			return NULL;
		}		
		memset(driver, 0, sizeof(struct spi_dev));
		driver->is_allocated = true;
	} else
		driver->is_allocated = false;

	local_driver = driver;
	local_driver->send = send;
	local_driver->recv = recv;
	local_driver->send_reg = send_reg;
	local_driver->recv_reg = recv_reg;
	local_driver->name = name ? name : DEFAULT_SPI_DEV;

	/* Open the device */
	local_driver->fd = open(local_driver->name, O_RDWR);

	if (local_driver->fd < 0) {
		debug("%s: Fail to open\n", name);
		return NULL;
	}

	if (ioctl(local_driver->fd, SPI_IOC_WR_MODE, &local_driver->mode))
		debug("IOCTL SPI_IOC_WR_MODE failed at line %u\n", __LINE__);

	if (ioctl(local_driver->fd, SPI_IOC_WR_BITS_PER_WORD, 
			&local_driver->bits_per_word))
		debug("IOCTL SPI_IOC_WR_BITS_PER_WORD failed at line %u\n", __LINE__);

	if (ioctl(local_driver->fd, SPI_IOC_WR_MAX_SPEED_HZ,
		&local_driver->freq))
		debug("IOCTL SPI_IOC_WR_MAX_SPEED_HZ failed at line %u\n", __LINE__);

	return local_driver;
}

void spi_deinit(void)
{
	close(local_driver->fd);

	if (local_driver->is_allocated)
		free(local_driver);
}
