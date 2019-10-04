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

#include "mfrc522_rfid.h"

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

/* Prepare data according general rule of data storage in MIFARE */
static void mfrc_encode_data(uint32_t value, uint8_t *data)
{
	union {
		uint32_t v32;
		uint8_t v8[sizeof(uint32_t)];
	} V32b;
	V32b.v32 = value;

	/* value */
	data[0] = V32b.v8[0];
	data[1] = V32b.v8[1];
	data[2] = V32b.v8[2];
	data[3] = V32b.v8[3];

	/* NOT value */
	data[4] = ~V32b.v8[0];
	data[5] = ~V32b.v8[1];
	data[6] = ~V32b.v8[2];
	data[7] = ~V32b.v8[3];

	/* value */
	data[8] = V32b.v8[0];
	data[9] = V32b.v8[1];
	data[10] = V32b.v8[2];
	data[11] = V32b.v8[3];
}

static enum mfrc_status mfrc_decode_data(uint8_t *data, uint32_t *value)
{
	enum mfrc_status res = MI_OK;
	uint8_t cnt;
	union {
		uint32_t v32;
		uint8_t v8[sizeof(uint32_t)];
	} V32b;

	for (cnt = 0; cnt != sizeof(uint32_t); cnt++)
		if (data[cnt] != data[cnt + 8])
			res = MI_WRONG_DATA;

	V32b.v8[0] = data[0];
	V32b.v8[1] = data[1];
	V32b.v8[2] = data[2];
	V32b.v8[3] = data[3];

	*value = V32b.v32;
	return res;
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
	uint8_t irqEn = 0, waitIRq = 0, lastBits, n;
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

				if (n > MFRC522_MAX_LEN)
					n = MFRC522_MAX_LEN;

				for (i = 0; i < n; i++)
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
	uint8_t buffer[9];

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;

	for (i = 0; i != 5; i++)
		buffer[i + 2] = *(sn + i);

	mfrc_calc_crc(buffer, 7, &buffer[7]);

	status = mfrc_to_card(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18))
		CardType = (enum mfrc_card_type)buffer[0];
	else
		CardType = MifareErr;

	return CardType;
}

