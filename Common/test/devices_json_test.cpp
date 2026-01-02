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

	/* SPI device test */
	SPI_DeviceJSON spi_device("devices_test.json", 10);
	uint8_t rx_data[4] = {0};
	spi_device.Transfer(nullptr, rx_data, 4);
	assert(rx_data[0] == 170);
	assert(rx_data[1] == 187);
	assert(rx_data[2] == 204);
	assert(rx_data[3] == 221);
	spi_device.Step();
	spi_device.Transfer(nullptr, rx_data, 4);
	assert(rx_data[0] == 221);
	assert(rx_data[1] == 238);
	assert(rx_data[2] == 255);
	assert(rx_data[3] == 16);
	spi_device.Step();

	/* UART interface test */
	UART_DeviceJSON uart_interface("devices_test.json", "UART1");
	uint8_t uart_rx[11] = {0};
	uart_interface.SendReceive(nullptr, uart_rx, 11);
	std::string uart_message(reinterpret_cast<char*>(uart_rx), 11);
	assert(uart_message == "Hello world");
	uart_interface.Step();
	uart_interface.SendReceive(nullptr, uart_rx, 11);
	uart_message = std::string(reinterpret_cast<char*>(uart_rx), 11);
	assert(uart_message == "Hello world");

	return 0;
}
