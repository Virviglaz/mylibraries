#include "arg_parser.h"
#include "arg_parser_test.h"
#include <errno.h>

static int err = EINVAL;

static int test_cmd(int argc, char *argv[])
{
	if (argc == 2)
		err = 0;
	return 0;
}

static arg_list_t arg_list[] = {
	{"test_cmd", test_cmd},
};

int do_arg_parse_test()
{
	char arg_str[] = "test_cmd arg1 arg2";
	int ret = ARG_PARSE(arg_str, arg_list);

	return ret ? ret : err;
}