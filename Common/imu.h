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
 * IMU class header file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef IMU_H
#define IMU_H

#include <type_traits>
#include <cstdint>

namespace IMU
{
	/**
	 * @brief Class representing raw data from IMU sensors.
	 */
	class RawData16_XYZ
	{
	public:
		RawData16_XYZ() = default;
		RawData16_XYZ(int16_t x, int16_t y, int16_t z, float scale)
			: x(x), y(y), z(z), scale(scale) {}

		int16_t GetRawX() const { return x; }
		int16_t GetRawY() const { return y; }
		int16_t GetRawZ() const { return z; }

		float GetX() const { return static_cast<float>(x) / scale; }
		float GetY() const { return static_cast<float>(y) / scale; }
		float GetZ() const { return static_cast<float>(z) / scale; }
	protected:
		int16_t x, y, z;
		float scale;
	};

	/**
	 * @brief Base class representing combined (acc + gyro) raw data from IMU sensors.
	 *
	 * GetTemperature() method is not implemented in this base class and should be implemented in derived classes.
	 */
	class RawData_6DOF_Base
	{
	public:
		RawData_6DOF_Base() = default;
		RawData_6DOF_Base(RawData16_XYZ &&accel, RawData16_XYZ &&gyro, float temp)
			: Accel(accel), Gyro(gyro), temperature(temp) {}
		RawData16_XYZ Accel;
		RawData16_XYZ Gyro;
		virtual float GetTemperature() const = 0;
	protected:
		float temperature;
	};

	/**
	 * @brief Class representing real data (in degrees and radians) from IMU sensors
	 * calculated from quaternions, provided by the IMU DMP (Digital Motion Processor).
	 *
	 * @tparam T Floating-point type (float or double) for the real data.
	 */
	template <class T>
	class ImuRealData
	{
	public:
		ImuRealData() = default;
		ImuRealData(T x, T y, T z, T w);

		T GetRollDeg() const { return roll; }
		T GetPitchDeg() const { return pitch; }
		T GetYawDeg() const { return yaw; }

		T GetRollRad() const { return roll * pi / 180.0; }
		T GetPitchRad() const { return pitch * pi / 180.0; }
		T GetYawRad() const { return yaw * pi / 180.0; }
	protected:
		T pi = 3.14159265358979323846;
		T roll, pitch, yaw;
		static_assert(std::is_floating_point<T>::value,
				"ImuData template parameter T must be a floating-point type");
	};

	class IMU_Position
	{
	public:
		explicit IMU_Position() = default;

		struct Vector3D {
		    float x = 0.0f;
		    float y = 0.0f;
		    float z = 0.0f;
		};

		template <class T>
		IMU_Position& Update(const RawData_6DOF_Base &raw_data,
				const ImuRealData<T> &real_data, T dt);

		Vector3D getPosition() const { return position; }
		Vector3D getVelocity() const { return velocity; }

		IMU_Position& Reset();
	protected:
		const float GRAVITY = 9.80665f; // м/с^2
		Vector3D velocity;
		Vector3D position;
	};
}

#endif // IMU_H
