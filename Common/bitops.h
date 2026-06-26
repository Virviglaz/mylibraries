/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2025-2026 Pavel Nadein
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
#include <cstdint>
#include <cstring>
#include <type_traits>

// --- C++20 DETECTOR AND VERSION STANDARDS ---
#if __cplusplus >= 202002L
    #include <bit>
    // In C++20, we can evaluate native endianness directly at compile-time
    constexpr bool IS_LITTLE_ENDIAN = (std::endian::native == std::endian::little);
#else
    // C++17 Fallback Macro System
    #if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
        constexpr bool IS_LITTLE_ENDIAN = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
    #elif defined(_WIN32)
        constexpr bool IS_LITTLE_ENDIAN = true; 
    #elif defined(__LITTLE_ENDIAN__) || defined(_LITTLE_ENDIAN)
        constexpr bool IS_LITTLE_ENDIAN = true;
    #else
        constexpr bool IS_LITTLE_ENDIAN = false;
    #endif
#endif

template <typename T>
constexpr T BigEndianToNative(T value)
{
    if constexpr (IS_LITTLE_ENDIAN)
    {
        // --- 16-BIT SWAP ---
        if constexpr (sizeof(T) == 2)
        {
#if __cplusplus >= 202002L
            // C++20 implementation (Safe for compile-time execution via std::bit_cast)
            std::uint16_t u16 = std::bit_cast<std::uint16_t>(value);
            #if defined(__GNUC__) || defined(__clang__)
                u16 = __builtin_bswap16(u16);
            #elif defined(_MSC_VER)
                u16 = _byteswap_ushort(u16);
            #else
                u16 = (u16 >> 8) | (u16 << 8);
            #endif
            return std::bit_cast<T>(u16);
#else
            // C++17 implementation (Uses ultra-fast runtime memcpy)
            std::uint16_t u16;
            std::memcpy(&u16, &value, 2);
            #if defined(__GNUC__) || defined(__clang__)
                u16 = __builtin_bswap16(u16);
            #elif defined(_MSC_VER)
                u16 = _byteswap_ushort(u16);
            #else
                u16 = (u16 >> 8) | (u16 << 8);
            #endif
            std::memcpy(&value, &u16, 2);
            return value;
#endif
        }
        
        // --- 32-BIT SWAP ---
        else if constexpr (sizeof(T) == 4)
        {
#if __cplusplus >= 202002L
            std::uint32_t u32 = std::bit_cast<std::uint32_t>(value);
            #if defined(__GNUC__) || defined(__clang__)
                u32 = __builtin_bswap32(u32);
            #elif defined(_MSC_VER)
                u32 = _byteswap_ulong(u32);
            #else
                u32 = ((u32 >> 24) & 0x000000FF) | ((u32 >>  8) & 0x0000FF00) |
                      ((u32 <<  8) & 0x00FF0000) | ((u32 << 24) & 0xFF000000);
            #endif
            return std::bit_cast<T>(u32);
#else
            std::uint32_t u32;
            std::memcpy(&u32, &value, 4);
            #if defined(__GNUC__) || defined(__clang__)
                u32 = __builtin_bswap32(u32);
            #elif defined(_MSC_VER)
                u32 = _byteswap_ulong(u32);
            #else
                u32 = ((u32 >> 24) & 0x000000FF) | ((u32 >>  8) & 0x0000FF00) |
                      ((u32 <<  8) & 0x00FF0000) | ((u32 << 24) & 0xFF000000);
            #endif
            std::memcpy(&value, &u32, 4);
            return value;
#endif
        }
        
        // --- 64-BIT SWAP ---
        else if constexpr (sizeof(T) == 8)
        {
#if __cplusplus >= 202002L
            std::uint64_t u64 = std::bit_cast<std::uint64_t>(value);
            #if defined(__GNUC__) || defined(__clang__)
                u64 = __builtin_bswap64(u64);
            #elif defined(_MSC_VER)
                u64 = _byteswap_uint64(u64);
            #else
                u64 = ((u64 >> 56) & 0x00000000000000FF) | ((u64 >> 40) & 0x000000000000FF00) |
                      ((u64 >> 24) & 0x0000000000FF0000) | ((u64 >>  8) & 0x00000000FF000000) |
                      ((u64 <<  8) & 0x000000FF00000000) | ((u64 << 24) & 0x0000FF0000000000) |
                      ((u64 << 40) & 0x00FF000000000000) | ((u64 << 56) & 0xFF00000000000000);
            #endif
            return std::bit_cast<T>(u64);
#else
            std::uint64_t u64;
            std::memcpy(&u64, &value, 8);
            #if defined(__GNUC__) || defined(__clang__)
                u64 = __builtin_bswap64(u64);
            #elif defined(_MSC_VER)
                u64 = _byteswap_uint64(u64);
            #else
                u64 = ((u64 >> 56) & 0x00000000000000FF) | ((u64 >> 40) & 0x000000000000FF00) |
                      ((u64 >> 24) & 0x0000000000FF0000) | ((u64 >>  8) & 0x00000000FF000000) |
                      ((u64 <<  8) & 0x000000FF00000000) | ((u64 << 24) & 0x0000FF0000000000) |
                      ((u64 << 40) & 0x00FF000000000000) | ((u64 << 56) & 0xFF00000000000000);
            #endif
            std::memcpy(&value, &u64, 8);
            return value;
#endif
        }
    }
    
    return value;
}

