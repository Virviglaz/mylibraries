#include "file_ops.h"
#include <cstring>
#include <iostream>

int do_file_ops_test()
{
    File f("test.txt");

    char str[] = "Test string\n";
    f.Write(static_cast<void *>(str), sizeof(str));

    char rd[sizeof(str)];
    f.Seek(0, File::SeekAt::SET);
    f.Read(static_cast<void *>(rd), sizeof(rd));

    std::cout << "File size: " << f.GetStats().GetSize() << std::endl;
    std::cout << "File contents:\n";
    std::cout << f;

    return strcmp(str, rd);
}
