#include <gtest/gtest.h>
#include <iostream>
#include "bitops.h"

int do_bitops_test()
{
    std::cout << "Starting BitOps test..." << std::endl;

    // Test reverse_bits32
    EXPECT_EQ(reverse_bits32(0x00000001), 0x80000000);
    EXPECT_EQ(reverse_bits32(0xFFFFFFFF), 0xFFFFFFFF);
    EXPECT_EQ(reverse_bits32(0x12345678), 0x1E6A2C48);

    // Test reverse_bits16
    EXPECT_EQ(reverse_bits16(0x0001), 0x8000);
    EXPECT_EQ(reverse_bits16(0xFFFF), 0xFFFF);
    EXPECT_EQ(reverse_bits16(0x1234), 0x2C48);

    // Test reverse_bits8
    EXPECT_EQ(reverse_bits8(0x01), 0x80);
    EXPECT_EQ(reverse_bits8(0xFF), 0xFF);
    EXPECT_EQ(reverse_bits8(0x12), 0x48);

    // Test bit_str32
    char buffer32[33];
    EXPECT_EQ(std::string(bit_str32(0x00000001, buffer32)), "00000000000000000000000000000001");
    EXPECT_EQ(std::string(bit_str32(0xFFFFFFFF, buffer32)), "11111111111111111111111111111111");
    EXPECT_EQ(std::string(bit_str32(0x12345678, buffer32)), "00010010001101000101011001111000");
    // Test bit_str16
    char buffer16[17];
    EXPECT_EQ(std::string(bit_str16(0x0001, buffer16)), "0000000000000001");
    EXPECT_EQ(std::string(bit_str16(0xFFFF, buffer16)), "1111111111111111");
    EXPECT_EQ(std::string(bit_str16(0x1234, buffer16)), "0001001000110100");
    // Test bit_str8
    char buffer8[9];
    EXPECT_EQ(std::string(bit_str8(0x01, buffer8)), "00000001");
    EXPECT_EQ(std::string(bit_str8(0xFF, buffer8)), "11111111");
    EXPECT_EQ(std::string(bit_str8(0x12, buffer8)), "00010010");
    std::cout << "All BitOps tests passed!" << std::endl;
    return 0;
}
