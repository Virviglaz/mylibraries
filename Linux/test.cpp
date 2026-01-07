#include <iostream>
#include <cassert>
#include "utils.h"

int main(int argc, char *argv[])
{
	auto backtrace = get_backtrace();
	assert(!backtrace.empty());
	for (const auto &frame : backtrace) {
		std::cout << frame << std::endl;
	}

	return 0;
}

