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
 * MFRC522 RFID driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <string.h>
#include "mfrc522_rfid.h"

#define MFRC522_SN_LEN			16
#define MFRC522_DATA_LEN		16
#define MFRC522_PASS_LEN		6
#define MFRC522_MAX_SECTORS		40

/* Mifare_One card command word */
#define PICC_REQIDL		0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL		0x52   // find all the cards antenna area
#define PICC_ANTICOLL		0x93   // anti-collision
#define PICC_SElECTTAG		0x93   // election card
#define PICC_READ		0x30   // Read Block
#define PICC_WRITE		0xA0   // write block
#define PICC_DECREMENT		0xC0   // debit
#define PICC_INCREMENT		0xC1   // reuint8_tge
#define PICC_RESTORE		0xC2   // transfer block data to the buffer
#define PICC_TRANSFER		0xB0   // save the data in the buffer
#define PICC_HALT		0x50   // Sleep

enum mfrc_cmd {
	PCD_IDLE	= 0x00,   // NO action; Cancel the current command
	PCD_AUTHENT	= 0x0E,   // Authentication Key
	PCD_RECEIVE	= 0x08,   // Receive Data
	PCD_TRANSMIT	= 0x04,   // Transmit data
	PCD_TRANSCEIVE	= 0x0C,   // Transmit and receive data,
	PCD_RESETPHASE	= 0x0F,   // Reset
	PCD_CALCCRC	= 0x03,   // CRC Calculate
};

enum mfrc_card_type
{
	MifareErr = 0x00,
	Mifare_1k = 0x08,
	Mifare_4k = 0x18,
};

enum mfrc_auth
{
	PICC_AUTHENT1A = 0x60,
	PICC_AUTHENT1B = 0x61,
};

enum mfrc_registers {
	 MFRC522_REG_RESERVED00		= 0x00,
	 MFRC522_REG_COMMAND		= 0x01,
	 MFRC522_REG_COMM_IE_N		= 0x02,
	 MFRC522_REG_DIV1_EN		= 0x03,
	 MFRC522_REG_COMM_IRQ		= 0x04,
	 MFRC522_REG_DIV_IRQ		= 0x05,
	 MFRC522_REG_ERROR		= 0x06,
	 MFRC522_REG_STATUS1		= 0x07,
	 MFRC522_REG_STATUS2		= 0x08,
	 MFRC522_REG_FIFO_DATA		= 0x09,
	 MFRC522_REG_FIFO_LEVEL		= 0x0A,
	 MFRC522_REG_WATER_LEVEL	= 0x0B,
	 MFRC522_REG_CONTROL		= 0x0C,
	 MFRC522_REG_BIT_FRAMING	= 0x0D,
	 MFRC522_REG_COLL		= 0x0E,
	 MFRC522_REG_RESERVED01		= 0x0F,

	 MFRC522_REG_RESERVED10		= 0x10,
	 MFRC522_REG_MODE		= 0x11,
	 MFRC522_REG_TX_MODE		= 0x12,
	 MFRC522_REG_RX_MODE		= 0x13,
	 MFRC522_REG_TX_CONTROL		= 0x14,
	 MFRC522_REG_TX_AUTO		= 0x15,
	 MFRC522_REG_TX_SELL		= 0x16,
	 MFRC522_REG_RX_SELL		= 0x17,
	 MFRC522_REG_RX_THRESHOLD	= 0x18,
	 MFRC522_REG_DEMOD		= 0x19,
	 MFRC522_REG_RESERVED11		= 0x1A,
	 MFRC522_REG_RESERVED12		= 0x1B,
	 MFRC522_REG_MIFARE		= 0x1C,
	 MFRC522_REG_RESERVED13		= 0x1D,
	 MFRC522_REG_RESERVED14		= 0x1E,
	 MFRC522_REG_SERIALSPEED	= 0x1F,

	 MFRC522_REG_RESERVED20		= 0x20,
	 MFRC522_REG_CRC_RESULT_M	= 0x21,
	 MFRC522_REG_CRC_RESULT_L	= 0x22,
	 MFRC522_REG_RESERVED21		= 0x23,
	 MFRC522_REG_MOD_WIDTH		= 0x24,
	 MFRC522_REG_RESERVED22		= 0x25,
	 MFRC522_REG_RF_CFG		= 0x26,
	 MFRC522_REG_GS_N		= 0x27,
	 MFRC522_REG_CWGS_PREG		= 0x28,
	 MFRC522_REG__MODGS_PREG	= 0x29,
	 MFRC522_REG_T_MODE		= 0x2A,
	 MFRC522_REG_T_PRESCALER	= 0x2B,
	 MFRC522_REG_T_RELOAD_H		= 0x2C,
	 MFRC522_REG_T_RELOAD_L		= 0x2D,
	 MFRC522_REG_T_COUNTER_VALUE_H	= 0x2E,
	 MFRC522_REG_T_COUNTER_VALUE_L	= 0x2F,

