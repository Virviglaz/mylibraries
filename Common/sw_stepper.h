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


/* Example of use:

struct stp_task_t dec = {
	.speed_from = 10000,
	.speed_to = 100,
	.acc = -10,
	.todo_steps = 0,
	.next = 0,
};

struct stp_task_t run = {
	.speed_from = 10000,
	.speed_to = 10000,
	.acc = 0,
	.todo_steps = 3200,
	.next = &dec,
};

struct stp_task_t acc = {
	.speed_from = 100,
	.speed_to = 10000,
	.acc = 10,
	.todo_steps = 0,
	.next = &run,
};

Code:

static void tim1_handler(void)
{
	tim1_set_freq(stepper_duty_calc(1000000));
}

// 1000 Hz TIMER
static void tim2_handler(void)
{
	if (stepper_speed_calc() == STP_DONE) {
		tim1_enable(false);
		tim2_enable(false);
	}
}

int main( void )
{
	clk_set(CLK_NO_DIV);
	init_gpio();
	delays_init();

	stepper_start(&acc);

	tim1_init(16, 10000);
	tim2_init(TIM2_HSI_DIV_32, 1000);
	tim1_enable_irq(tim1_handler);
	tim2_enable_irq(tim2_handler);
	pwm_enable(STP_CH4, 30);
	rim();

	while (stepper_status() != STP_DONE);
}

*/

#ifndef __SW_STEPPER_H__
#define __SW_STEPPER_H__

#include <stdint.h>
#include <stdbool.h>

enum stp_status {
	STP_DONE,
	STP_NEXT,
	STP_RUN,
	STP_ERR,
};

struct stp_task_t {
	uint16_t speed_from;		/* USER PROVIDE */
	uint16_t speed_to;		/* USER PROVIDE */
	int16_t acc;			/* USER PROVIDE */
	uint32_t todo_steps;		/* USER PROVIDE */
	struct stp_task_t *next;	/* USER PROVIDE */
	uint16_t speed;			/* INTERNAL */
	uint32_t done_steps;		/* REAL VALUE */
	enum stp_status status;		/* STATUS */
};

uint16_t stepper_duty_calc(const uint32_t time_base);
enum stp_status stepper_speed_calc(void);
enum stp_status stepper_start(struct stp_task_t *task);
enum stp_status stepper_status(void);
uint16_t calc_step(uint16_t start, uint16_t steps);

#endif /* __SW_STEPPER_H__ */
