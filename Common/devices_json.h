/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2026 Pavel Nadein
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Devices JSON configurable interface implementation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef DEVICES_JSON_H
#define DEVICES_JSON_H

#include "devices.h"
#include <string>
#include <vector>
#include <map>

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

/**
 * Helper JSON Step Handler Base Class.
 */
class DeviceJSON_StepHandler
{
public:
	/**
	 * Step function to process device actions
	 * @return Reference to the current object
	 */
	DeviceJSON_StepHandler &Step();

	/**
	 * Reset device to initial state
	 * @return Reference to the current object
	 */
	DeviceJSON_StepHandler &Reset();
protected:
	size_t step_count = 0;
};

/**
 * GPIO Device JSON Class. Implements GPIO device behavior based on JSON configuration.
 */
class GPIO_DeviceJSON : public GPIO_DeviceBase, public DeviceJSON_StepHandler
{
public:
	GPIO_DeviceJSON() = delete;

	/**
	 * Constructor
	 *
	 * @param json_file JSON configuration file path
	 * @param pin GPIO pin number
	 * @param dir Pin direction
	 */
	explicit GPIO_DeviceJSON(const std::string &json_file,
							 uint16_t port,
							 uint16_t pin,
							 GPIO_DeviceBase::dir dir);

	/**
	 * Get pin state
	 *
	 * @return Pin state
	 */
	int Get() override;
private:
	std::vector<uint16_t> steps{};
};

/**
 * I2C Device JSON Class. Implements I2C device behavior based on JSON configuration.
 */
class I2C_DeviceJSON : public I2C_DeviceBase, public DeviceJSON_StepHandler
{
public:
	I2C_DeviceJSON() = delete;

	/**
	 * Constructor
	 *
	 * @param json_file JSON configuration file path
	 * @param address I2C device address
	 */
	explicit I2C_DeviceJSON(const std::string &json_file,
							uint8_t address);

 	using I2C_DeviceBase::Write;

	I2C_DeviceJSON &Read(uint8_t reg_addr,
						 uint8_t *data,
						 uint32_t length) override;

private:
	class I2C_InterfaceDummy : public I2C_InterfaceBase {
	public:
		I2C_InterfaceDummy() {}
		int Write(uint8_t device_addr, uint8_t reg_addr,
			const uint8_t *data, uint32_t length) override { return 0; }
		int Read(uint8_t device_addr, uint8_t reg_addr,
			uint8_t *data, uint32_t length) override { return 0; }
	};

	I2C_InterfaceDummy dummy_interface;
	std::map<size_t, std::vector<uint16_t>> steps{};
};

/**
 * SPI Device JSON Class. Implements SPI device behavior based on JSON configuration.
 */
class SPI_DeviceJSON : public SPI_DeviceBase, public DeviceJSON_StepHandler
{
public:
	SPI_DeviceJSON() = delete;

	/**
	 * Constructor
	 *
	 * @param json_file JSON configuration file path
	 * @param ifs SPI interface
	 * @param cs_pin Chip select GPIO pin
	 */
	explicit SPI_DeviceJSON(const std::string &json_file,
							uint16_t cs_pin);

	SPI_DeviceJSON &Transfer(const uint8_t *tx_data,
							 uint8_t *rx_data,
							 uint32_t length) override;

private:
	class GPIO_InterfaceDummy : public GPIO_InterfaceBase {
	public:
		GPIO_InterfaceDummy() {}
		int Read(uint16_t port, uint16_t pin) override { return 0; }
		void Write(uint16_t port, uint16_t pin, int state) override {}
	};

	class SPI_InterfaceDummy : public SPI_InterfaceBase {
	public:
		SPI_InterfaceDummy() {}
		int Transfer(const uint8_t *tx_data,
			uint8_t *rx_data,
			uint32_t length) override { return 0; }
	};

	GPIO_InterfaceDummy dummy_gpio_interface;
	SPI_InterfaceDummy dummy_spi_interface;
	std::vector<std::vector<uint8_t>> steps{};
};

/**
 * UART Interface JSON Class. Implements UART interface behavior based on JSON configuration.
 */
class UART_DeviceJSON : public UART_InterfaceBase, public DeviceJSON_StepHandler
{
public:
	UART_DeviceJSON() = delete;

	/**
	 * Constructor
	 *
	 * @param json_file JSON configuration file path
	 * @param baudrate UART baudrate
	 */
	explicit UART_DeviceJSON(const std::string &json_file, const std::string &name);

	/**
	 * Send and receive data over UART
	 *
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	int SendReceive(const uint8_t *tx_data,
					uint8_t *rx_data,
					uint32_t length) override;
private:
	std::vector<std::vector<uint8_t>> steps{};
};

#endif /* DEVICES_JSON_H */
