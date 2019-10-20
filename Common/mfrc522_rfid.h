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

#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

enum mfrc_gain {
	RX_GAIN_18dB = 0x20,
	RX_GAIN_23dB = 0x30,
	RX_GAIN_33dB = 0x40,
	RX_GAIN_38dB = 0x50,
	RX_GAIN_43dB = 0x60,
	RX_GAIN_48dB = 0x70,
};

enum mfrc_status {
	MI_OK,
	MI_NOTAGERR,
	MI_ERR,
	MI_NOCARD,
	MI_COLLISION,
	MI_WRONG_CARD,
	MI_AUTH_ERROR,
	MI_READ_ERROR,
	MI_WRITE_ERROR,
	MI_WRONG_DATA,
	MI_WRONG_PARAM,
	MI_INTERFACE_ERROR,
};

enum mfrc_card_type
{
	MifareErr = 0x00,
	Mifare_1k = 0x08,
	Mifare_4k = 0x18,
};

enum mfrc_cmd {
	PCD_IDLE	= 0x00,   // NO action; Cancel the current command
	PCD_AUTHENT	= 0x0E,   // Authentication Key
	PCD_RECEIVE	= 0x08,   // Receive Data
	PCD_TRANSMIT	= 0x04,   // Transmit data
	PCD_TRANSCEIVE	= 0x0C,   // Transmit and receive data,
	PCD_RESETPHASE	= 0x0F,   // Reset
	PCD_CALCCRC	= 0x03,   // CRC Calculate
};

enum mfrc_auth
{
	PICC_AUTHENT1A = 0x60,
	PICC_AUTHENT1B = 0x61,
};

enum mfrc_op {
	CARD_READ,
	CARD_WRITE,
	CARD_RW,
	CARD_RW_ANYWAY,
};

struct mfrc_t {
	/* Settings */
	enum mfrc_gain rx_gain;

	struct {
		/* Functions */
		void (*wr)(uint8_t reg, uint8_t *buf, uint8_t size);
		void (*rd)(uint8_t reg, uint8_t *buf, uint8_t size);
	} io;

	enum mfrc_status(*operate)(uint8_t sector,
		void(*key_gen_func)(uint8_t *sn, uint8_t *key),
		void (*handler)(uint8_t *sn, uint8_t *value),
		enum mfrc_op task);

	void (*sleep)(void);
};

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

void mifare_encode(uint32_t value, uint8_t *buf);
uint32_t mifare_decode(uint8_t *buf);
struct mfrc_t *mfrc_init(struct mfrc_t *init);

#endif /* MFRC522_H */
