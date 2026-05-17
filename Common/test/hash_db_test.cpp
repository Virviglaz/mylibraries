#include "hash_db.h"
#include "hash.h"
#include <gtest/gtest.h>

int do_hash_db_test()
{
    HashDB db(1024, Hash("db1"));
    EXPECT_EQ(db.Write<int>("value1", 42), 0);
    EXPECT_EQ(db.Write<int>("value2", 43), 0);
    EXPECT_EQ(db.Write<int>("value3", 44), 0);

    int read_value[3] = { 0 };
    EXPECT_EQ(db.Read<int>("value1", read_value[0]), 0);
    EXPECT_EQ(db.Read<int>("value2", read_value[1]), 0);
    EXPECT_EQ(db.Read<int>("value3", read_value[2]), 0);

    EXPECT_EQ(read_value[0], 42);
    EXPECT_EQ(read_value[1], 43);
    EXPECT_EQ(read_value[2], 44);

    EXPECT_EQ(db.Write<int>("value1", 52), 0);
    EXPECT_EQ(db.Write<int>("value2", 53), 0);
    EXPECT_EQ(db.Write<int>("value3", 54), 0);
    EXPECT_EQ(db.Read<int>("value1", read_value[0]), 0);
    EXPECT_EQ(db.Read<int>("value2", read_value[1]), 0);
    EXPECT_EQ(db.Read<int>("value1", read_value[2]), 0);

    EXPECT_EQ(read_value[0], 52);
    EXPECT_EQ(read_value[1], 53);
    EXPECT_EQ(read_value[2], 52);

    struct somedata {
        char buf[17];
        uint8_t val;
    } __attribute__((packed));

    somedata data = { "example", 23 };
    EXPECT_EQ(db.Write<somedata>("data1", data), 0);
    somedata read_data;
    EXPECT_EQ(db.Read<somedata>("data1", read_data), 0);

    EXPECT_EQ(db.Read<somedata>("data1", read_data), 0);
    EXPECT_TRUE(db.CheckName("db1"));

    return 0;
}
