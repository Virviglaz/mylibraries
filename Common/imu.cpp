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
 * IMU class source file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "imu.h"
#include <cmath>
#include <algorithm>

using namespace IMU;

template<class T>
EulerAngles<T>::EulerAngles(T x, T y, T z, T w) {
	T norm = std::sqrt(w * w + x * x + y * y + z * z);

	if (norm > std::numeric_limits<T>::epsilon()) {
		T invNorm = static_cast<T>(1.0) / norm;
		w *= invNorm;
		x *= invNorm;
		y *= invNorm;
		z *= invNorm;
	}

	const T one = static_cast<T>(1.0);
	const T two = static_cast<T>(2.0);

	// Roll
	roll = std::atan2(two * (w * x + y * z), one - two * (x * x + y * y));

	// Pitch
	T pitch_val = two * (w * y - z * x);
	pitch = std::asin(std::clamp(pitch_val, -one, one));

	// Yaw
	yaw = std::atan2(two * (w * z + x * y), one - two * (y * y + z * z));
}

template<class T>
Acceleration<T>::Acceleration(const RawData_6DOF_Base &raw_data, T qx, T qy, T qz, T qw)
{
	const T one = static_cast<T>(1.0);
	const T two = static_cast<T>(2.0);

	// 2. Convert quaternion to rotation matrix
	T xx = qx * qx; T yy = qy * qy; T zz = qz * qz;
	T xy = qx * qy; T xz = qx * qz; T yz = qy * qz;
	T wx = qw * qx; T wy = qw * qy; T wz = qw * qz;

	// Matrix elements
	T R11 = one - two * (yy + zz);
	T R12 = two * (xy - wz);
	T R13 = two * (xz + wy);

	T R21 = two * (xy + wz);
	T R22 = one - two * (xx + zz);
	T R23 = two * (yz - wx);

	T R31 = two * (xz - wy);
	T R32 = two * (yz + wx);
	T R33 = one - two * (xx + yy);

	// 3. Transform acceleration to world frame
	x = R11 * raw_data.Accel.GetX() + R12 * raw_data.Accel.GetY() + R13 * raw_data.Accel.GetZ();
	y = R21 * raw_data.Accel.GetX() + R22 * raw_data.Accel.GetY() + R23 * raw_data.Accel.GetZ();
	z = R31 * raw_data.Accel.GetX() + R32 * raw_data.Accel.GetY() + R33 * raw_data.Accel.GetZ();
}

