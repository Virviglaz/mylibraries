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
	 * @brief Class representing 16-bit signed raw data from IMU sensors (MPU6x50/ICM).
	 *
	 * @todo Add support for 32-bit raw data from IMU sensors.
	 * @todo Add support for double precision raw data from IMU sensors.
	 */
	class RawData16_XYZ
	{
	public:
		explicit RawData16_XYZ() = default;
		virtual ~RawData16_XYZ() = default;
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
		explicit RawData_6DOF_Base() = default;
		virtual ~RawData_6DOF_Base() = default;
		RawData_6DOF_Base(RawData16_XYZ accel, RawData16_XYZ gyro, float temp)
			: Accel(accel), Gyro(gyro), temperature(temp) {}
		RawData16_XYZ Accel;
		RawData16_XYZ Gyro;
		virtual float GetTemperature() const = 0;

	protected:
		float temperature;
	};

	/**
	 * @brief Helper class representing 3D vector data (x, y, z) with floating point values.
	 *
	 * @tparam T Floating-point type (float or double) for the real data.
	 */
	template <class T>
	struct Vector3D
	{
		static_assert(std::is_floating_point<T>::value, "Vector3D type must be a floating point type.");

		Vector3D(T x, T y, T z) : x(x), y(y), z(z) {}
		explicit Vector3D() : x(static_cast<T>(0.0)), y(static_cast<T>(0.0)), z(static_cast<T>(0.0)) {}
		virtual ~Vector3D() = default;

		T x, y, z;
	};

	/**
	 * @brief Helper class representing quaternion data (x, y, z, w) with floating point values.
	 *
	 * @tparam T Floating-point type (float or double) for the real data.
	 */
	template <class T>
	struct Quaternion
	{
		static_assert(std::is_floating_point<T>::value, "Quaternion type must be a floating point type.");

		Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
		explicit Quaternion() : x(static_cast<T>(1.0)), y(static_cast<T>(0.0)), z(static_cast<T>(0.0)), w(static_cast<T>(0.0)) {}
		virtual ~Quaternion() = default;

		T x, y, z, w;
	};

	/**
	 * @brief Class representing Eulers angles data (in degrees and radians) from IMU sensors
	 * calculated from quaternions, provided by the IMU DMP (Digital Motion Processor).
	 *
	 * @note Calculations requires complex math operations! Use it only when you need to get Euler angles from quaternions.
	 * Otherwise, use quaternions directly.
	 *
	 * @tparam T Floating-point type (float or double) for the real data.
	 */
	template <class T>
	class EulerAngles
	{
		static_assert(std::is_floating_point<T>::value,
					  "EulerAngles template parameter T must be a floating-point type");

	public:
		EulerAngles() = default;

		/**
		 * @brief Construct Euler angles from quaternion components.
		 */
		EulerAngles(T x, T y, T z, T w);

		/**
		 * @brief Construct Euler angles from a quaternion object.
		 */
		EulerAngles(Quaternion<T> q) : EulerAngles(q.x, q.y, q.z, q.w) {}

		/**
		 * @brief Get the roll angle in degrees.
		 */
		T GetRollDeg() const
		{
			return roll * rad2deg;
		}

		/**
		 * @brief Get the pitch angle in degrees.
		 */
		T GetPitchDeg() const
		{
			return pitch * rad2deg;
		}

		/**
		 * @brief Get the yaw angle in degrees.
		 */
		T GetYawDeg() const
		{
			return yaw * rad2deg;
		}

		/**
		 * @brief Get the roll angle in radians.
		 */
		T GetRollRad() const
		{
			return roll;
		}

		/**
		 * @brief Get the pitch angle in radians.
		 */
		T GetPitchRad() const
		{
			return pitch;
		}

		/**
		 * @brief Get the yaw angle in radians.
		 */
		T GetYawRad() const
		{
			return yaw;
		}

	private:
		static constexpr T pi = 3.14159265358979323846;
		static constexpr T rad2deg = static_cast<T>(180.0) / pi;
		T roll = static_cast<T>(0.0);
		T pitch = static_cast<T>(0.0);
		T yaw = static_cast<T>(0.0);
	};

	/**
	 * @brief Class representing linear acceleration data (in m/s^2) from IMU sensors
	 * calculated from raw acceleration data and orientation (quaternion) provided by the IMU DMP (Digital Motion Processor).
	 *
	 * @tparam T Floating-point type (float or double) for the real data.
	 */
	template <class T>
	class Acceleration
	{
		static_assert(std::is_floating_point<T>::value,
					  "Acceleration template parameter T must be a floating-point type");

	public:
		Acceleration() = default;

		/**
		 * @brief Construct Acceleration from raw data and quaternion components.
		 */
		Acceleration(const RawData_6DOF_Base &raw_data, T qx, T qy, T qz, T qw);

		/**
		 * @brief Construct Acceleration from raw data and a quaternion object.
		 */
		Acceleration(const RawData_6DOF_Base &raw_data, Quaternion<T> q) : Acceleration(raw_data, q.x, q.y, q.z, q.w) {}

		/**
		 * @brief Construct Acceleration from rvalue reference of raw data and quaternion components.
		 */
		Acceleration(RawData_6DOF_Base &&raw_data, Quaternion<T> q) : Acceleration(raw_data, q.x, q.y, q.z, q.w) {}

		/**
		 * @brief Get the X component of the acceleration in m/s^2.
		 */
		T GetX() const { return x; }

		/**
		 * @brief Get the Y component of the acceleration in m/s^2.
		 */
		T GetY() const { return y; }

		/**
		 * @brief Get the Z component of the acceleration in m/s^2.
		 */
		T GetZ() const { return z; }

		static constexpr T GRAVITY = static_cast<T>(9.80665); // м/с^2
	private:
		T x = static_cast<T>(0.0);
		T y = static_cast<T>(0.0);
		T z = static_cast<T>(0.0);
	};

	/**
	 * @brief Class holding actual position in 3D space, calculated from acceleration data.
	 *
	 * @tparam T Floating-point type (float or double) for the real data.
	 */
	template <class T>
	class Position
	{
		static_assert(std::is_floating_point<T>::value,
					  "Position template parameter T must be a floating-point type");

	public:
		explicit Position() = default;

		/**
		 * @brief Update the position based on the provided Euler angles and acceleration.
		 *
		 * @note Calculations requires complex math operations! Use it only when you need to get position from acceleration.
		 * Otherwise, use acceleration directly.
		 *
		 * @param raw_data	Raw linear acceleration data from the IMU.
		 * @param euler_angles Euler angles data (in radians) from the IMU.
		 * @param dt The time interval (in seconds) since the last update.
		 */
		Position &Update(const RawData_6DOF_Base &raw_data,
						 EulerAngles<T> euler_angles, T dt);

		/**
		 * @brief Update the position based on the provided acceleration and time interval.
		 *
		 * @param acceleration Real linear acceleration data from the IMU.
		 * @param dt The time interval (in seconds) since the last update.
		 * @param stationary_threshold The threshold for determining if the device is stationary (in m/s^2).
		 * If the magnitude of the linear acceleration is below this threshold,
		 * the device is considered stationary and a velocity decay is applied to reduce drift.
		 * TODO: Consider making this threshold configurable or adaptive based on the application.
		 *
		 * @return A reference to the updated Position object.
		 */
		Position &Update(const Acceleration<T> &acceleration, T dt, T stationary_threshold = static_cast<T>(0.15));

		/**
		 * @brief Get the current position in 3D space.
		 *
		 * @return A Vector3D<T> representing the current position (x, y, z).
		 */
		Vector3D<T> getPosition() const { return position; }

		/**
		 * @brief Get the current velocity in 3D space.
		 *
		 * @return A Vector3D<T> representing the current velocity (x, y, z).
		 */
		Vector3D<T> getVelocity() const { return velocity; }

		/**
		 * @brief Reset the position and velocity to zero.
		 */
		Position &Reset();

	protected:
		Vector3D<T> velocity;
		Vector3D<T> position;
	};

	template <typename T>
	class MadgwickFilter
	{
		static_assert(std::is_floating_point<T>::value, "Filter type must be a floating point type.");

	public:
		// beta - filter coefficient. Usually from 0.01 to 0.1
		explicit MadgwickFilter(T beta = static_cast<T>(0.1))
			: m_beta(beta), m_q(static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0)) {}

		// Reset the orientation to the initial state
		void Reset()
		{
			m_q = Quaternion<T>();
		}

		// Set the beta coefficient manually
		void SetBeta(T beta) { m_beta = beta; }

		// Main update method. Takes your raw sensor data and the time step dt in seconds
		void Update(const RawData_6DOF_Base &imuData, T dt);

		// Get the current orientation quaternion
		Quaternion<T> GetQuaternion() const { return m_q; }

	private:
		T m_beta;
		Quaternion<T> m_q;
	};

	/**
	 * @brief Abstract interface class for IMU device descriptions.
	 *
	 * This class provides an interface for retrieving the name and description of an IMU device.
	 */
	class IDescription
	{
	public:
		virtual const char *GetName() const = 0;
		virtual const char *GetDescription() const = 0;
	};

	/**
	 * @brief Abstract interface class for basic IMU devices.
	 *
	 * This class defines the interface for IMU devices,
	 * including methods for initialization, resetting,
	 * setting gain, reading calibration offsets, and retrieving raw data.
	 * Derived classes must implement the specific functionality
	 * for their respective IMU devices.
	 */
	class IDevice : public IDescription
	{
	public:
		explicit IDevice() = default;
		virtual ~IDevice() = default;

		/**
		 * @brief Reset the IMU device to its default state.
		 */
		virtual int Reset() = 0;

		/**
		 * @brief Initialize the IMU device.
		 * This method must be called after resetting the device.
		 *
		 * @return 0 on success, or a negative error code on failure.
		 */
		virtual int Init() = 0;

		/**
		 * Performs a calibration of the sensor. The calibration process adjusts the hardware offset registers
		 * iteratively to minimize the error between the sensor readings and the expected values (0 g for accelerometer axes,
		 * 0 °/s for gyro axes). The calibration will run for a maximum of max_iterations or until the error is within
		 * target_error LSB of the target values.
		 * Returns 0 on success, or a negative error code on failure.
		 *
		 * @param max_iterations The maximum number of calibration iterations to perform.
		 * A higher number may yield better results but will take longer.
		 * @param target_error The target error threshold in LSB.
		 * Calibration will stop when all sensor readings are within this error
		 * threshold of the target values (0 for accel X/Y, 16384 for accel Z, 0 for gyro X/Y/Z).
		 *
		 * @return 0 if calibration succeeded, or a negative error code if calibration failed after max_iterations.
		 */
		virtual int Calibrate(int max_iterations = 100, int16_t target_error = 50) = 0;

		/**
		 * @brief Set the accelerometer gain (full scale range) for the IMU device.
		 * The gain setting affects the sensitivity of the accelerometer readings.
		 */
		enum class AccelGain : uint8_t
		{
			ACC_GAIN_2G,
			ACC_GAIN_4G,
			ACC_GAIN_8G,
			ACC_GAIN_16G
		};

		/**
		 * @brief Set the accelerometer gain (full scale range) for the IMU device.
		 * The gain setting affects the sensitivity of the accelerometer readings.
		 *
		 * @param gain The desired accelerometer gain setting.
		 */
		virtual void SetAccGain(AccelGain) = 0;

		/**
		 * @brief Set the gyroscope gain (full scale range) for the IMU device.
		 * The gain setting affects the sensitivity of the gyroscope readings.
		 */
		enum class GyroGain : uint8_t
		{
			GYRO_GAIN_250DS,
			GYRO_GAIN_500DS,
			GYRO_GAIN_1000DS,
			GYRO_GAIN_2000DS
		};

		/**
		 * @brief Set the gyroscope gain (full scale range) for the IMU device.
		 * The gain setting affects the sensitivity of the gyroscope readings.
		 *
		 * @param gain The desired gyroscope gain setting.
		 */
		virtual void SetGyroGain(GyroGain) = 0;

		/**
		 * @brief Calibration offsets for the IMU device.
		 *
		 * @return A CalibrationData struct containing the calibration offsets for accelerometer and gyroscope.
		 * In order to calibrate the IMU device, the user can read the current offsets, adjust them as needed,
		 * and then write the new offsets back to the device.
		 */
		struct CalibrationData
		{
			CalibrationData() : ax(0), ay(0), az(0), gx(0), gy(0), gz(0) {}
			CalibrationData(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz)
				: ax(ax), ay(ay), az(az), gx(gx), gy(gy), gz(gz) {}
			int16_t ax, ay, az, gx, gy, gz;
		};

		/**
		 * @brief Read the current calibration offsets from the IMU device.
		 *
		 * @return A CalibrationData struct containing the current calibration
		 * offsets for accelerometer and gyroscope.
		 */
		virtual CalibrationData ReadCalibrationOffsets() = 0;

		/**
		 * @brief Write the given calibration offsets to the IMU device.
		 *
		 * @param offsets A CalibrationData struct containing the calibration
		 * offsets for accelerometer and gyroscope to be written to the device.
		 */
		virtual void WriteCalibrationOffsets(CalibrationData) = 0;

		/**
		 * @brief Get the raw 6DOF data from the IMU device.
		 *
		 * @return A reference to a RawData_6DOF_Base struct containing the raw sensor data.
		 */
		virtual RawData_6DOF_Base &ReadData() = 0;
	};

	/**
	 * @brief Abstract interface class for IMU devices with DMP (Digital Motion Processor) support.
	 */
	template <class T>
	class IDeviceDMP : public virtual IDevice
	{
		static_assert(std::is_floating_point<T>::value,
					  "IDeviceDMP template parameter T must be a floating-point type");
	public:
		explicit IDeviceDMP() = default;
		virtual ~IDeviceDMP() = default;

		virtual Quaternion<T> &ReadQuaternion() = 0;
	};
}
#endif // IMU_H
