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
 * DS18B20 1-Wire Temperature Sensor Driver Header File
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef DS18B20_H
#define DS18B20_H

#include "interfaces.h"
#include <stdint.h>
#include <vector>
#include <map>

#ifndef __cplusplus
#error "This header requires C++11 or higher"
#endif

class DS18B20_InterfaceBase
{
public:
	explicit DS18B20_InterfaceBase() = delete;
	virtual ~DS18B20_InterfaceBase() = default;

	/**
	 * Resolution settings
	 */
	enum Resolution {
		Res_9bit,
		Res_10bit,
		Res_11bit,
		Res_12bit,
	};

	/**
	 * Constructor of DS18B20 interface base class
	 *
	 * @param interface Reference to OneWire interface object
	 */
	explicit
	DS18B20_InterfaceBase(OneWire_InterfaceBase &interface,
						  Resolution resolution = Res_12bit) :
						  interface_(interface), resolution_(resolution) {}

	/**
	 * Initialize the sensor
	 *
	 * @return Result of the operation
	 */
	virtual OneWire_InterfaceBase::Result Init(uint8_t devices) = 0;

	/**
	 * Start temperature conversion
	 *
	 * @return Result of the operation
	 */
	virtual OneWire_InterfaceBase::Result StartConversion() = 0;

	/**
	 * Read temperature from the sensor
	 *
	 * @param temp Reference to store the read temperature
	 *
	 * @return Result of the operation
	 */
	virtual OneWire_InterfaceBase::Result ReadTemperature(float &temp, int num) = 0;

	/**
	 * Get conversion delay based on resolution
	 *
	 * @return Conversion delay in milliseconds
	 */
	virtual uint16_t GetConversionDelayMs();
protected:
	OneWire_InterfaceBase &interface_;
	Resolution resolution_;
};

/**
 * DS18B20 Interface Class for single sensor on the bus
 *
 * Only one DS18B20 sensor is present on the One-Wire bus
 */
class DS18B20_Single : public DS18B20_InterfaceBase
{
public:
	explicit DS18B20_Single() = delete;

	using DS18B20_InterfaceBase::DS18B20_InterfaceBase;

	/**
	 * Constructor of DS18B20 single sensor interface class
	 *
	 * @param interface Reference to OneWire interface object
	 * @param resolution Resolution setting
	 */
	OneWire_InterfaceBase::Result Init(uint8_t devices) override;

	/**
	 * Start temperature conversion
	 *
	 * @return Result of the operation
	 */
	OneWire_InterfaceBase::Result StartConversion() override;

	/**
	 * Read temperature from the sensor
	 *
	 * @param temp Reference to store the read temperature
	 *
	 * @return Result of the operation
	 */
	OneWire_InterfaceBase::Result ReadTemperature(float &temp, int num = 0) override;
};

/**
 * DS18B20 Interface Class for multiple sensors on the bus
 *
 * Multiple DS18B20 sensors are present on the One-Wire bus
 */
template<int max_devices = 8>
class DS18B20_Multi : public DS18B20_InterfaceBase
{
public:
	explicit DS18B20_Multi() = delete;

	using DS18B20_InterfaceBase::DS18B20_InterfaceBase;

	/**
	 * Search for DS18B20 sensors on the One-Wire bus
	 *
	 * @param devices_found Pointer to store number of found devices (optional)
	 *
	 * @return Result of the operation
	 */
	OneWire_InterfaceBase::Result SearchSensors(uint8_t *devices_found = nullptr);

	/**
	 * Initialize all sensors on the bus
	 *
	 * @param devices Number of devices to initialize (0 = all found devices)
	 *
	 * @return Result of the operation
	 */
	OneWire_InterfaceBase::Result Init(uint8_t devices = 0);

	/**
	 * Start temperature conversion on all sensors
	 *
	 * @param num Sensor number (0 to max_devices-1). If num is 0, start conversion on all sensors.
	 *
	 * @return Result of the operation
	 */
	OneWire_InterfaceBase::Result StartConversion(int num = 0);

	/**
	 * Read temperature from a specific sensor
	 *
	 * @param num Sensor number (0 to max_devices-1)
	 * @param temp Reference to store the read temperature
	 *
	 * @return Result of the operation
	 */
	OneWire_InterfaceBase::Result ReadTemperature(float &temp, int num);

	/**
	 * Read temperatures from all sensors
	 *
	 * @return Vector of read temperatures
	 */
	std::vector<float> ReadAllTemperatures();

	/**
	 * Read temperatures from all sensors
	 *
	 * @return Map of sensor index to read temperature
	 */
	std::map<uint8_t, float> ReadAllTemperaturesMap();
private:
	uint8_t serial_number_[max_devices][8];
};

#endif // DS18B20_H