template <class T>
Position<T>& Position<T>::Update(
		const RawData_6DOF_Base &raw_data,
		EulerAngles<T> euler_angles,
		T dt) {
	// 1. Transform acceleration from body frame to world frame using the orientation (real_data)
	T acc_local_x = (static_cast<float>(raw_data.Accel.GetX())) * GRAVITY;
	T acc_local_y = (static_cast<float>(raw_data.Accel.GetY())) * GRAVITY;
	T acc_local_z = (static_cast<float>(raw_data.Accel.GetZ())) * GRAVITY;

	// 2. Calculate velocity by integrating acceleration over time
	T sr = std::sin(euler_angles.GetRollRad());
	T cr = std::cos(euler_angles.GetRollRad());
	T sp = std::sin(euler_angles.GetPitchRad());
	T cp = std::cos(euler_angles.GetPitchRad());
	T sy = std::sin(euler_angles.GetYawRad());
	T cy = std::cos(euler_angles.GetYawRad());

	// Matrix to transform from body frame to world frame
	T R11 = cy * cp;
	T R12 = cy * sp * sr - sy * cr;
	T R13 = cy * sp * cr + sy * sr;

	T R21 = sy * cp;
	T R22 = sy * sp * sr + cy * cr;
	T R23 = sy * sp * cr - cy * sr;

	T R31 = -sp;
	T R32 = cp * sr;
	T R33 = cp * cr;

	// 3. Transform acceleration to world frame
	T acc_world_x = R11 * acc_local_x + R12 * acc_local_y
			+ R13 * acc_local_z;
	T acc_world_y = R21 * acc_local_x + R22 * acc_local_y
			+ R23 * acc_local_z;
	T acc_world_z = R31 * acc_local_x + R32 * acc_local_y
			+ R33 * acc_local_z;

	// 4. Gravity compensation (gravity acts in the negative Z direction in world frame)
	T linear_acc_x = acc_world_x;
	T linear_acc_y = acc_world_y;
	T linear_acc_z = acc_world_z;// - GRAVITY; // Subtract gravity so a stationary sensor reads ~0

	// 5. Zero-velocity update (ZUPT): if the *total* linear acceleration is small,
	// assume the device is stationary and clamp velocity to zero. This is required
	// because pure accelerometer double integration always drifts - any residual
	// bias/noise (even below a per-axis deadband) accumulates without bound over time.
	T acc_mag = std::sqrt(linear_acc_x * linear_acc_x +
			linear_acc_y * linear_acc_y +
			linear_acc_z * linear_acc_z);
	const T stationary_threshold = 0.15f; // m/s^2, tune to your sensor noise floor

	if (acc_mag < stationary_threshold) {
		// Treat as stationary: do not integrate acceleration, and quickly bleed off
		// any residual velocity so the position does not keep drifting.
		velocity.x *= static_cast<T>(0.90);
		velocity.y *= static_cast<T>(0.90);
		velocity.z *= static_cast<T>(0.90);
	} else {
		// 6. First-order integration to update velocity
		velocity.x += linear_acc_x * dt;
		velocity.y += linear_acc_y * dt;
		velocity.z += linear_acc_z * dt;

		// 7. Continuous velocity leak (high-pass behavior) to bound long-term drift
		// caused by any remaining constant acceleration bias/orientation error.
		const T leak = static_cast<T>(0.02); // 1/s, small compared to dt scale
		T decay = static_cast<T>(1.0) - leak * dt;
		velocity.x *= decay;
		velocity.y *= decay;
		velocity.z *= decay;
	}

	// 8. Second-order integration to update position
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	position.z += velocity.z * dt;

	return *this;
}

template<class T>
Position<T>& Position<T>::Update(const Acceleration<T> &acceleration, T dt) {
	// 1. Get the linear acceleration in world frame
	T acc_world_x = acceleration.GetX();
	T acc_world_y = acceleration.GetY();
	T acc_world_z = acceleration.GetZ();

	// 2. Gravity compensation (gravity acts in the negative Z direction in world frame)
	T linear_acc_x = acc_world_x;
	T linear_acc_y = acc_world_y;
	T linear_acc_z = acc_world_z;// - GRAVITY; TODO: Check if gravity compensation is needed here

	// 3. Calculate the magnitude of the linear acceleration
	T acc_mag = std::sqrt(
			linear_acc_x * linear_acc_x + linear_acc_y * linear_acc_y
					+ linear_acc_z * linear_acc_z);

	// Movement detection threshold
	const T stationary_threshold = static_cast<T>(0.15);

	// 4. Zero-velocity update (ZUPT): if the total linear acceleration is small,
	if (acc_mag < stationary_threshold) {
		// The device is considered stationary, and we apply a velocity decay to reduce drift.
		velocity.x *= static_cast<T>(0.90);
		velocity.y *= static_cast<T>(0.90);
		velocity.z *= static_cast<T>(0.90);
	} else {
		// 5. First-order integration to update velocity
		velocity.x += linear_acc_x * dt;
		velocity.y += linear_acc_y * dt;
		velocity.z += linear_acc_z * dt;

		// 6. High-pass filter to reduce long-term drift caused by constant acceleration bias/orientation error.
		const T leak = static_cast<T>(0.02); // 1/s
		T decay = static_cast<T>(1.0) - leak * dt;
		velocity.x *= decay;
		velocity.y *= decay;
		velocity.z *= decay;
	}

	// 7. Second-order integration to update position
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	position.z += velocity.z * dt;

	return *this;
}

template<class T>
Position<T>& Position<T>::Reset() {
	velocity = { static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0) };
	position = { static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(0.0) };
	return *this;
}

template class IMU::EulerAngles<float>;
template class IMU::EulerAngles<double>;

template class IMU::Acceleration<float>;
template class IMU::Acceleration<double>;

template class IMU::Position<float>;
template class IMU::Position<double>;
