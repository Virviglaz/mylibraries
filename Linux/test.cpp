#include <iostream>
#include <cassert>
#include "utils.h"
#include "i2c.h"
#include "spi.h"

int main(int argc, char *argv[])
{
	assert(Backtrace().Print().GetFrames().size() > 0);

	try {
		I2C i2c("/dev/i2c-X");
	} catch (const std::exception &e) {
		std::cout << "Expected error: " << e.what() << std::endl;
	}

	try {
		SPI spi("/dev/spidevX.Y");
	} catch (const std::exception &e) {
		std::cout << "Expected error: " << e.what() << std::endl;
	}

	return 0;
}
