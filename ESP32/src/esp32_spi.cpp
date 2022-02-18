/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2022 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ESP32 spi driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "esp32_spi.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <cstring>

static const char *tag = "SPI";
static bool in_use[2] = { false, false };

/*
 * Initialize SPI bus.
 *
 * @param mosi		MOSI pin number (DATA OUT).
 * @param miso		MISO pin number (DATA IN).
 * @param msck		MSCK pin number (CLOCK OUT).
 * @param bus		HSPI or VSPI peripheral.
 */
spi_bus::spi_bus(int mosi, int miso, int msck, enum spi_num bus)
{
	if (in_use[bus - HSPI]) {
		ESP_LOGE(tag, "bus is already in use");
		return;
	}

	spi_num = (spi_host_device_t)bus;
	spi_bus_config_t pincfg;
	memset(&pincfg, 0, sizeof(pincfg));
	pincfg.mosi_io_num = mosi;
	pincfg.miso_io_num = miso;
	pincfg.sclk_io_num = msck;
	pincfg.quadwp_io_num = -1;
	pincfg.quadhd_io_num = -1;
	pincfg.max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE;
	pincfg.flags = SPICOMMON_BUSFLAG_MASTER;
	pincfg.intr_flags = 0;

	gpio_reset_pin((gpio_num_t)mosi);
	gpio_reset_pin((gpio_num_t)miso);
	gpio_reset_pin((gpio_num_t)msck);

	esp_err_t res = spi_bus_initialize(spi_num, &pincfg, SPI_DMA_DISABLED);
	if (res) {
		ESP_LOGE(tag, "bus initialize failed: %s",
			esp_err_to_name(res));
		return;
	}

	lock = xSemaphoreCreateMutex();

	in_use[bus - HSPI] = true;
}

spi_bus::~spi_bus()
{
	vSemaphoreDelete(lock);
	spi_bus_free(spi_num);
	in_use[spi_num - HSPI] = false;
}

/*
 * Creates a SPI device.
 *
 * @param bus		Pointer to SPI bus instance.
 * @param cs		Chip select pin number.
 * @param freq		SPI clock frequency.
 * @param clk_mode	CPOL/CPHA mode (SPI clock idle state).
 * @param addr_bits	Size of address register in bits.
 */
spi_dev::spi_dev(spi_bus *bus, int cs, enum spi_freq freq,
		enum spi_clk_mode clk_mode, uint8_t addr_bits)
{
	spi_device_interface_config_t dev_cfg;
	memset(&dev_cfg, 0, sizeof(dev_cfg));
	dev_cfg.clock_speed_hz = freq;
	dev_cfg.mode = 0;
	dev_cfg.spics_io_num = cs;
	dev_cfg.input_delay_ns = 0;
	dev_cfg.queue_size = 1;
	dev_cfg.pre_cb = NULL;
	dev_cfg.post_cb = NULL;
	dev_cfg.flags = 0;
	dev_cfg.address_bits = addr_bits;
	dev_cfg.command_bits = 0;
	dev_cfg.dummy_bits = 0;
	dev_cfg.duty_cycle_pos = 0;
	dev_cfg.cs_ena_pretrans = 0;
	dev_cfg.cs_ena_posttrans = 0;

	gpio_reset_pin((gpio_num_t)cs);

	lock = bus->lock;

	esp_err_t res = spi_bus_add_device(bus->spi_num, &dev_cfg, &handle);
	if (res) {
		ESP_LOGE(tag, "device initialize failed: %s",
			esp_err_to_name(res));
		return;
	}
}

spi_dev::~spi_dev()
{
	spi_bus_remove_device(handle);
}

esp_err_t spi_dev::transmit(uint32_t a, uint8_t *tx, uint8_t *rx, uint32_t size)
{
	spi_transaction_t msg;
	msg.length = size * 8;
	msg.tx_buffer = tx;
	msg.rx_buffer = rx;
	msg.rxlength = 0;
	msg.addr = a;
	msg.flags = 0;

	xSemaphoreTake(lock, portMAX_DELAY);
	esp_err_t res = spi_device_polling_transmit(handle, &msg);
	xSemaphoreGive(lock);
	if (res)
		ESP_LOGE(tag, "SPI transmit error: %s", esp_err_to_name(res));

	return res;
}

/*
 * Write data to register.
 *
 * @param reg		Register address.
 * @param buf		Pointer to the data buffer.
 * @param size		Size of data buffer.
 *
 * @return 		0 if success, error code if failed.
 */
esp_err_t spi_dev::wr_reg(uint32_t reg, uint8_t *buf, uint32_t size)
{
	return transmit(reg, buf, 0, size);
}

/*
 * Read data from register.
 *
 * @param reg		Register address.
 * @param buf		Pointer to the data buffer.
 * @param size		Size of data buffer.
 *
 * @return 		0 if success, error code if failed.
 */
esp_err_t spi_dev::rd_reg(uint32_t reg, uint8_t *buf, uint32_t size)
{
	return transmit(reg, 0, buf, size);
}
