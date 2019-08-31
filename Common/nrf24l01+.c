/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *   
 *   Copyright (c) 2019 Pavel Nadein
 *   
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *   
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *   
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * nRF24L01 open source library/driver
 *
 * Contact Information:
 * Pavel Nadein <pavel.nadein@gmail.com>
 */

#include "nrf24l01+.h"

#define CONFIG_REG				0x00
#define EN_AA_REG					0x01
#define EN_RXADDR_REG				0x02
#define SETUP_AW_REG				0x03
#define SETUP_RETR_REG			0x04
#define RF_CH_REG					0x05
#define RF_SETUP_REG				0x06
#define STATUS_REG				0x07
#define OBSERV_TX_REG				0x08
#define CD_REG					0x09
#define RX_ADDR_P0_REG			0x0A
#define RX_ADDR_P1_REG			0x0B
#define RX_ADDR_P2_REG			0x0C
#define RX_ADDR_P3_REG			0x0D
#define RX_ADDR_P4_REG			0x0E
#define RX_ADDR_P5_REG			0x0F
#define TX_ADDR_REG				0x10
#define RX_PW_P0_REG				0x11
#define RX_PW_P1_REG				0x12
#define RX_PW_P2_REG				0x13
#define RX_PW_P3_REG				0x14
#define RX_PW_P4_REG				0x15
#define RX_PW_P5_REG				0x16
#define FIFO_STATUS_REG			0x17

#define SEND_PAYLOAD_CMD			0xA0
#define RX_DR_IRQ_MASK			0x40
#define TX_DS_IRQ_MASK			0x20
#define MAX_RT_IRQ_MASK			0x10
#define FLUSH_TX_CMD				0xE1
#define FLUSH_RX_CMD				0xE2
#define RX_PAYLOAD_CMD			0x61
#define PIPE_BITMASK				0x0E
#define RX_FIFO_EMPTY_MASK		0x01
#define INVERT_IRQ_BITS				0x70

#define MAX_PIPE_SIZE			32

#define MIN(a,b) (((a)<(b))?(a):(b))

static struct nrf24l01_conf *local_driver;

static void write_reg(uint8_t reg, uint8_t value)
{
	local_driver->interface.error =
		local_driver->interface.write((0x1F & reg) | (1 << 5),
			&value, sizeof(value));
}

static uint8_t read_reg(uint8_t reg)
{
	uint8_t res;
	local_driver->interface.error =
		local_driver->interface.read(0x1F & reg, &res, sizeof(res));

	return res;
}

static void write_address(uint8_t reg, uint8_t *address)
{
	if (address)
		local_driver->interface.write(reg | (1 << 5), address, 5);
}

static bool read_irq(uint8_t irq_mask)
{
	if (local_driver->interface.read_irq)
		return !local_driver->interface.read_irq();

	return (read_reg(STATUS_REG)) & irq_mask;
}

static void clear_irq(uint8_t irq_mask)
{
	write_reg(STATUS_REG, irq_mask);
}

static void flush_buffer(uint8_t cmd)
{
	local_driver->interface.write(cmd, 0, 0);
}

static void update_config(void)
{
	write_reg(CONFIG_REG, *(uint8_t *)&local_driver->config ^ INVERT_IRQ_BITS);
}

static void disable_radio (void)
{
	write_reg(CONFIG_REG, 0x00);
}

static bool send(uint8_t *dest, uint8_t *data, uint8_t size)
{
	uint32_t cnt = local_driver->read_cnt;
	bool res, in_rx = local_driver->config.mode == RADIO_RX;
	uint8_t tx_irq = local_driver->auto_retransmit.count ?
		MAX_RT_IRQ_MASK | TX_DS_IRQ_MASK : TX_DS_IRQ_MASK;

	if (dest) /* Change distanation if needed */
		write_address(TX_ADDR_REG, dest);
	
	local_driver->interface.radio_en(true);
	
	local_driver->mode(RADIO_TX, true);

	local_driver->interface.write(SEND_PAYLOAD_CMD, data, size);
	
	while (!read_irq(tx_irq) && --cnt);

	if (local_driver->auto_retransmit.count)
		/* Check maximum retransmit is set, return false */
		res = !(read_reg(STATUS_REG) & MAX_RT_IRQ_MASK);
	else
		/* Just check that chip is responding */
		res = !(cnt == 0);

	clear_irq(tx_irq);
	flush_buffer(FLUSH_TX_CMD);
	
	/* Switch back to RX */
	if (in_rx)
		local_driver->mode(RADIO_RX, true);
	else
		local_driver->interface.radio_en(false);

	return res;
}

