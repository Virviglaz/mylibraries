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

#include "devices_json.h"
#include "file_ops.h"
#include <jsoncpp/json/json.h>

GPIO_DeviceJSON::GPIO_DeviceJSON(const std::string &json_file,
								 uint16_t pin,
								 GPIO_DeviceBase::dir dir) :
								 GPIO_DeviceBase(pin, dir)
{
	Json::Value root;
	Json::Reader reader;
	reader.parse(File(json_file.c_str(), O_RDONLY).Read(), root);
	auto device_type = root["Devices"];
	for (const auto &device : device_type)
	{
		if (device["Type"].asString() == "GPIO" && device["Pin"].asUInt() == pin_)
		{
			for (const auto &state_entry : device["Steps"])
			{
				steps.push_back(state_entry["State"].asUInt());
			}
		}
	}
}

int GPIO_DeviceJSON::Get()
{
	if (step_count < steps.size())
		return steps[step_count];
	else
		throw std::out_of_range("Step count exceeds defined steps");
}

DeviceJSON_StepHandler& DeviceJSON_StepHandler::Step()
{
	step_count++;
	return *this;
}

DeviceJSON_StepHandler& DeviceJSON_StepHandler::Reset()
{
	step_count = 0;
	return *this;
}

I2C_DeviceJSON::I2C_DeviceJSON(const std::string &json_file,
							   uint8_t address) : I2C_DeviceBase(dummy_interface, address)
{
	Json::Value root;
	Json::Reader reader;
	reader.parse(File(json_file.c_str(), O_RDONLY).Read(), root);
	auto device_type = root["Devices"];
	for (const auto &device : device_type)
	{
		if (device["Type"].asString() == "I2C" && device["Address"].asUInt() == address)
		{
			for (const auto &data_entry : device["Registers"])
			{
				for (const auto &step_entry : data_entry["Steps"])
				{
					steps[address].push_back(step_entry["Data"].asUInt());
				}
			}
		}
	}
}

I2C_DeviceJSON &I2C_DeviceJSON::Read(uint8_t reg_addr,
									 uint8_t *data,
									 uint32_t length)
{
	if (steps.find(address_) == steps.end())
		throw std::out_of_range("No data defined for this device address");

	if (step_count >= steps[address_].size())
		throw std::out_of_range("Step count exceeds defined steps");

	uint16_t value = steps[address_][step_count];

	switch (length)
	{
	case 1:
		data[0] = static_cast<uint8_t>(value & 0xFF);
		break;
	case 2:
		data[0] = static_cast<uint8_t>((value >> 8) & 0xFF);
		data[1] = static_cast<uint8_t>(value & 0xFF);
		break;
	default:
		throw std::invalid_argument("Unsupported read size");
	}

	return *this;
}
