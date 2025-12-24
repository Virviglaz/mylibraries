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

#include "rand.h"

/** Tiny Mersenne Twister (TinyMT) state structure. */
static uint32_t state[4];

void tinymt32_init(uint32_t seed)
{
	state[0] = seed;
	state[1] = 0x8f7011ee;
	state[2] = 0xfc78ff1f;
	state[3] = 0x3793fdff;

	for (int i = 1; i < 8; i++)
	{
		state[i & 3] ^= i + 1812433253 * \
			(state[(i - 1) & 3] ^ (state[(i - 1) & 3] >> 30));
	}
}

uint32_t tinymt32_generate(void)
{
	uint32_t x = (state[0] & 0x7fffffff) ^ state[1] ^ state[2];
	x ^= x << 1;
	state[0] = state[1];
	state[1] = state[2];
	state[2] = state[3] ^ (x >> 1);
	state[3] = x;
	return state[3];
}

uint8_t micro_rand(uint8_t *state)
{
	*state = 29 * (*state) + 217;
	return *state;
}

uint32_t mulberry32(uint32_t *state)
{
	uint32_t z = (*state += 0x6D2B79F5);
	z = (z ^ (z >> 15)) * (z | 1);
	z ^= z + (z ^ (z >> 7)) * (z | 1);
	return z ^ (z >> 14);
}

uint64_t ranq1(uint64_t *state)
{
	*state ^= *state >> 21;
	*state ^= *state << 35;
	*state ^= *state >> 4;
	return *state * 2685821657736338717ULL;
}
