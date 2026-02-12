#include <iostream>
#include <cassert>
#include "utils.h"

int main(int argc, char *argv[])
{
	assert(Backtrace().Print().GetFrames().size() > 0);

	return 0;
}

