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
 * DS18B20 1-Wire Temperature Sensor Driver Source File
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "crc.h"
#include "DS18B20.h"

/* Commands list */
#define READ_ROM 0x33
#define SKIP_ROM 0xCC
#define SEARCH_ROM 0xF0
#define MATCH_ROM 0x55

#define CONVERT_T_CMD 0x44
#define WRITE_STRATCHPAD_CMD 0x4E
#define READ_STRATCHPAD_CMD 0xBE
#define COPY_STRATCHPAD_CMD 0x48
#define RECALL_E_CMD 0xB8
#define READ_POWER_SUPPLY_CMD 0xB4

#define STRATCHPAD_SIZE 0x09

uint16_t DS18B20_InterfaceBase::GetConversionDelayMs()
{
	const uint16_t delays[] = {100, 200, 400, 800};
	return delays[static_cast<uint8_t>(resolution_)];
}

OneWire_InterfaceBase::Result DS18B20_Single::Init(uint8_t devices)
{
	auto res = interface_.Reset();
	if (res != OneWire_InterfaceBase::Success)
		return res;

	interface_.Write(SKIP_ROM);
	interface_.Write(WRITE_STRATCHPAD_CMD);
	interface_.Write(0); // Th
	interface_.Write(0); // Tl
	interface_.Write(static_cast<uint8_t>(resolution_) << 5 | 0x1F);

	return res;
}

OneWire_InterfaceBase::Result DS18B20_Single::StartConversion()
{
	auto res = interface_.Reset();
	if (res != OneWire_InterfaceBase::Success)
		return res;

	/* Start conversion skipping ROM */
	interface_.Write(SKIP_ROM);
	interface_.Write(CONVERT_T_CMD);

	return res;
}

OneWire_InterfaceBase::Result DS18B20_Single::ReadTemperature(float &temp, int num)
{
	uint8_t buf[STRATCHPAD_SIZE];
	int16_t rawvalue;

	auto res = interface_.Reset();
	if (res != OneWire_InterfaceBase::Success)
		return res;

	interface_.Write(SKIP_ROM);
	interface_.Write(READ_STRATCHPAD_CMD);

	for (uint8_t res = 0; res != STRATCHPAD_SIZE; res++)
		buf[res] = interface_.Read();

	if (crc8Dallas(buf, sizeof(buf)))
		return OneWire_InterfaceBase::Result::CrcMissmatch;

	rawvalue = buf[0] | (buf[1] << 8);
	if (rawvalue == 2495)
		return OneWire_InterfaceBase::Result::NotReady;

	temp = static_cast<float>(rawvalue) / 16.0f;

	return OneWire_InterfaceBase::Success;
}

template<int max_devices>
OneWire_InterfaceBase::Result DS18B20_Multi<max_devices>::SearchSensors(uint8_t *devices_found)
{
	unsigned long path, next, pos;	/* decision markers */
	uint8_t bit, chk;				/* bit values */
	unsigned char cnt_num, tmp;
	path = 0;						/* initial path to follow */
	cnt_num = 0;
	do
	{
		/* 
		 * Each ROM search pass
		 */
		auto res = interface_.Reset();
		if (res != OneWire_InterfaceBase::Success)
			return res;

		interface_.Write(SEARCH_ROM);
		next = 0;	/* next path to follow */
		pos = 1;	/* path bit pointer */
		for (uint8_t cnt_byte = 0; cnt_byte != 8; cnt_byte++)
		{
			serial_number_[cnt_num][cnt_byte] = 0;
			for (uint8_t cnt_bit = 0; cnt_bit != 8; cnt_bit++)
			{
				/* 
				 * Each bit of the ROM value
				 * (read two bits, 'bit' and 'chk', from the 1-wire bus)
				 */
				bit = interface_.ReadBit();
				chk = interface_.ReadBit();
				if (!bit && !chk)
				{
					/* collision, both are zero */
					if (pos & path)
						bit = 1; /* if we've been here before */
					else
						next = (path & (pos - 1)) | pos; /* else, new branch for next */
					pos <<= 1;
				}

				interface_.WriteBit(bit);

				//(save this bit as part of the current ROM value)
				if (bit != 0)
					serial_number_[cnt_num][cnt_byte] |= (1 << cnt_bit);
			}
		}
		//(output the just-completed ROM value)
		path = next;
		cnt_num++;
		if (cnt_num == max_devices)
			break;
	} while (path);

	if (devices_found)
		*devices_found = cnt_num;

	return *devices_found == max_devices ? \
		OneWire_InterfaceBase::Success : OneWire_InterfaceBase::NotAllDevicesFound;
}

