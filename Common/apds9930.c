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
 * APDS-9930 Digital Proximity and Ambient Light Sensor
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "apds9930.h"

#define APDS_ENABLE_REG		0x00 /* R/W Enable of states and interrupts */
#define APDS_ATIME_REG		0x01 /* R/W ALS ADC time */
#define APDS_PTIME_REG		0x02 /* R/W Proximity ADC time */
#define APDS_WTIME_REG		0x03 /* R/W Wait time */
#define APDS_AILTL_REG		0x04 /* R/W ALS interrupt low threshold low byte */
#define APDS_AILTH_REG		0x05 /* R/W ALS interrupt low threshold hi byte */
#define APDS_AIHTL_REG		0x06 /* R/W ALS interrupt hi threshold low byte */
#define APDS_AIHTH_REG		0x07 /* R/W ALS interrupt hi threshold hi byte */
#define APDS_PILTL_REG		0x08 /* R/W Proximity interrupt low threshold low byte */
#define APDS_PILTH_REG		0x09 /* R/W Proximity interrupt low threshold hi byte */
#define APDS_PIHTL_REG		0x0A /* R/W Proximity interrupt hi threshold low byte */
#define APDS_PIHTH_REG		0x0B /* R/W Proximity interrupt hi threshold hi byte */
#define APDS_PERS_REG		0x0C /* R/W Interrupt persistence filters */
#define APDS_CONFIG_REG		0x0D /* R/W Configuration */
#define APDS_PPULSE_REG		0x0E /* R/W Proximity pulse count */
#define APDS_CONTROL_REG	0x0F /* R/W Gain control register */
#define APDS_ID_REG		0x12 /* R Device ID ID */
#define APDS_STATUS_REG		0x13 /* R Device status */
#define APDS_CH0DATAL_REG	0x14 /* R Ch0 ADC low data register */
#define APDS_CH0DATAH_REG	0x15 /* R Ch0 ADC high data register */
#define APDS_CH1DATAL_REG	0x16 /* R Ch1 ADC low data register */
#define APDS_CH1DATAH_REG	0x17 /* R Ch1 ADC high data register */
#define APDS_PDATAL_REG		0x18 /* R Proximity ADC low data register */
#define APDS_PDATAH_REG		0x19 /* R Proximity ADC high data register */
#define APDS_POFFSET_REG	0x1E /* R/W Proximity offset register */

uint8_t apds9930_init(struct apds9930 *dev)
{
	uint8_t id;
	dev->read_reg(APDS_ID_REG, &id, 1);
	
	if (id == 0x39)
		return 0;
	return 1;
}
