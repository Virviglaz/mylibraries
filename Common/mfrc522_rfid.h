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

void mifare_encode(uint32_t value, uint8_t *buf);
uint32_t mifare_decode(uint8_t *buf);
struct mfrc_t *mfrc_init(struct mfrc_t *init);

#endif /* MFRC522_H */
