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

#ifndef BITOPS_H
#define BITOPS_H

#include <stdint.h>

#if !defined(BIT)
#define BIT(x) (1U << (x))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/** Reverses the bits in a 32-bit unsigned integer. */
uint32_t reverse_bits32(uint32_t x);

/** Reverses the bits in a 16-bit unsigned integer. */
uint16_t reverse_bits16(uint16_t x);

/** Reverses the bits in an 8-bit unsigned integer. */
uint8_t reverse_bits8(uint8_t x);

/** Converts a 32-bit unsigned integer to its bit string representation. */
char *bit_str32(uint32_t value, char *buffer);

/** Converts a 16-bit unsigned integer to its bit string representation. */
char *bit_str16(uint16_t value, char *buffer);

/** Converts an 8-bit unsigned integer to its bit string representation. */
char *bit_str8(uint8_t value, char *buffer);

/** Converts a bit string to a 8-bit unsigned integer. */
uint32_t str_bits32(const char *str);

/** Converts a bit string to a 16-bit unsigned integer. */
uint16_t str_bits16(const char *str);

/** Converts a bit string to an 8-bit unsigned integer. */
uint8_t str_bits8(const char *str);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* BITOPS_H */