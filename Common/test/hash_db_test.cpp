#include "hash_db.h"
#include "hash.h"
#include <cassert>
#include <iostream>

int do_hash_db_test()
{
    std::cout << "Starting HashDB [hash_db.cpp] test..." << std::endl;

    HashDB db(1024, Hash("db1"));
    assert(db.Write<int>(Hash("value1"), 42) == 0);
    assert(db.Write<int>(Hash("value2"), 43) == 0);
    assert(db.Write<int>(Hash("value3"), 44) == 0);

    int read_value[3] = { 0 };
    assert(!db.Read<int>(Hash("value1"), read_value[0]));
    assert(!db.Read<int>(Hash("value2"), read_value[1]));
    assert(!db.Read<int>(Hash("value3"), read_value[2]));
    std::cout << "Read from HashDB: " << read_value[0] << std::endl;
    std::cout << "Read from HashDB: " << read_value[1] << std::endl;
    std::cout << "Read from HashDB: " << read_value[2] << std::endl;

    assert(read_value[0] == 42);
    assert(read_value[1] == 43);
    assert(read_value[2] == 44);

    assert(!db.Write<int>(Hash("value1"), 52));
    std::cout << "Used size: " << db.GetUsedSize() << " / "
              << db.GetMaxSize() << std::endl;
    assert(!db.Write<int>(Hash("value2"), 53));
    std::cout << "Used size: " << db.GetUsedSize() << " / "
              << db.GetMaxSize() << std::endl;
    assert(!db.Write<int>(Hash("value3"), 54));
    std::cout << "Used size: " << db.GetUsedSize() << " / "
              << db.GetMaxSize() << std::endl;
    assert(!db.Read<int>(Hash("value1"), read_value[0]));
    assert(!db.Read<int>(Hash("value2"), read_value[1]));
    assert(!db.Read<int>(Hash("value1"), read_value[2]));
    std::cout << "Read from HashDB: " << read_value[0] << std::endl;
    std::cout << "Read from HashDB: " << read_value[1] << std::endl;
    std::cout << "Read from HashDB: " << read_value[2] << std::endl;

    assert(read_value[0] == 52);
    assert(read_value[1] == 53);
    assert(read_value[2] == 52);

    struct somedata {
        char buf[17];
        uint8_t val;
    } __attribute__((packed));

    somedata data = { "example", 23 };
    assert(!db.Write<somedata>(Hash("data1"), data));  
    somedata read_data;
    assert(!db.Read<somedata>(Hash("data1"), read_data));
    std::cout << "Read from HashDB: " << read_data.buf << ", "
              << (uint32_t)read_data.val << std::endl;

    assert(db.Read<somedata>(Hash("data1"), read_data) == 0);
    std::cout << "Used size: " << db.GetUsedSize() << " / "
              << db.GetMaxSize() << std::endl;
    assert(db.CheckName(Hash("db1")));

    std::cout << "HashDB test completed successfully." << std::endl;

    return 0;
}