	 MFRC522_REG_RESERVED30		= 0x30,
	 MFRC522_REG_TEST_SEL1		= 0x31,
	 MFRC522_REG_TEST_SEL2		= 0x32,
	 MFRC522_REG_TEST_PIN_EN	= 0x33,
	 MFRC522_REG_TEST_PIN_VALUE	= 0x34,
	 MFRC522_REG_TEST_BUS		= 0x35,
	 MFRC522_REG_AUTO_TEST		= 0x36,
	 MFRC522_REG_VERSION		= 0x37,
	 MFRC522_REG_ANALOG_TEST	= 0x38,
	 MFRC522_REG_TEST_ADC1		= 0x39,
	 MFRC522_REG_TEST_ADC2		= 0x3A,
	 MFRC522_REG_TEST_ADC0		= 0x3B,
	 MFRC522_REG_RESERVED31		= 0x3C,
	 MFRC522_REG_RESERVED32		= 0x3D,
	 MFRC522_REG_RESERVED33		= 0x3E,
	 MFRC522_REG_RESERVED34		= 0x3F,
};

/* Local pointer to existing outside configuration structure */
static struct mfrc_t *mfrc_driver = 0;

static void write_reg(enum mfrc_registers addr, uint8_t val)
{
	addr <<= 1;
	addr &= 0x7E;

	mfrc_driver->io.wr(addr, &val, sizeof(val));
}

static uint8_t read_reg (enum mfrc_registers addr)
{
	uint8_t val;

	addr <<= 1;
	addr &= 0x7E;
	addr |= 0x80;

	mfrc_driver->io.rd(addr, &val, sizeof(val));

	return val;
}

inline static void mfrc_set_bit_mask(enum mfrc_registers reg, uint8_t mask)
{
	write_reg(reg, read_reg(reg) | mask);
}

inline static void mfrc_clear_bit_mask(enum mfrc_registers reg, uint8_t mask)
{
	write_reg(reg, read_reg(reg) & (~mask));
}

static void mfrc_tx_enable (void)
{
	if (!(read_reg(MFRC522_REG_TX_CONTROL) & 0x03))
		mfrc_set_bit_mask(MFRC522_REG_TX_CONTROL, 0x03);
}

inline static void mfrc_tx_disable(void)
{
	mfrc_clear_bit_mask(MFRC522_REG_TX_CONTROL, 0x03);
}

