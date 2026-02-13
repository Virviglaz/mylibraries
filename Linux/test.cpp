#include <iostream>
#include <cassert>
#include "utils.h"
#include "i2c.h"

int main(int argc, char *argv[])
{
	assert(Backtrace().Print().GetFrames().size() > 0);

	try {
		I2C i2c("/dev/i2c-X");
		uint8_t data[2] = {0};
		i2c.Read(0x50, 0x00, data, sizeof(data));
		std::cout << "Read data: " << std::hex << static_cast<int>(data[0]) << " " << static_cast<int>(data[1]) << std::endl;
	} catch (const std::exception &e) {
		std::cout << "Expected error: " << e.what() << std::endl;
	}

	return 0;
}