template <typename T>
constexpr T LittleEndianToNative(T value)
{
    // Byte swap is only required if the underlying platform is BIG ENDIAN
    if constexpr (!IS_LITTLE_ENDIAN)
    {
        // --- 16-BIT SWAP ---
        if constexpr (sizeof(T) == 2)
        {
#if __cplusplus >= 202002L
            // C++20 implementation (Safe for compile-time execution via std::bit_cast)
            std::uint16_t u16 = std::bit_cast<std::uint16_t>(value);
            #if defined(__GNUC__) || defined(__clang__)
                u16 = __builtin_bswap16(u16);
            #elif defined(_MSC_VER)
                u16 = _byteswap_ushort(u16);
            #else
                u16 = (u16 >> 8) | (u16 << 8);
            #endif
            return std::bit_cast<T>(u16);
#else
            // C++17 implementation (Uses ultra-fast runtime memcpy)
            std::uint16_t u16;
            std::memcpy(&u16, &value, 2);
            #if defined(__GNUC__) || defined(__clang__)
                u16 = __builtin_bswap16(u16);
            #elif defined(_MSC_VER)
                u16 = _byteswap_ushort(u16);
            #else
                u16 = (u16 >> 8) | (u16 << 8);
            #endif
            std::memcpy(&value, &u16, 2);
            return value;
#endif
        }
        
        // --- 32-BIT SWAP ---
        else if constexpr (sizeof(T) == 4)
        {
#if __cplusplus >= 202002L
            std::uint32_t u32 = std::bit_cast<std::uint32_t>(value);
            #if defined(__GNUC__) || defined(__clang__)
                u32 = __builtin_bswap32(u32);
            #elif defined(_MSC_VER)
                u32 = _byteswap_ulong(u32);
            #else
                u32 = ((u32 >> 24) & 0x000000FF) | ((u32 >>  8) & 0x0000FF00) |
                      ((u32 <<  8) & 0x00FF0000) | ((u32 << 24) & 0xFF000000);
            #endif
            return std::bit_cast<T>(u32);
#else
            std::uint32_t u32;
            std::memcpy(&u32, &value, 4);
            #if defined(__GNUC__) || defined(__clang__)
                u32 = __builtin_bswap32(u32);
            #elif defined(_MSC_VER)
                u32 = _byteswap_ulong(u32);
            #else
                u32 = ((u32 >> 24) & 0x000000FF) | ((u32 >>  8) & 0x0000FF00) |
                      ((u32 <<  8) & 0x00FF0000) | ((u32 << 24) & 0xFF000000);
            #endif
            std::memcpy(&value, &u32, 4);
            return value;
#endif
        }
        
        // --- 64-BIT SWAP ---
        else if constexpr (sizeof(T) == 8)
        {
#if __cplusplus >= 202002L
            std::uint64_t u64 = std::bit_cast<std::uint64_t>(value);
            #if defined(__GNUC__) || defined(__clang__)
                u64 = __builtin_bswap64(u64);
            #elif defined(_MSC_VER)
                u64 = _byteswap_uint64(u64);
            #else
                u64 = ((u64 >> 56) & 0x00000000000000FF) | ((u64 >> 40) & 0x000000000000FF00) |
                      ((u64 >> 24) & 0x0000000000FF0000) | ((u64 >>  8) & 0x00000000FF000000) |
                      ((u64 <<  8) & 0x000000FF00000000) | ((u64 << 24) & 0x0000FF0000000000) |
                      ((u64 << 40) & 0x00FF000000000000) | ((u64 << 56) & 0xFF00000000000000);
            #endif
            return std::bit_cast<T>(u64);
#else
            std::uint64_t u64;
            std::memcpy(&u64, &value, 8);
            #if defined(__GNUC__) || defined(__clang__)
                u64 = __builtin_bswap64(u64);
            #elif defined(_MSC_VER)
                u64 = _byteswap_uint64(u64);
            #else
                u64 = ((u64 >> 56) & 0x00000000000000FF) | ((u64 >> 40) & 0x000000000000FF00) |
                      ((u64 >> 24) & 0x0000000000FF0000) | ((u64 >>  8) & 0x00000000FF000000) |
                      ((u64 <<  8) & 0x000000FF00000000) | ((u64 << 24) & 0x0000FF0000000000) |
                      ((u64 << 40) & 0x00FF000000000000) | ((u64 << 56) & 0xFF00000000000000);
            #endif
            std::memcpy(&value, &u64, 8);
            return value;
#endif
        }
    }
    
    // Returns unmodified value if target is already little-endian, or size doesn't match 2/4/8
    return value;
}

template <typename T> constexpr
T NativeToBigEndian(T value)
{
    return BigEndianToNative(value);
}

template <typename T> constexpr
T NativeToLittleEndian(T value)
{
    return LittleEndianToNative(value);
}

// --- UNIFIED UNIFIED INTERFACE WRAPPERS ---

// SFINAE Constraint helper: Ensures we only convert trivially copyable standard layouts (ints, floats, simple structs)
#if __cplusplus >= 202002L
template<typename T>
concept TrivialType = std::is_trivially_copyable_v<T>;
#else
template<typename T>
using TrivialType = typename std::enable_if_t<std::is_trivially_copyable<T>::value, bool>;
#endif

enum class Endian {
    Little,
    Big
};

// Master Target Function
template <Endian TargetLayout, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
constexpr T NativeTo(T value) {
    if constexpr (TargetLayout == Endian::Big) {
        return BigEndianToNative(value); // Transposition logic is symmetric
    } else {
        return LittleEndianToNative(value);
    }
}

// Master Reverse Function (Optional counterpart for reading data streams)
template <Endian SourceLayout, typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
constexpr T ToNative(T value) {
    if constexpr (SourceLayout == Endian::Big) {
        return BigEndianToNative(value);
    } else {
        return LittleEndianToNative(value);
    }
}

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