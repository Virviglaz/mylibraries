#ifndef __HASH_H__
#define __HASH_H__

#include <stdint.h>
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

#endif /* __HASH_H__ */