template<int max_devices>
OneWire_InterfaceBase::Result DS18B20_Multi<max_devices>::Init(uint8_t devices)
{
	OneWire_InterfaceBase::Result res;

	for (uint8_t i = 0; i != devices ? devices : max_devices; i++)
	{
		res = interface_.Reset();
		if (res != OneWire_InterfaceBase::Success)
			return res;

		interface_.Write(MATCH_ROM);
		for (uint8_t j = 0; j != 8; j++)
			interface_.Write(serial_number_[i][j]);

		interface_.Write(WRITE_STRATCHPAD_CMD);
		interface_.Write(0); // Th
		interface_.Write(0); // Tl
		interface_.Write(static_cast<uint8_t>(resolution_) << 5 | 0x1F);
	}

	return res;
}

template<int max_devices>
OneWire_InterfaceBase::Result DS18B20_Multi<max_devices>::StartConversion(int num)
{
	OneWire_InterfaceBase::Result res;

	if (num == 0)
	{
		// Start conversion on all sensors
		res = interface_.Reset();
		if (res != OneWire_InterfaceBase::Success)
			return res;

		interface_.Write(SKIP_ROM);
		interface_.Write(CONVERT_T_CMD);
	}
	else
	{
		// Start conversion on a single sensor
		res = interface_.Reset();
		if (res != OneWire_InterfaceBase::Success)
			return res;

		interface_.Write(MATCH_ROM);
		for (uint8_t i = 0; i != 8; i++)
			interface_.Write(serial_number_[num - 1][i]);
		interface_.Write(CONVERT_T_CMD);
	}

	return res;
}

template<int max_devices>
OneWire_InterfaceBase::Result DS18B20_Multi<max_devices>::ReadTemperature(float &temp, int num)
{
	uint8_t buf[STRATCHPAD_SIZE];
	int16_t rawvalue;

	auto res = interface_.Reset();
	if (res != OneWire_InterfaceBase::Success)
		return res;

	interface_.Write(MATCH_ROM);
	for (uint8_t i = 0; i != 8; i++)
		interface_.Write(serial_number_[num][i]);

	interface_.Write(READ_STRATCHPAD_CMD);

	for (uint8_t res = 0; res != STRATCHPAD_SIZE; res++)
		buf[res] = interface_.Read();

	if (crc8Dallas(buf, sizeof(buf)))
		return OneWire_InterfaceBase::Result::CrcMissmatch;

	rawvalue = buf[0] | (buf[1] << 8);
	if (rawvalue == 2495)
		return OneWire_InterfaceBase::Result::NotReady;

	temp = static_cast<float>(rawvalue) / 16.0f;

	return OneWire_InterfaceBase::Success;
}

template<int max_devices>
std::vector<float> DS18B20_Multi<max_devices>::ReadAllTemperatures()
{
	std::vector<float> temperatures;
	float temp;

	for (uint8_t i = 0; i != max_devices; i++)
	{
		if (ReadTemperature(i, temp) == OneWire_InterfaceBase::Success)
			temperatures.push_back(temp);
	}

	return temperatures;
}

template<int max_devices>
std::map<uint8_t, float> DS18B20_Multi<max_devices>::ReadAllTemperaturesMap()
{
	std::map<uint8_t, float> temperatures;
	float temp;

	for (uint8_t i = 0; i != max_devices; i++)
	{
		if (ReadTemperature(i, temp) == OneWire_InterfaceBase::Success)
			temperatures[i] = temp;
	}

	return temperatures;
}
