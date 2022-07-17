/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2022 Pavel Nadein
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
 * PID
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __PID_H__
#define __PID_H__

#include <stdbool.h>

/**
 * PID class (proportional integral derivative regulator)
 * @tparam T	Data type
 */
template<class T>
class pid
{
public:
	/**
	 * @brief	Class constructor
	 *
	 * @param[in] p		(P)roportional Tuning Parameter
	 * @param[in] i		(I)ntegral Tuning Parameter
	 * @param[in] d		(D)erivative Tuning Parameter
	 */
	pid(T p, T i, T d) : kp(p), ki(i), kd(d) {}

	/**
	 * @brief	Class constructor
	 *
	 * @param[in] p		(P)roportional Tuning Parameter
	 * @param[in] i		(I)ntegral Tuning Parameter
	 * @param[in] d		(D)erivative Tuning Parameter
	 * @param[in] min	Mininum value
	 * @param[in] max	Maximum value
	 */
	pid(T p, T i, T d, T min, T max) :
		kp(p), ki(i), kd(d), min(min), max(max) { limits_en = true; }

	/**
	 * @brief	Define the regulation target value
	 *
	 * @param[in] value	Target value
	 */
	void set_target(T value)
	{
		set_point = value;
	}

	/**
	 * @brief	Calculate output value
	 *
	 * @param[in] input	Input value
	 * @return		Output value
	 */
	T calc(T input)
	{
		/* Compute all the working error variables */
		T error = set_point - input;
		T d_input;
		T output;

		iterm += (ki * error);

		if (limits_en) {
			if (iterm > max)
				iterm = max;
			else if (iterm < min)
				iterm = min;
		}

		d_input = input - last_input;

		/* Compute PID Output */
		output = kp * error + iterm - kd * d_input;

		if (limits_en) {
			if (output > max)
				output = max;
			else if (output < min)
				output = min;
		}

		/* Remember some variables for next time */
		last_input = input;

		return output;
	}
private:
	T kp;
	T ki;
	T kd;

	T in;
	T out;
	T set_point = 0;

	T iterm = 0;
	T last_input = 0;
	T min, max;
	bool limits_en = false;
};



#endif /* __PID_H__ */