static enum mfrc_status mfrc_auth(enum mfrc_auth auth,
	uint8_t block, uint8_t *sector, uint8_t *sn)
{
	enum mfrc_status status;
	uint16_t recvBits;
	uint8_t i, buff[12];

	buff[0] = auth;
	buff[1] = block;

	for (i = 0; i != 6; i++)
		buff[i + 2] = *(sector + i);

	for (i=0; i != 4; i++)
		buff[i + 8] = *(sn + i);

	status = mfrc_to_card(PCD_AUTHENT, buff, 12, buff, &recvBits);
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
	uint8_t i, buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = block;

	mfrc_calc_crc(buff, 2, &buff[2]);

	status = mfrc_to_card(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		status = MI_ERR;

	if (status == MI_OK) {
		// Data to the FIFO write 16Byte
		for (i = 0; i != 16; i++)
			buff[i] = *(data+i);

		mfrc_calc_crc(buff, 16, &buff[16]);

		status = mfrc_to_card(PCD_TRANSCEIVE, buff,
				      18, buff, &recvBits);
		if ((status != MI_OK) || (recvBits != 4) || \
			((buff[0] & 0x0F) != 0x0A))
			status = MI_ERR;
	}

	return status;
}

/**
  * @brief  High Level. Read and decode data from one sector.
  * @param  sn: pointer to serial number array.
  * @param  sector: sector number.
  * @param  key: pointer to access key array.
  * @param  handler: Handler, that receive 32 bit value from card.
  * @param  task: CARD_READ, CARD_WRITE, CARD_RW or CARD_RW_ANYWAY in case data format error.
  * @retval None
  */
static enum mfrc_status mfrc_operate32(uint8_t *sn, uint8_t sector,
	uint8_t *key, void (*key_gen_func)(uint8_t *sn, uint8_t *key),
	void (*handler)(uint8_t *sn, uint32_t *data), enum mfrc_op task)
{
	enum mfrc_status res = MI_OK;
	uint8_t data[16], uid[2];
	uint32_t value;

	/* Check params */
	if (sector > 39)
		return MI_WRONG_PARAM;

	/* Check card presense */
	if (mfrc_request(PICC_REQIDL, uid) != MI_OK)
		return MI_NOCARD;

	/* Check link for collision */
	if (mfrc_anti_collision(sn) != MI_OK)
		return MI_COLLISION;

	/* Check cart type. Only 1k supported for now */
	if (mfrc_select_tag(sn) != Mifare_1k)
		return MI_WRONG_CARD;

	/* Generate key */
	key_gen_func(sn, key);

	/* Authorise with provided key */
	if (mfrc_auth(PICC_AUTHENT1A, sector, key, sn) != MI_OK)
		return MI_AUTH_ERROR;

	/* Read EEPROM data */
	if (task != CARD_WRITE) {
		if (mfrc_read(sector, data) != MI_OK)
			return MI_READ_ERROR;

		/* Check data is valid */
		res = mfrc_decode_data(data, &value);
	}

	/* Execute event handler */
	handler(sn, &value);

	/* Return data, no write */
	if (task == CARD_READ)
		return res;

	/* Check what to do. Procced writing anyway, but return error */
	if (res != MI_OK && task != CARD_RW_ANYWAY)
		return MI_WRONG_DATA;

	/* Encode data */
	mfrc_encode_data(value, data);

	/* Write data back to card */
	if (mfrc_write(sector, data) != MI_OK)
		return MI_WRITE_ERROR;

	return res;
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
enum mfrc_status mfrc_operate(uint8_t *sn, uint8_t sector,
	uint8_t *key, void (*key_gen_func)(uint8_t *sn, uint8_t *key),
	void (*handler)(uint8_t *sn, uint8_t *value), enum mfrc_op task)
{
	enum mfrc_status res = MI_OK;
	uint8_t data[16];

	/* Check params */
	if (sector > 39)
		return MI_WRONG_PARAM;

	/* Enable transmitter */
	mfrc_tx_enable();

	/* Check card presense */
	if (mfrc_request(PICC_REQIDL, sn) != MI_OK)
		return MI_NOCARD;

	/* Check link for collision */
	if (mfrc_anti_collision(sn) != MI_OK)
		return MI_COLLISION;

	/* Check cart type. Only 1k supported for now */
	if (mfrc_select_tag(sn) != Mifare_1k)
		return MI_WRONG_CARD;

	/* Generate key */
	key_gen_func(sn, key);

	/* Authorise with provided key */
	if (mfrc_auth(PICC_AUTHENT1A, sector, key, sn) != MI_OK)
		return MI_AUTH_ERROR;

	/* Read EEPROM data */
	if (task != CARD_WRITE)
		if (mfrc_read(sector, data) != MI_OK)
			return MI_READ_ERROR;

	/* Execute event handler */
	handler(sn, data);

	/* Return data, no write */
	if (task == CARD_READ)
		return res;

	/* Write data back to card */
	if (mfrc_write(sector, data) != MI_OK)
		return MI_WRITE_ERROR;

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
	uint8_t buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	mfrc_calc_crc(buff, 2, &buff[2]);
	mfrc_to_card(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}


struct mfrc_t *mfrc_init(struct mfrc_t *init)
{
	if (init) {
		mfrc_driver = init;
		mfrc_driver->operate32 = mfrc_operate32;
		mfrc_driver->operate = mfrc_operate;
		mfrc_driver->sleep = mfrc_sleep;

		if (!mfrc_driver->rx_gain)
			mfrc_driver->rx_gain = RX_GAIN_48dB;

		mfrc_reset();
		if (mfrc_check_interface())
			return 0;

		write_reg(MFRC522_REG_T_MODE, 0x8D);
		write_reg(MFRC522_REG_T_PRESCALER, 0x3E);
		write_reg(MFRC522_REG_T_RELOAD_L, 30);
		write_reg(MFRC522_REG_T_RELOAD_H, 0);
		write_reg(MFRC522_REG_RF_CFG, init->rx_gain);
		write_reg(MFRC522_REG_TX_AUTO, 0x40);
		write_reg(MFRC522_REG_MODE, 0x3D);
		mfrc_tx_disable();
	}

	return mfrc_driver;
}