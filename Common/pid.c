/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2021 Pavel Nadein
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

#include "pid.h"

void pid_init(struct pid *p, PID_T kp, PID_T ki, PID_T kd)
{
	p->iterm = 0;
	p->last_input = 0;

	p->kp = kp;
	p->ki = ki;
	p->kd = kd;

	p->limits_en = false;
}

void pid_set_limits(struct pid *p, PID_T min, PID_T max)
{
	p->min = min;
	p->max = max;

	p->limits_en = true;
}

void pid_set_target(struct pid *p, PID_T v)
{
	p->set_point = v;
}

PID_T pid_calc(struct pid *p, PID_T input)
{
	/* Compute all the working error variables */
	PID_T error = p->set_point - input;
	PID_T d_input;
	PID_T output;

	p->iterm += (p->ki * error);

	if (p->limits_en) {
		if (p->iterm > p->max)
			p->iterm = p->max;
		else if (p->iterm < p->min)
			p->iterm = p->min;
	}

	d_input = input - p->last_input;

	/* Compute PID Output */
	output = p->kp * error + p->iterm - p->kd * d_input;

	if (p->limits_en) {
		if (output > p->max)
			output = p->max;
		else if (output < p->min)
			output = p->min;
	}

	/* Remember some variables for next time */
	p->last_input = input;

	return output;
}
