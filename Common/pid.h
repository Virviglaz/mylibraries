/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2021-2026 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * PID controller implementation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __PID_H__
#define __PID_H__

#ifdef __cplusplus

#include <algorithm>
#include <limits>

template<class T = float>
class PID {
public:
	explicit PID() = default;
	~PID() = default;

    /**
     * @brief Constructor to initialize the PID controller.
     * @param kp Proportional gain
     * @param ki Integral gain
     * @param kd Derivative gain
     * @param max_output Maximum allowable control signal
     * @param min_output Minimum allowable control signal
     * @param dt Sampling time step in seconds
     */
    PID(T kp = static_cast<T>(1.0),
		T ki = static_cast<T>(0.0),
		T kd = static_cast<T>(0.0),
		T max_output = std::numeric_limits<T>::max(),
		T min_output = std::numeric_limits<T>::lowest(),
		T dt = static_cast<T>(1.0))
        : m_kp(kp), m_ki(ki), m_kd(kd), 
          m_max_output(max_output), m_min_output(min_output), 
          m_dt(dt), m_prev_error(0.0), m_integral(0.0) {}

    /**
     * @brief Computes the control output based on the setpoint and current process value.
     * @param setpoint Desired target value
     * @param current_val Actual measured value from the system
     * @return Clamped control signal
     */
    T Calculate(T setpoint, T current_val) noexcept {
        // Calculate the tracking error
        T error = setpoint - current_val;

        // Proportional term (P)
        T P_term = m_kp * error;

        // Integral term (I) with anti-windup clamping to prevent saturation
        m_integral += error * m_dt;
        m_integral = std::clamp(m_integral, m_min_output / m_ki, m_max_output / m_ki);
        T I_term = m_ki * m_integral;

        // Derivative term (D) based on error rate of change
        T derivative = (error - m_prev_error) / m_dt;
        T D_term = m_kd * derivative;

        // Total calculated controller output
        T output = P_term + I_term + D_term;

        // Clamp the final output to match hardware physical limits
        output = std::clamp(output, m_min_output, m_max_output);

        // Save current error for the next iteration
        m_prev_error = error;

        return output;
    }

    /**
     * @brief Resets the internal state (integral sum and previous error).
     * Useful when changing setpoints drastically or restarting the system.
     */
    void Reset() noexcept {
        m_prev_error = 0.0;
        m_integral = 0.0;
    }

private:
    T m_kp; // Proportional coefficient
    T m_ki; // Integral coefficient
    T m_kd; // Derivative coefficient

    T m_max_output; // Upper limit of the control signal
    T m_min_output; // Lower limit of the control signal
    T m_dt;         // Fixed time step between execution cycles

    T m_prev_error; // Error stored from the last calculation step
    T m_integral;   // Accumulated error sum over time
};

#else /* __cplusplus */

#ifndef PID_T
#define PID_T	double
#endif

#include <stdbool.h>

struct pid {
	PID_T disp_kp;	// * we'll hold on to the tuning parameters in user-entered
	PID_T disp_ki;	//   format for display purposes
	PID_T disp_kd;	//

	PID_T kp;	// * (P)roportional Tuning Parameter
	PID_T ki;	// * (I)ntegral Tuning Parameter
	PID_T kd;	// * (D)erivative Tuning Parameter

	PID_T in;		// * Pointers to the Input, Output, and Setpoint variables
	PID_T out;		//   This creates a hard link between the variables and the
	PID_T set_point;	//   PID, freeing the user from having to constantly tell us

	PID_T iterm, last_input;
	PID_T min, max;
	bool limits_en;
};

void pid_init(struct pid *, PID_T, PID_T, PID_T);
void pid_set_limits(struct pid *, PID_T, PID_T);
void pid_set_target(struct pid *, PID_T);
PID_T pid_calc(struct pid *, PID_T);

#endif /* __cplusplus */

#endif /* __PID_H__ */
