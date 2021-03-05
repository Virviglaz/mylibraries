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
 * stepper motor driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "stepper.h"

#ifndef ASSERT
#define ASSERT(x)
#endif

void stepper_init(struct stepper_t *dev,
		  void (*tim)(stp_t new_duty),
		  void (*pin)(stp_t high),
		  void (*dir)(stp_t dir),
		  stp_t tim_freq,
		  stp_t tim_duty)
{
	ASSERT(!dev);

	dev->set_duty = tim;
	dev->set_step = pin;
	dev->set_dir = dir;
	dev->tim_freq = tim_freq;
	dev->tim_duty = tim_duty;
	dev->next_task = 0;
	dev->stop = 0;
	dev->stop_param = 0;
}

stp_t stepper_add_task(struct stepper_t *dev, struct stepper_task_t *task,
	 stp_t steps, stp_t speed, stp_t acc, stp_t dec, stp_t dir,
	 struct stp_buf_t *buf)
{
	struct stepper_task_t *prev_task = dev->next_task;
	double prev_speed = 0;
	double t;

	ASSERT(!dev);
	ASSERT(!task);
	ASSERT(!dev->tim_freq);

	t = (double)speed / (double)acc;
	task->acc_steps = (stp_t)(t * t * (double)acc) >> 1;
	task->acc = (stp_t)((double)speed / (double)task->acc_steps);

	t = (double)speed / (double)dec;
	task->dec_steps = (stp_t)(t * t * (double)dec) >> 1;
	task->dec = (stp_t)((double)speed / (double)task->dec_steps);

	task->speed = speed;

	if (task->acc_steps + task->dec_steps >= steps)
		return 0;

	task->run_steps = steps - task->acc_steps - task->dec_steps;
	task->dir = dir;
	task->buf = buf;

	if (!prev_task) /* empty list */
		dev->next_task = task;
	else {
		prev_speed = prev_task->speed;
		while (prev_task->next)
			prev_task = prev_task->next;
		prev_task->next = task;
	}

	if (buf && buf->acc_buf_size > task->acc_steps) {
		stp_t i;
		for (i = 0; i != task->acc_steps; i++) {
			prev_speed += (double)task->acc;
			buf->acc_buf[i] = (stp_t)((double)dev->tim_duty / prev_speed);
		}
		dev->acc_steps = 0;
	}

	if (buf && buf->dec_buf_size > task->dec_steps) {
		stp_t i;
		for (i = 0; i != task->dec_steps; i++) {
			prev_speed -= (double)task->dec;
			buf->dec_buf[i] = (stp_t)((double)dev->tim_duty / prev_speed);
		}
		dev->dec_steps = 0;
	}

	task->state = STEPPER_IDLE;

	return task->run_steps;
}

void stepper_run(struct stepper_t *dev)
{
	struct stepper_task_t *task = dev->next_task;

	if (task->state != STEPPER_IDLE)
		return;

	if (dev->set_dir)
		dev->set_dir(task->dir);

	task->state = STEPPER_ACCL;
}

enum state_t stepper_status(struct stepper_t *dev)
{
	struct stepper_task_t *task = dev->next_task;

	return task->state;
}

void stepper_use_stop(struct stepper_t *dev,
		stp_t (*stop)(void *param),
		void *param)
{
	ASSERT(!dev);

	dev->stop = stop;
	dev->stop_param = param;
}

void stepper_clean_tasks(struct stepper_t *dev)
{
	ASSERT(!dev);

	dev->next_task = 0;
}

/* frequency base counter per step */
void stepper_isr1(struct stepper_t *dev)
{
	struct stepper_task_t *task = dev->next_task;

	if (!task || task->state > STEPPER_DECL)
		return;

	dev->prev_state = !dev->prev_state;
	dev->set_step(dev->prev_state);

	if (!dev->prev_state)
		return;

	switch (task->state) {
	case STEPPER_ACCL:
		task->acc_steps--;
		if (task->buf)
			dev->cur_duty = task->buf->acc_buf[dev->acc_steps++];
		if (!task->acc_steps)
			task->state++;
		break;
	case STEPPER_RUNN:
		task->run_steps--;
		if(!task->run_steps)
			task->state++;
		return;
	case STEPPER_DECL:
		task->dec_steps--;
		if (task->buf)
			dev->cur_duty = task->buf->dec_buf[dev->dec_steps++];
		if(!task->dec_steps) {
			if (task->next) {
				dev->next_task = task->next;
				stepper_run(dev);
			} else
				task->state++;
		}
		break;
	default:
		return;
	}

	if (dev->cur_duty == dev->pre_duty)
		return;

	dev->set_duty(dev->cur_duty);
	dev->pre_duty = dev->cur_duty;
}

/* timebase counter */
void stepper_isr2(struct stepper_t *dev)
{
	struct stepper_task_t *task = dev->next_task;

	if (!task)
		return;

	switch (task->state) {
	case STEPPER_ACCL:
		dev->cur_speed += task->acc;
		break;
	case STEPPER_DECL:
		dev->cur_speed -= task->dec;
		break;
	}

	/* avoid mechanical impact */
	if (dev->stop)
		if (dev->stop(dev->stop_param))
			task->state = STEPPER_STOP;

	if (task->buf)
		return;

	/* runtime calculations */
	dev->cur_duty = dev->tim_duty / (1 + dev->cur_speed);
}

