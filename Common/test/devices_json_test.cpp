#include "devices_json.h"
#include <cassert>

int do_device_json_test()
{
	/* GPIO device test */
	GPIO_DeviceJSON gpio_device("devices_test.json", 5, GPIO_DeviceBase::INPUT);
	gpio_device.Set(1);
	assert(gpio_device.Get() == 1);
	gpio_device.Step();
	assert(gpio_device.Get() == 0);
	gpio_device.Step();
	assert(gpio_device.Get() == 1);
	gpio_device.Step();
	gpio_device.Reset();
	assert(gpio_device.Get() == 1);

	/* I2C device test */
	I2C_DeviceJSON i2c_device("devices_test.json", 42);
	uint8_t read_buffer[2] = {0};
	i2c_device.Read(16, read_buffer, 1);
	assert(read_buffer[0] == 255);
	i2c_device.Step();
	i2c_device.Read(16, read_buffer, 1);
	assert(read_buffer[0] == 128);
	i2c_device.Step();
	i2c_device.Read(16, read_buffer, 1);
	assert(read_buffer[0] == 0);

	return 0;
}
