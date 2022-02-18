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

#ifndef __ESP32_SPI_H__
#define __ESP32_SPI_H__

enum spi_num { HSPI = 2, VSPI = 3 };

#include <stdint.h>
#include "driver/spi_master.h"

class spi_bus
{
public:
	spi_bus(int mosi, int miso, int msck, enum spi_num bus = HSPI);
	~spi_bus();
	spi_host_device_t spi_num;
};

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

class spi_dev
{
public:
	spi_dev(spi_bus *bus, int cs, enum spi_freq freq,
		enum spi_clk_mode clk_mode, uint8_t addr_bits = 8);
	~spi_dev();

	esp_err_t wr_reg(uint32_t reg, uint8_t *buf, uint32_t size);
	esp_err_t rd_reg(uint32_t reg, uint8_t *buf, uint32_t size);
private:
	esp_err_t transmit(uint32_t a, uint8_t *tx, uint8_t *rx, uint32_t size);
	spi_device_handle_t handle = NULL;
};

#endif /* __ESP32_SPI_H__ */
