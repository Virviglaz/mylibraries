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

#ifndef __ESP_STEPPER_H__
#define __ESP_STEPPER_H__

#include <stdint.h>

/* ESP32 drivers */
#include "driver/gpio.h"

/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

class stepper
{
public:
	stepper(gpio_num_t clk, gpio_num_t dir, float speed,
		float acc, float dec, int core = -1, uint32_t queue_size = 1);
	~stepper();
	bool run();
	void move_to(int32_t new_pos);
	void move(int32_t dist);
	void wait_for_stop();
	void stop();
	void set_limit_check(bool (*func)(void *param), void *param);

private:
	esp_err_t gpio_config(gpio_num_t g);
	void calc_next_step();
	static void handler(void *param);

	bool (*check_limit)(void *param);
	void *limit_func_param;

	gpio_num_t clk_pin;
	gpio_num_t dir_pin;
	float period_us_per_step;
	float dec_step_per_sec;
	float acc_step_per_us;
	float dec_step_per_us;
	float min_per_for_stop;
	float per_slowest_step;
	float next_step_per_us;
	float cur_step_per_in_us;
	uint32_t last_time_step_us;
	int32_t cur_pos_steps;
	int32_t pos_dir_steps;
	int8_t direction;
	bool target_reached = true;
	bool init_done = false;
	bool force_stop = false;

	TaskHandle_t handle = NULL;
	TaskHandle_t caller = NULL;
	QueueHandle_t queue;
};

#endif /* __ESP_STEPPER_H__ */
