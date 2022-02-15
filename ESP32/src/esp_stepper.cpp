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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
 * ESP32 stepper motor driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "esp_stepper.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <math.h>

#ifndef STEPPER_TASK_SIZE
#define STEPPER_TASK_SIZE		2048
#endif

#define GPIO_SET(p, s)			gpio_set_level(p, s)

#define ERR_RTN(x)	if (x) return;
/*
 * If the core is not set the polling method is used.
 * If pinned to core 0 or 1 the handling task is running in the background
 * allowing to use of non-waiting calls. The queue size allows using of
 * non-waiting calls to set up the relative steps with no waiting
 * for the movement to finish.
 * @param clk	GPIO pin to use as the clock output
 * @param dir	GPIO pin to use as direction control output
 * @param speed	Motor speed in pulses per second.
 * @param acc	Motor acceleration in pulses per second^2.
 * @param dec	Motor deceleration in pulses per second^2.
 * @param core	Core number to run handling task in background.
 * @queue_size	Depth of queue to keep movements. 1 makes next call waiting.
 */
stepper::stepper(gpio_num_t clk, gpio_num_t dir, float speed,
	float acc, float dec, int core, uint32_t queue_size)
{
	/* GPIOs */
	clk_pin = clk;
	dir_pin = dir;
	ERR_RTN(gpio_config(clk));
	ERR_RTN(gpio_config(dir));

	/* speed */
	period_us_per_step = 1000000.0 / speed;

	/* acceleration */
	acc_step_per_us = acc / 1E12;

	per_slowest_step = 1000000.0 / sqrt(2.0 * acc);
	min_per_for_stop = per_slowest_step / 2.8;

	/* deceleration */
	dec_step_per_sec = dec;
	dec_step_per_us = dec_step_per_sec / 1E12;

	if (core < 0)
		init_done = true;
	else if (core <= 2) {
		queue = xQueueCreate(queue_size, sizeof(pos_dir_steps));

		init_done = queue && xTaskCreatePinnedToCore(stepper::handler,
			"Stepper", STEPPER_TASK_SIZE, this, 1,
			&this->handle, core) == pdPASS;
	}
}

stepper::~stepper()
{
	wait_for_stop();

	init_done = false;

	if (handle) {
		gpio_reset_pin(clk_pin);
		gpio_reset_pin(dir_pin);
		vTaskDelete(handle);
		vQueueDelete(queue);
	}
}

esp_err_t stepper::gpio_config(gpio_num_t g)
{
	esp_err_t res;

	res = gpio_reset_pin(g);
	if (res)
		return res;

	res = gpio_set_direction(g, GPIO_MODE_OUTPUT);
	if (res)
		return res;

	res = GPIO_SET(g, 0);
	return res;
}

void stepper::handler(void *param)
{
	stepper *s = static_cast<stepper *>(param);

	while (1) {
		int32_t dist;
		/* Wait for start */
		xQueueReceive(s->queue, (void *)&dist, portMAX_DELAY);

		s->pos_dir_steps += dist;

		while (s->run())
			taskYIELD();
		
		/* check more moves to do */
		if (uxQueueMessagesWaiting(s->queue))
			continue;

		/* wakeup caller */
		if (s->caller)
			vTaskResume(s->caller);

		s->caller = NULL;
	}
}

