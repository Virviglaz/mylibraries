#include "state_machine_test.h"
#include "arg_parser_test.h"
#include <cassert>

int main()
{
    extern int do_hash_db_test();
    extern int do_bitops_test();
    extern int do_state_machine_test();
    extern int do_filters_test();
    extern int do_file_ops_test();
    extern int do_device_json_test();
    extern int DS18B20_json_test();
	extern int do_HD44780_ToFile_test();

    assert(do_hash_db_test() == 0);
    assert(do_bitops_test() == 0);
    assert(do_state_machine_test() == 0);
    assert(do_filters_test() == 0);
    assert(do_state_machine_c_test() == 0);
    assert(do_arg_parse_test() == 0);
    assert(do_file_ops_test() == 0);
    assert(do_device_json_test() == 0);
    assert(DS18B20_json_test() == 0);
	assert(do_HD44780_ToFile_test() == 0);

    return 0;
}
