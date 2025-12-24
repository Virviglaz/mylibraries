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
 * Bitwise operations.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "bitops.h"

uint32_t reverse_bits32(uint32_t x)
{
	x = ((x >> 1) & 0x55555555) | ((x & 0x55555555) << 1);
	x = ((x >> 2) & 0x33333333) | ((x & 0x33333333) << 2);
	x = ((x >> 4) & 0x0F0F0F0F) | ((x & 0x0F0F0F0F) << 4);
	x = ((x >> 8) & 0x00FF00FF) | ((x & 0x00FF00FF) << 8);
	x = (x >> 16) | (x << 16);
	return x;
}

uint16_t reverse_bits16(uint16_t x)
{
	x = ((x >> 1) & 0x5555) | ((x & 0x5555) << 1);
	x = ((x >> 2) & 0x3333) | ((x & 0x3333) << 2);
	x = ((x >> 4) & 0x0F0F) | ((x & 0x0F0F) << 4);
	x = (x >> 8) | (x << 8);
	return x;
}

uint8_t reverse_bits8(uint8_t x)
{
	x = ((x >> 1) & 0x55) | ((x & 0x55) << 1);
	x = ((x >> 2) & 0x33) | ((x & 0x33) << 2);
	x = (x >> 4) | (x << 4);
	return x;
}

char *bit_str32(uint32_t value, char *buffer)
{
	for (int i = 31; i >= 0; --i) {
		buffer[i] = (value & 1) ? '1' : '0';
		value >>= 1;
	}

	buffer[32] = '\0';
	return buffer;
}

char *bit_str16(uint16_t value, char *buffer)
{
	for (int i = 15; i >= 0; --i) {
		buffer[i] = (value & 1) ? '1' : '0';
		value >>= 1;
	}

	buffer[16] = '\0';
	return buffer;
}

char *bit_str8(uint8_t value, char *buffer)
{
	for (int i = 7; i >= 0; --i) {
		buffer[i] = (value & 1) ? '1' : '0';
		value >>= 1;
	}

	buffer[8] = '\0';
	return buffer;
}

uint32_t str_bits32(const char *str)
{
	uint32_t result = 0;
	for (int i = 0; i < 32 && str[i] != '\0'; ++i) {
		result <<= 1;
		if (str[i] == '1') {
			result |= 1;
		} else if (str[i] != '0') {
			break;
		}
	}
	return result;
}

uint16_t str_bits16(const char *str)
{
	uint16_t result = 0;
	for (int i = 0; i < 16 && str[i] != '\0'; ++i) {
		result <<= 1;
		if (str[i] == '1') {
			result |= 1;
		} else if (str[i] != '0') {
			break;
		}
	}
	return result;
}

uint8_t str_bits8(const char *str)
{
	uint8_t result = 0;
	for (int i = 0; i < 8 && str[i] != '\0'; ++i) {
		result <<= 1;
		if (str[i] == '1') {
			result |= 1;
		} else if (str[i] != '0') {
			break;
		}
	}
	return result;
}
