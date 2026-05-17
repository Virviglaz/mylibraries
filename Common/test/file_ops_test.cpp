#include "file_ops.h"
#include <cstring>
#include <iostream>
#include <gtest/gtest.h>

int do_file_ops_test()
{
    File f("test.txt");

    char str[] = "Test string\n";
    f.Write(static_cast<void *>(str), sizeof(str));

    char rd[sizeof(str)];
    f.Seek(0, File::SeekAt::SET);
    f.Read(static_cast<void *>(rd), sizeof(rd));
	EXPECT_EQ(f.GetStats().GetSize(), 13);

    return strcmp(str, rd);
}

int do_csv_file_test()
{
	CSVFile csv("test.csv");

	EXPECT_EQ(csv.GetLineCount(), 5);

	auto csvData = csv.Parse();
	EXPECT_EQ(csvData.size(), 5);

	for (const auto &line : csvData) {
		for (const auto &value : line) {
			std::visit([](const auto &v) { std::cout << v << " "; }, value.Get());
		}
		std::cout << std::endl;
	}

	return 0;
}

int do_ext_file_test()
{
	CSVFile csv;

	try {
		csv.Parse();
	} catch (const std::runtime_error &e) {
		EXPECT_STREQ(e.what(), "File is not open");
	}

	try {
		csv.Open("nonexistent.csv", File::READ_ONLY);
	} catch (const std::system_error &e) {
		EXPECT_EQ(e.code().value(), ENOENT);
	}

	try {
		csv.Close();
	} catch (const std::runtime_error &e) {
		EXPECT_STREQ(e.what(), "File is not open");
	}

	try {
		csv.Seek(0, File::SeekAt::SET);
	} catch (const std::runtime_error &e) {
		EXPECT_STREQ(e.what(), "File is not open");
	}

	csv.Open("test.csv", File::READ_ONLY);

	try {
		csv.Write("data", 4);
	} catch (const std::runtime_error &e) {
		EXPECT_STREQ(e.what(), "File is not open for writing");
	}

	csv.Close();

	return 0;
}