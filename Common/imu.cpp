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

template <class T>
ImuRealData<T>::ImuRealData(T x, T y, T z, T w)
{
	T norm = std::sqrt(w * w + x * x + y * y + z * z);
	if (norm > 0.0f) {
		w /= norm;
		x /= norm;
		y /= norm;
		z /= norm;
	}

	// Roll
	roll = std::atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

	// Pitch
	T pitch_val = 2.0f * (w * y - z * x);
	pitch = std::asin(std::clamp(pitch_val, static_cast<T>(-1.0), static_cast<T>(1.0)));

	// Yaw
	yaw = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));

}

template <class T>
IMU_Position& IMU_Position::Update(
		const RawData_6DOF_Base &raw_data,
		const ImuRealData<T> &real_data,
		T dt) {
	// 1. Transform acceleration from body frame to world frame using the orientation (real_data)
	T acc_local_x = (static_cast<float>(raw_data.Accel.GetX())) * GRAVITY;
	T acc_local_y = (static_cast<float>(raw_data.Accel.GetY())) * GRAVITY;
	T acc_local_z = (static_cast<float>(raw_data.Accel.GetZ())) * GRAVITY;

	// 2. Calculate velocity by integrating acceleration over time
	T sr = std::sin(real_data.GetRollRad());
	T cr = std::cos(real_data.GetRollRad());
	T sp = std::sin(real_data.GetPitchRad());
	T cp = std::cos(real_data.GetPitchRad());
	T sy = std::sin(real_data.GetYawRad());
	T cy = std::cos(real_data.GetYawRad());

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

	// 4. Gravity compensation (assuming gravity acts in the negative Z direction)
	T linear_acc_x = acc_world_x;
	T linear_acc_y = acc_world_y;
	T linear_acc_z = acc_world_z;// - GRAVITY; // Subtract gravity

	// 5. First-order integration to update velocity
	T deadband = 0.05f; // Remove small noise in acceleration
	if (std::abs(linear_acc_x) > deadband)
		velocity.x += linear_acc_x * dt;
	if (std::abs(linear_acc_y) > deadband)
		velocity.y += linear_acc_y * dt;
	if (std::abs(linear_acc_z) > deadband)
		velocity.z += linear_acc_z * dt;

	// 6. Second-order integration to update position
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
	position.z += velocity.z * dt;

	return *this;
}

IMU_Position& IMU_Position::Reset() {
	velocity = { 0.0f, 0.0f, 0.0f };
	position = { 0.0f, 0.0f, 0.0f };
	return *this;
}

template class IMU::ImuRealData<float>;
template class IMU::ImuRealData<double>;

// IMU_Position is not a class template, but Update() is a template member
// function defined here in the .cpp file. Without explicit instantiation,
// no object code for Update<float>/Update<double> is ever emitted, causing
// "undefined reference to IMU_Position::Update<double>(...)" at link time.
template IMU_Position& IMU::IMU_Position::Update<float>(
		const RawData_6DOF_Base &raw_data,
		const ImuRealData<float> &real_data,
		float dt);
template IMU_Position& IMU::IMU_Position::Update<double>(
		const RawData_6DOF_Base &raw_data,
		const ImuRealData<double> &real_data,
		double dt);
