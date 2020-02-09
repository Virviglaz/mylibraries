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

	struct stp_task_t stp;
	static void tim1_handler(void)
	{
		if (stepper_do_step())
		gpio_inv(STP_CLK);
	}

	static void tim2_handler(void)
	{
		u16 freq = stepper_handler();
		if (freq)
			tim1_set_freq(freq);
	}

Code:
	stp.acc = 20;
	stp.run = 20000;
	stp.dcc = 20;
	stp.steps = 3200 * 2;
	stp.time_base = 1000000;
	stp.status = STP_IDLE;

	tim1_init(8, 65000);
	tim2_init(TIM2_HSI_DIV_8, 10000);
	tim1_enable_irq(tim1_handler);
	tim2_enable_irq(tim2_handler);
	rim();

	while(1) {

		delay_ms(500);
		stepper_run(&stp);
		delay_ms(500);
		while (stp.status != STP_IDLE);
		gpio_inv(STP_DIR);
	}
*/

#ifndef __SW_STEPPER_H__
#define __SW_STEPPER_H__

#include <stdint.h>
#include <stdbool.h>

enum stp_status {
	STP_IDLE,
	STP_ACC,
	STP_RUN,
	STP_DCC,
	STP_ERR,
};

struct stp_task_t {
	uint32_t time_base; /* Hz */
	uint32_t steps; /* How many steps we want to do */
	uint16_t acc, run, dcc; /* SPEEDS in Hz */
	enum stp_status status;
	struct {
		uint16_t speed; /* current speed */
		uint32_t steps; /* steps passed */
		uint16_t dcc_at; /* start deacc at possition */
	} state;
};

uint16_t stepper_handler(void);
bool stepper_do_step(void);
enum stp_status stepper_run(struct stp_task_t *task);

#endif /* __SW_STEPPER_H__ */