static uint8_t recv(uint8_t *data, uint8_t *pipe_num)
{
	uint8_t num;
	
	if (local_driver->config.mode == RADIO_TX) {
		local_driver->mode(RADIO_RX, true);
		local_driver->interface.radio_en(true);
	}

	if (!read_irq(RX_DR_IRQ_MASK))
		return 0; /* FIFO empty */

	/* Calculate pipe num */
	num = read_reg(STATUS_REG);
	num &= PIPE_BITMASK;
	num >>= 1;

	if (pipe_num)
		*pipe_num = num;
	num = local_driver->rx_pipe_size[num];

	local_driver->interface.read(RX_PAYLOAD_CMD, data, num);

	/* Clear IRQ only if last data received (using FIFO) */
	if (read_reg(FIFO_STATUS_REG) & RX_FIFO_EMPTY_MASK) {
		clear_irq(RX_DR_IRQ_MASK);
		flush_buffer(FLUSH_RX_CMD);
	}

	/* Return number of bytes in pipe */
	return num;
}

static void switch_mode(enum radio_mode mode, bool power_enable)
{
	local_driver->config.mode = mode;
	local_driver->config.power_enable = power_enable;

	disable_radio();

	if (mode == RADIO_TX) {
		local_driver->config.tx_irq = true;
		local_driver->config.rx_irq = false;
		local_driver->config.max_rt_irq = local_driver->auto_retransmit.count;
	} else { /* RX MODE */
		local_driver->config.rx_irq = true;
		local_driver->config.tx_irq = false;
		local_driver->config.max_rt_irq = false;

		/* Turn on radio in RX mode */
		local_driver->interface.radio_en(power_enable);
	}

	update_config();
}

static void sleep(void)
{
	local_driver->interface.radio_en(false);
	switch_mode(local_driver->config.mode, false);
}

static void wakeup(void)
{
	switch_mode(local_driver->config.mode, true);

	if (local_driver->config.mode == RADIO_RX)
		local_driver->interface.radio_en(true);
}

/**
  * @brief  Configures the nRF24L01
  * @param  driver: Pointer to nrf24l01_conf infostructure.
  * @retval value, returned back by interface function (can be used for error handling)
  */
uint8_t nrf24l01_init(struct nrf24l01_conf *driver)
{
	local_driver = driver;
	local_driver->send = send;
	local_driver->recv = recv;
	local_driver->mode = switch_mode;
	local_driver->sleep = sleep;
	local_driver->wakeup = wakeup;

	disable_radio();
	write_reg(EN_AA_REG, *(uint8_t *)&local_driver->auto_acknowledgment);
	write_reg(EN_RXADDR_REG, *(uint8_t *)&local_driver->enabled_rx_addresses);
	write_reg(SETUP_AW_REG, *(uint8_t *)&local_driver->address_widths);
	write_reg(SETUP_RETR_REG, *(uint8_t *)&local_driver->auto_retransmit);
	write_reg(RF_CH_REG, local_driver->channel & 0x7F);
	write_reg(RF_SETUP_REG, *(uint8_t *)&local_driver->setup);
	write_address(RX_ADDR_P0_REG, local_driver->rx_address_p0);
	write_address(RX_ADDR_P1_REG, local_driver->rx_address_p1);
	write_address(TX_ADDR_REG, local_driver->tx_address);
	write_reg(RX_ADDR_P2_REG, local_driver->rx_address_p2);
	write_reg(RX_ADDR_P3_REG, local_driver->rx_address_p3);
	write_reg(RX_ADDR_P4_REG, local_driver->rx_address_p4);
	write_reg(RX_ADDR_P5_REG, local_driver->rx_address_p5);
	write_reg(RX_PW_P0_REG, MIN(local_driver->rx_pipe_size[0], MAX_PIPE_SIZE));
	write_reg(RX_PW_P1_REG, MIN(local_driver->rx_pipe_size[1], MAX_PIPE_SIZE));
	write_reg(RX_PW_P2_REG, MIN(local_driver->rx_pipe_size[2], MAX_PIPE_SIZE));
	write_reg(RX_PW_P3_REG, MIN(local_driver->rx_pipe_size[3], MAX_PIPE_SIZE));
	write_reg(RX_PW_P4_REG, MIN(local_driver->rx_pipe_size[4], MAX_PIPE_SIZE));
	write_reg(RX_PW_P5_REG, MIN(local_driver->rx_pipe_size[5], MAX_PIPE_SIZE));

	switch_mode(local_driver->config.mode, local_driver->config.power_enable);

	return local_driver->interface.error;
}
