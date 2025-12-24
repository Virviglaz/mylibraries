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
 * Pseudo-random number gerenator.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <stdint.h>

#ifndef RAND_H
#define RAND_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Initialize TinyMT state with a given seed.
 *
 * @param seed		Seed value.
 */
void tinymt32_init(uint32_t seed);

/**
 * Generate a pseudo-random number using TinyMT.
 *
 * @return Generated pseudo-random number.
 */
uint32_t tinymt32_generate(void);

/**
 * Simple linear congruential generator for microcontrollers.
 *
 * @param state		Pointer to the generator state variable.
 * @return Generated pseudo-random byte.
 */
uint8_t micro_rand(uint8_t *state);

/**
 * Mulberry32 pseudo-random number generator.
 *
 * @param state		Pointer to the generator state variable.
 * @return Generated pseudo-random number.
 */
uint32_t mulberry32(uint32_t *state);

/**
 * 64-bit random number generator by David Stafford.
 *
 * @param state		Pointer to the generator state variable.
 * @return Generated pseudo-random number.
 */
uint64_t ranq1(uint64_t *state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RAND_H */