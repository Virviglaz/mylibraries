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

#ifndef __STEPPER_H__
#define __STEPPER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STEPPER_32BIT_SUPPORT
typedef uint32_t stp_t;
#else
typedef uint16_t stp_t;
#endif

enum state_t {
	STEPPER_ACCL,
	STEPPER_RUNN,
	STEPPER_DECL,
	STEPPER_IDLE,
	STEPPER_HOLD,
	STEPPER_DONE,
	STEPPER_STOP,
};

struct stepper_t {
	void (*set_duty)(stp_t duty);
	void (*set_step)(stp_t high);
	void (*set_dir)(stp_t dir);
	stp_t (*stop)(void *param);

	stp_t cur_speed;
	stp_t cur_duty;
	stp_t pre_duty;
	stp_t acc_steps;
	stp_t dec_steps;

	void *next_task;
	stp_t prev_state;
	stp_t tim_freq;
	stp_t tim_duty;
	void *stop_param;
};

struct stp_buf_t {
	stp_t *acc_buf;
	stp_t *dec_buf;
	stp_t acc_buf_size;
	stp_t dec_buf_size;
};

struct stepper_task_t {
	stp_t speed;
	stp_t run_steps;
	stp_t acc_steps;
	stp_t dec_steps;
	stp_t acc;
	stp_t dec;
	stp_t dir;

	enum state_t state;
	struct stp_buf_t *buf;

	void *next;
};

void stepper_init(struct stepper_t *dev,
		  void (*tim)(stp_t new_duty),
		  void (*pin)(stp_t high),
		  void (*dir)(stp_t dir),
		  stp_t tim_freq,
		  stp_t tim_duty);

stp_t stepper_add_task(struct stepper_t *dev, struct stepper_task_t *task,
	 stp_t steps, stp_t speed, stp_t acc, stp_t dec, stp_t dir,
	 struct stp_buf_t *buf);

void stepper_run(struct stepper_t *dev);

enum state_t stepper_status(struct stepper_t *dev);

void stepper_use_stop(struct stepper_t *dev,
		stp_t (*stop)(void *param),
		void *param);

void stepper_clean_tasks(struct stepper_t *dev);

void stepper_isr1(struct stepper_t *dev);
void stepper_isr2(struct stepper_t *dev);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __STEPPER_H__ */