inline static void mfrc_reset(void)
{
	write_reg(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

static enum mfrc_status mfrc_to_card(enum mfrc_cmd command,
		uint8_t *sendData, uint8_t len,
		uint8_t *backData, uint16_t *backLen)
{
	enum mfrc_status status = MI_ERR;
	uint8_t irqEn, waitIRq, lastBits, n;
	uint16_t i;

	switch (command) {
	case PCD_AUTHENT:
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	case PCD_TRANSCEIVE:
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	default:
		return MI_WRONG_PARAM;
	}

	write_reg(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	mfrc_clear_bit_mask(MFRC522_REG_COMM_IRQ, 0x80);
	mfrc_set_bit_mask(MFRC522_REG_FIFO_LEVEL, 0x80);
	write_reg(MFRC522_REG_COMMAND, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i != len; i++)
		write_reg(MFRC522_REG_FIFO_DATA, sendData[i]);

	write_reg(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE)
		mfrc_set_bit_mask(MFRC522_REG_BIT_FRAMING, 0x80);

	i = 2000;
	do {
		n = read_reg(MFRC522_REG_COMM_IRQ);
		i--;
	} while (i && !(n & 0x01) && !(n & waitIRq));

	mfrc_clear_bit_mask(MFRC522_REG_BIT_FRAMING, 0x80);

	if (i) {
		if (!(read_reg(MFRC522_REG_ERROR) & 0x1B)) {
			status = MI_OK;
			if (n & irqEn & 0x01)
				status = MI_NOTAGERR;

			if (command == PCD_TRANSCEIVE) {
				n = read_reg(MFRC522_REG_FIFO_LEVEL);
				lastBits = read_reg(MFRC522_REG_CONTROL) & 0x07;

				if (lastBits)
					*backLen = (n - 1) * 8 + lastBits;
				else
					*backLen = n * 8;

				if (n == 0)
					n = 1;

				if (n > MFRC522_SN_LEN)
					n = MFRC522_SN_LEN;

				for (i = 0; i != n; i++)
					backData[i] =
						read_reg(MFRC522_REG_FIFO_DATA);
			}
		} else
			status = MI_ERR;
	}
	return status;
}

static enum mfrc_status mfrc_request (uint8_t reqMode, uint8_t *tag)
{
	enum mfrc_status status;
	uint16_t backBits;

	write_reg(MFRC522_REG_BIT_FRAMING, 0x07);
	tag[0] = reqMode;
	status = mfrc_to_card(PCD_TRANSCEIVE, tag, 1, tag, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
		status = MI_ERR;

	return status;
}

static enum mfrc_status mfrc_anti_collision (uint8_t *sn)
{
	enum mfrc_status status;
	uint8_t i, sn_check = 0;
	uint16_t unLen;

	sn_check = 0;
	write_reg(MFRC522_REG_BIT_FRAMING, 0x00);
	sn[0] = PICC_ANTICOLL;
	sn[1] = 0x20;
	status = mfrc_to_card(PCD_TRANSCEIVE, sn, 2, sn, &unLen);

	if (status == MI_OK) {
		for (i = 0; i != 4; i++)
			sn_check ^= sn[i];
		if (sn_check != sn[i])
			status = MI_ERR;
	}
	return status;
}

static void mfrc_calc_crc (uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
	uint8_t i, n;

	mfrc_clear_bit_mask(MFRC522_REG_DIV_IRQ, 0x04);
	mfrc_set_bit_mask(MFRC522_REG_FIFO_LEVEL, 0x80);

	// Writing data to the FIFO
	for (i = 0; i != len; i++)
		write_reg(MFRC522_REG_FIFO_DATA, *(pIndata+i));

	write_reg(MFRC522_REG_COMMAND, PCD_CALCCRC);

	// Wait CRC calculation is complete
	i = 0xFF;
	do {
		n = read_reg(MFRC522_REG_DIV_IRQ);
		i--;
	} while (i && !(n & 0x04));

	// Read CRC calculation result
	pOutData[0] = read_reg(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = read_reg(MFRC522_REG_CRC_RESULT_M);
}

static enum mfrc_card_type mfrc_select_tag (uint8_t *sn)
{
	uint8_t i;
	enum mfrc_status status;
	enum mfrc_card_type CardType;
	uint16_t recvBits;
	uint8_t buf[9];

	buf[0] = PICC_SElECTTAG;
	buf[1] = 0x70;

	for (i = 0; i != 5; i++)
		buf[i + 2] = *(sn + i);

	mfrc_calc_crc(buf, 7, &buf[7]);

	status = mfrc_to_card(PCD_TRANSCEIVE, buf, 9, buf, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18))
		CardType = (enum mfrc_card_type)buf[0];
	else
		CardType = MifareErr;

	return CardType;
}

static enum mfrc_status mfrc_auth(enum mfrc_auth auth,
	uint8_t block, uint8_t *sector, uint8_t *sn)
{
	enum mfrc_status status;
	uint16_t recvBits;
	uint8_t i, buf[12];

	buf[0] = auth;
	buf[1] = block;

	for (i = 0; i != 6; i++)
		buf[i + 2] = *(sector + i);

	for (i=0; i != 4; i++)
		buf[i + 8] = *(sn + i);

	status = mfrc_to_card(PCD_AUTHENT, buf, 12, buf, &recvBits);
	if ((status != MI_OK) || (!(read_reg(MFRC522_REG_STATUS2) & 0x08)))
		status = MI_ERR;

	return status;
}

static enum mfrc_status mfrc_read(uint8_t block, uint8_t *data)
{
	enum mfrc_status status;
	uint16_t unLen;

	data[0] = PICC_READ;
	data[1] = block;

	mfrc_calc_crc(data, 2, &data[2]);

	status = mfrc_to_card(PCD_TRANSCEIVE, data, 4, data, &unLen);

	if ((status != MI_OK) || (unLen != 0x90))
		status = MI_ERR;

	return status;
}

static enum mfrc_status mfrc_write(uint8_t block, uint8_t *data)
{
	enum mfrc_status status;
	uint16_t recvBits;
	uint8_t i, buf[MFRC522_DATA_LEN + 2];

	buf[0] = PICC_WRITE;
	buf[1] = block;

	mfrc_calc_crc(buf, 2, &buf[2]);

	status = mfrc_to_card(PCD_TRANSCEIVE, buf, 4, buf, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buf[0] & 0x0F) != 0x0A))
		status = MI_ERR;

	if (status == MI_OK) {
		// Data to the FIFO write 16Byte
		for (i = 0; i != MFRC522_DATA_LEN; i++)
			buf[i] = *(data+i);

		mfrc_calc_crc(buf, 16, &buf[16]);

		status = mfrc_to_card(PCD_TRANSCEIVE, buf, sizeof(buf),
				      buf, &recvBits);
		if ((status != MI_OK) || (recvBits != 4) || \
			((buf[0] & 0x0F) != 0x0A))
			status = MI_ERR;
	}

	return status;
}

static void mfrc_restart(void)
{
	mfrc_reset();
	write_reg(MFRC522_REG_T_MODE, 0x8D);
	write_reg(MFRC522_REG_T_PRESCALER, 0x3E);
	write_reg(MFRC522_REG_T_RELOAD_L, 30);
	write_reg(MFRC522_REG_T_RELOAD_H, 0);
	write_reg(MFRC522_REG_RF_CFG, mfrc_driver->rx_gain);
	write_reg(MFRC522_REG_TX_AUTO, 0x40);
	write_reg(MFRC522_REG_MODE, 0x3D);
	mfrc_tx_disable();
}

/**
  * @brief  High Level. Read data from one sector.
  * @param  sn: pointer to serial number array.
  * @param  sector: sector number.
  * @param  key: pointer to access key array.
  * @param  handler: Handler, that receive data from card.
  * @param  task: CARD_READ, CARD_WRITE or CARD_RW.
  * @retval None
  */
volatile void *p;
enum mfrc_status mfrc_operate(uint8_t sector,
	void (*keygen_func)(uint8_t *sn, uint8_t *key),
	void (*handler)(uint8_t *sn, uint8_t *value), enum mfrc_op task)
{
	enum mfrc_status res = MI_OK;
	uint8_t serial[MFRC522_SN_LEN];
	uint8_t data[MFRC522_DATA_LEN];
	uint8_t pass[MFRC522_PASS_LEN];

	/* Check params */
	if (sector >= MFRC522_MAX_SECTORS)
		return MI_WRONG_PARAM;

	p = serial;
	/* Enable transmitter */
	mfrc_tx_enable();

	/* Check card presense */
	if (mfrc_request(PICC_REQIDL, serial) != MI_OK) {
		res = MI_NOCARD;
		goto operate_end;
	}

	/* Check link for collision */
	if (mfrc_anti_collision(serial) != MI_OK) {
		res = MI_COLLISION;
		goto operate_end;
	}

	/* Check cart type. Only 1k supported for now */
	if (mfrc_select_tag(serial) != Mifare_1k) {
		res = MI_WRONG_CARD;
		goto operate_end;
	}

	/* Generate key */
	keygen_func(serial, pass);

	/* Authorise with provided key */
	if (mfrc_auth(PICC_AUTHENT1A, sector, pass, serial) != MI_OK) {
		res = MI_AUTH_ERROR;
		goto operate_end;
	}

	/* Read EEPROM data */
	if (task != CARD_WRITE && mfrc_read(sector, data) != MI_OK) {
			res = MI_READ_ERROR;
			goto operate_end;
	}

	/* Execute event handler */
	handler(serial, data);

	/* Return data, no write */
	if (task == CARD_READ)
		goto operate_end;

	/* Write data back to card */
	if (mfrc_write(sector, data) != MI_OK) {
		res = MI_WRITE_ERROR;
		goto operate_end;
	}

operate_end:
	mfrc_restart();
	return res;
}

static enum mfrc_status mfrc_check_interface(void)
{
	uint8_t vers = read_reg(MFRC522_REG_VERSION);

	if (vers == 0x91 || vers == 0x92)
		return MI_OK;

	return MI_INTERFACE_ERROR;
}

static void mfrc_sleep(void)
{
	uint16_t unLen;
	uint8_t buf[4];

	buf[0] = PICC_HALT;
	buf[1] = 0;
	mfrc_calc_crc(buf, 2, &buf[2]);
	mfrc_to_card(PCD_TRANSCEIVE, buf, 4, buf, &unLen);
}

void mifare_encode(uint32_t value, uint8_t *buf)
{
	memcpy(buf, (void *)&value, sizeof(value));
	buf += sizeof(value);
	value = ~value;

	memcpy(buf, (void *)&value, sizeof(value));
	buf += sizeof(value);
	value = ~value;

	memcpy(buf, (void *)&value, sizeof(value));
	buf += sizeof(value);
}

uint32_t mifare_decode(uint8_t *buf)
{
	uint32_t value;
	memcpy((void *)&value, buf, sizeof(value));

	return value;
}

struct mfrc_t *mfrc_init(struct mfrc_t *init)
{
	if (init) {
		mfrc_driver = init;
		mfrc_driver->operate = mfrc_operate;
		mfrc_driver->sleep = mfrc_sleep;

		if (!mfrc_driver->rx_gain)
			mfrc_driver->rx_gain = RX_GAIN_48dB;

		if (mfrc_check_interface())
			return 0;

		mfrc_restart();
	}

	return mfrc_driver;
}