void inline stepper::calc_next_step()
{
	int32_t dist_to_target_s;
	int32_t dist_to_target_us;
	int32_t dec_dist_steps;
	float cur_per_sqrt;
	bool speed_up_flag = false;
	bool slow_down_flag = false;
	bool pos_dir_flag = false;
	bool neg_dir_flag = false;

	/* determine the distance to the target position */
	dist_to_target_s = pos_dir_steps - cur_pos_steps;
	if (dist_to_target_s >= 0L) {
		dist_to_target_us = dist_to_target_s;
		pos_dir_flag = true;
	} else {
		dist_to_target_us = -dist_to_target_s;
		neg_dir_flag = true;
	}

	/*
	 * determine the number of steps needed to go from the current speed
	 * down to a velocity of 0, Steps = Velocity^2 / (2 * Deceleration)
	 */
	cur_per_sqrt = cur_step_per_in_us * cur_step_per_in_us;
	dec_dist_steps =
		(int32_t)round(5E11 / (dec_step_per_sec * cur_per_sqrt));

	/* if: Moving in a positive direction & Moving toward the target */
	if ((direction == 1) && (pos_dir_flag)) {
		/*
		 * if need to start slowing down as we reach the target, or if
		 * we need to slow down because we are going too fast
		 */
		if ((dist_to_target_us < dec_dist_steps) ||
				(next_step_per_us < period_us_per_step))
			slow_down_flag = true;
		else
			speed_up_flag = true;
	}

	/* if: Moving in a positive direction & Moving away from the target */
	else if ((direction == 1) && (neg_dir_flag)) {
		/* need to slow down, then reverse direction */
		if (cur_step_per_in_us < per_slowest_step) {
			slow_down_flag = true;
		} else {
			direction = -1;
			GPIO_SET(dir_pin, 0);
		}
	}

	/* if: Moving in a negative direction & Moving toward the target */
	else if ((direction == -1) && (neg_dir_flag)) {
		/*
		 * if need to start slowing down as we reach the target, or if
		 * we need to slow down because we are going too fast
		 */
		if ((dist_to_target_us < dec_dist_steps) ||
				(next_step_per_us < period_us_per_step))
			slow_down_flag = true;
		else
			speed_up_flag = true;
	}

	/* if: Moving in a negative direction & Moving away from the target */
	else if ((direction == -1) && (pos_dir_flag)) {
		/* need to slow down, then reverse direction */
		if (cur_step_per_in_us < per_slowest_step) {
			slow_down_flag = true;
		} else {
			direction = 1;
			GPIO_SET(dir_pin, 1);
		}
	}

	/* check if accelerating */
	if (speed_up_flag) {
		/* StepPeriod = StepPeriod(1 - a * StepPeriod ^ 2) */
		next_step_per_us = cur_step_per_in_us - acc_step_per_us *
																											 cur_per_sqrt * cur_step_per_in_us;
		if (next_step_per_us < period_us_per_step)
			next_step_per_us = period_us_per_step;
	}

	/* check if decelerating */
	if (slow_down_flag) {
		next_step_per_us = cur_step_per_in_us + dec_step_per_us *
																											 cur_per_sqrt * cur_step_per_in_us;

		if (next_step_per_us > per_slowest_step)
			next_step_per_us = per_slowest_step;
	}
}

/* Perform movement. Returns false if done. */
bool stepper::run()
{
	int32_t dist_to_target_s;
	uint32_t per_last_step_us;
	uint32_t time_us = esp_timer_get_time();

	/* check if currently stopped */
	if (direction == 0) {
		dist_to_target_s = pos_dir_steps - cur_pos_steps;
		/* check if target position in a positive direction */
		if (dist_to_target_s > 0)
		{
			direction = 1;
			GPIO_SET(dir_pin, 1);
			next_step_per_us = per_slowest_step;
			last_time_step_us = time_us;
			return true;
		}

		/* check if target position in a negative direction */
		else if (dist_to_target_s < 0) {
			direction = -1;
			GPIO_SET(dir_pin, 0);
			next_step_per_us = per_slowest_step;
			last_time_step_us = time_us;
			return true;
		} else {
			return false;
		}
	}

	per_last_step_us = time_us - last_time_step_us;

	if (per_last_step_us < (uint32_t)next_step_per_us)
		return true;

	GPIO_SET(clk_pin, 1);

	cur_pos_steps += direction;
	cur_step_per_in_us = next_step_per_us;

	last_time_step_us = time_us;

	calc_next_step();

	GPIO_SET(clk_pin, 0);

	if (cur_pos_steps == pos_dir_steps) {
		if (next_step_per_us >= min_per_for_stop) {
			cur_step_per_in_us = 0.0;
			next_step_per_us = 0.0;
			direction = 0;

			if (target_reached)
				target_reached = false;

			return false;
		}
	}
	return true;
}

static void print_err_no_init(void)
{
	ESP_LOGE(__func__, "Not initialized");
}

#define ERR_CHECK()	if (!init_done) { \
				print_err_no_init(); \
				return; \
			}

/*
 * Move stepper to new absolute position.
 * @param new_pos	Absolute position to move in steps.
 */
void stepper::move_to(int32_t new_pos)
{
	ERR_CHECK();

	if (handle) {
		ESP_LOGE(__func__, "Not supported, use polling");
		return;
	}

	wait_for_stop();

	pos_dir_steps = new_pos;
	target_reached = true;
}

/*
 * Move stepper at distance in steps.
 * @param dist		Relative position to move in steps.
 */
void stepper::move(int32_t dist)
{
	if (handle) {
		xQueueSend(queue, (void *)&dist, portMAX_DELAY);
		return;
	}

	wait_for_stop();

	move_to(cur_pos_steps + dist);
}

/*
 * Wait for movement to finish. In polling mode, it makes the motor run.
 * In handle task-driven mode it suspends the running thread until the movement
 * is finished giving system resources to run.
 */
void stepper::wait_for_stop()
{
	/* Polling */
	if (!handle) {
		while (run()) { }
		return;
	}

	caller = xTaskGetCurrentTaskHandle();
	vTaskSuspend(NULL);
}
