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

static struct stp_task_t *work;

/* Executed by timer interrupt */
/* Returns the value to be writtern to the timer autoreload register */
uint16_t stepper_handler(void)
{
	if (!work || work->status == STP_IDLE)
		return 0;

	switch (work->status) {
	case STP_ACC:
		if (work->state.speed < work->run)
			work->state.speed += work->acc;
		else
			work->status = STP_RUN;
		break;
	case STP_RUN:
		if (work->state.steps > work->state.dcc_at)
			work->status = STP_DCC;
		break;
	case STP_DCC:
		if (work->state.speed > work->dcc)
			work->state.speed -= work->dcc;
		else
			work->status = STP_IDLE;
		break;
	}

	return (uint16_t)(work->time_base / (uint32_t)work->state.speed);
}

bool stepper_do_step(void)
{
	if (!work || work->status == STP_IDLE)
		return false;
	work->state.steps++;
	return true;
}

enum stp_status stepper_run(struct stp_task_t *task)
{
	/* Run only if IDLE */
	if (!task && task->status != STP_IDLE)
		return STP_ERR;

	work = task;
	work->state.steps = 0;
	work->state.speed = work->acc;
	work->state.dcc_at = work->steps - work->run / work->dcc;

	work->status = STP_ACC;
	return work->status;
}
