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
 * Hash number gerenator.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __HASH_H__
#define __HASH_H__

#include <stdint.h>

#ifdef __cplusplus
#include <stddef.h>

struct CstrView
{
	template <size_t N>
	constexpr CstrView(const char (&a_Str)[N]) : str(a_Str), sz(N - 1) {}
	constexpr size_t size() { return sz; }
	constexpr uint32_t getBlock32(size_t a_Index)
	{
		size_t i = a_Index * sizeof(uint32_t);
		uint32_t l_B0 = str[i];
		uint32_t l_B1 = str[i + 1];
		uint32_t l_B2 = str[i + 2];
		uint32_t l_B3 = str[i + 3];
		return (l_B3 << 24) | (l_B2 << 16) | (l_B1 << 8) | l_B0;
	}
	constexpr size_t blockSize32() { return (size() / sizeof(uint32_t)); }
	constexpr char tail32(size_t a_Index)
	{
		size_t l_TailSize = size() % sizeof(uint32_t);
		return str[size() - l_TailSize + a_Index];
	}

	const char *str;
	size_t sz;
};

constexpr size_t Hash(CstrView a_Key)
{
	const uint32_t l_M = 0x5bd1e995;
	const int l_R = 24;

	const size_t l_Len = a_Key.size();
	uint32_t l_H = 0 ^ l_Len;

	const size_t l_NofBlocks = a_Key.blockSize32();
	for (size_t i = 0; i < l_NofBlocks; ++i)
	{
		uint32_t l_K = a_Key.getBlock32(i);

		l_K *= l_M;
		l_K ^= l_K >> l_R;
		l_K *= l_M;

		l_H *= l_M;
		l_H ^= l_K;
	}

	switch (l_Len & (sizeof(uint32_t) - 1))
	{
	case 3:
		l_H ^= (uint32_t)(a_Key.tail32(2)) << 16;
	/* FALLTHROUGH */
	case 2:
		l_H ^= (uint32_t)(a_Key.tail32(1)) << 8;
	/* FALLTHROUGH */
	case 1:
		l_H ^= (uint32_t)(a_Key.tail32(0));
		l_H *= l_M;
		/* FALLTHROUGH */
	};

	l_H ^= l_H >> 13;
	l_H *= l_M;
	l_H ^= l_H >> 15;

	return l_H;
}

#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

	/**
	 * Compute FNV-1a hash for given data.
	 *
	 * @param data		Pointer to data.
	 * @param len		Length of data in bytes.
	 *
	 * @return Computed hash value.
	 */
	uint32_t fnv1a_hash(const void *data, uint32_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __HASH_H__ */
