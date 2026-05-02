#include "file_ops.h"
#include <cstring>
#include <iostream>
#include <assert.h>

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

int do_csv_file_test()
{
	CSVFile csv("test.csv");

	std::cout << "CSV file header:\n";
	std::cout << csv.ReadLine() << std::endl;

	assert(csv.GetLineCount() == 5);

	auto csvData = csv.Parse();
	assert(csvData.size() == 5);

	for (const auto &line : csvData) {
		for (const auto &value : line) {
			std::visit([](const auto &v) { std::cout << v << " "; }, value.Get());
		}
		std::cout << std::endl;
	}

	return 0;
}
