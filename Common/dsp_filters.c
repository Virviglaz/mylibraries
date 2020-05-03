/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2019 Pavel Nadein
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
 * Data filtering library
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "dsp_filters.h"

float kalman_calc_f(float value, float *prev, const float k)
{
	float res = k * value;
	value = 1.0 - k;
	*prev *= value;
	res += *prev;
	*prev = res;

	return res;
}

double kalman_calc_d(double value, double *prev, const double k)
{
	double res = k * value;
	value = 1.0 - k;
	*prev *= value;
	res += *prev;
	*prev = res;

	return res;
}

void add_value_f(float *buf, uint16_t size, float value)
{
	uint16_t i;

	for (i = 1; i != size; i++)
		buf[i] = buf[i + 1];

	buf[size - 1] = value;
}

void add_value_d(double *buf, uint16_t size, double value)
{
	uint16_t i;

	for (i = 1; i != size; i++)
		buf[i] = buf[i + 1];

	buf[size - 1] = value;
}

double get_average_f(float *buf, uint16_t size, uint16_t average)
{
	float sum = 0;
	uint16_t i;

	for (i = size - average; i != size; i++)
		sum += buf[i];

	return sum / average;
}

double get_average_d(double *buf, uint16_t size, uint16_t average)
{
	double sum = 0;
	uint16_t i;

	for (i = size - average; i != size; i++)
		sum += buf[i];

	return sum / average;
}
