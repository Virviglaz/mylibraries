/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020 Pavel Nadein
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
 * Software stepper control driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "sw_stepper.h"

static struct stp_task_t *work = 0;

/* Executed by timer interrupt */
/* Returns the value to be writtern to the timer autoreload register */
uint16_t stepper_duty_calc(const uint32_t time_base)
{
	work->done_steps++;
	return (uint16_t)(time_base / (uint32_t)work->speed);
}

/* Will return STP_DONE when timers have to be stoped */
enum stp_status stepper_speed_calc(void)
{
	/* Free run */
	if (work->speed_to == work->speed_from) {
		if (work->done_steps >= work->todo_steps)
			if (work->next) {
				work = work->next;
				work->speed = work->speed_from;
				return work->status = STP_NEXT;
			} else
				return work->status = STP_DONE;
		return work->status = STP_RUN;
	}

	/* Acceleration or deacceleration */
	if ((work->acc > 0 && work->speed > work->speed_to) || \
		(work->acc < 0 && work->speed < work->speed_to)) {
		if (work->next) {
			work = work->next;
			work->speed = work->speed_from;
			return work->status = STP_NEXT;
		} else
			return work->status = STP_DONE;
	}

	work->speed += work->acc;
	return work->status = STP_RUN;
}

enum stp_status stepper_start(struct stp_task_t *task)
{
	if (work && work->next)
		return STP_RUN;

	if (!task)
		return STP_ERR;

	work = task;

	task->speed = task->speed_from; /* Initial speed */

	return work->status;
}

enum stp_status stepper_status(void)
{
	return work->status;
}

/* Calculate every step based on initial speed */
uint16_t calc_step(uint16_t start, uint16_t steps)
{
	static uint32_t c;
	static uint16_t i, half, n;
	if (start) {
		c = start;
		i = steps;
		n = 0;
		half = steps / 2;
		return 0;
	}

	n++;

	if (n < half) /* Acceleration */
		c = c - 2 * c / (4 * (n + 1));
	else	/* Deacceleration */
		c = c + 2 * c / (4 * (i - n + 2) + 1);

	return n < i ? (uint16_t)c : 0;
}
