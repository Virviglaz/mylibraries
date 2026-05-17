#include "state_machine_test.h"
#include "arg_parser_test.h"
#include <gtest/gtest.h>

extern int do_hash_db_test();
extern int do_bitops_test();
extern int do_state_machine_test();
extern int do_filters_test();
extern int do_file_ops_test();
extern int do_csv_file_test();
extern int do_ext_file_test();
extern int do_device_json_test();
extern int DS18B20_json_test();
extern int do_HD44780_ToFile_test();
extern int do_singleton_test();

TEST(hash_db_test, TestHashDB) {
    EXPECT_EQ(do_hash_db_test(), 0);
}

TEST(bitops_test, TestBitOperations) {
    EXPECT_EQ(do_bitops_test(), 0);
}

TEST(state_machine_test, TestStateMachine) {
    EXPECT_EQ(do_state_machine_test(), 0);
}

TEST(filters_test, TestDigitalFilters) {
    EXPECT_EQ(do_filters_test(), 0);
}

TEST(do_state_machine_c_test, TestStateMachineC) {
    EXPECT_EQ(do_state_machine_c_test(), 0);
}

TEST(arg_parser_test, TestArgumentsParser) {
    EXPECT_EQ(do_arg_parse_test(), 0);
}

TEST(file_ops_test, TestFileOperations) {
    EXPECT_EQ(do_file_ops_test(), 0);
}

TEST(csv_file_test, TestCSVFileReader) {
	EXPECT_EQ(do_csv_file_test(), 0);
}

TEST(ext_file_test, TestExtendedFileOperations) {
	EXPECT_EQ(do_ext_file_test(), 0);
}

TEST(device_json_test, TestDeviceJsonTestFramework) {
    EXPECT_EQ(do_device_json_test(), 0);
}

TEST(DS18B20_json_test, TestDS18B20JsonSimulatedDevice) {
    EXPECT_EQ(DS18B20_json_test(), 0);
}

TEST(HD44780_ToFile_test, TestHD44780ToFile) {
    EXPECT_EQ(do_HD44780_ToFile_test(), 0);
}

TEST(singleton_test, TestSingleton) {
    EXPECT_EQ(do_singleton_test(), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
