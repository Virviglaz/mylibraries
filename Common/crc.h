/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2025 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
 * CRC calculation alghorithms.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __CRC_GENERIC_H__
#define __CRC_GENERIC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Calculate CRC32 checksum.
 *
 * @param buf		Pointer to data buffer.
 * @param len		Length of data buffer in bytes.
 *
 * @return Calculated CRC32 checksum.
 */
uint32_t crc32(uint8_t *buf, uint32_t len);

/**
 * Calculate CRC16 checksum.
 *
 * @param buf		Pointer to data buffer.
 * @param len		Length of data buffer in bytes.
 *
 * @return Calculated CRC16 checksum.
 */
uint16_t crc16(uint8_t *buf, uint16_t len);

/**
 * Calculate CRC8-Dallas/Maxim checksum.
 *
 * @param buf		Pointer to data buffer.
 * @param len		Length of data buffer in bytes.
 *
 * @return Calculated CRC8-Dallas/Maxim checksum.
 */
uint8_t crc8Dallas(uint8_t *buf, uint16_t len);

/**
 * Calculate CRC8 checksum.
 *
 * @param buf		Pointer to data buffer.
 * @param len		Length of data buffer in bytes.
 *
 * @return Calculated CRC8 checksum.
 */
uint8_t crc8(uint8_t *buf, uint16_t len);

/**
 * Calculate Fletcher-32 checksum.
 *
 * @param data		Pointer to data buffer (16-bit words).
 * @param len		Length of data buffer in 16-bit words.
 *
 * @return Calculated Fletcher-32 checksum.
 */
uint32_t fletcher32(uint16_t *data, uint32_t len);

/**
 * Calculate Fletcher-32 checksum for a string.
 *
 * @param str		Pointer to null-terminated string.
 *
 * @return Calculated Fletcher-32 checksum.
 */
uint32_t fletcher32_string(const char *str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRC_GENERIC_H__ */
