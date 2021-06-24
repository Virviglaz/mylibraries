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
 * PID driver with linux support
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "pid_linux.h"
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>

struct pid_rtos {
	struct pid pid;
	void *private_data;
	PID_T (*read_func)(void *private_data);
	void (*write_func)(PID_T value, void *private_data);
	sem_t semaphore;
	int delay;
	pthread_t thread;
};

static void *pid_task(void *args)
{
	struct pid_rtos *pid = (struct pid_rtos *)args;

	while (1) {
		PID_T in, out;
		struct timespec ts;

		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_nsec += pid->delay * 1000000UL;
		while (ts.tv_nsec >= 1000000000UL) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000UL;
		}
		
		if (sem_timedwait(&pid->semaphore, &ts) == -1
			&& errno != ETIMEDOUT) {
			printf("errno:%s, %lu\n", strerror(errno), ts.tv_nsec);
			sem_destroy(&pid->semaphore);
			free(pid);
			break;
		}

		in = pid->read_func(pid->private_data);
		out = pid_calc(&pid->pid, in);
		pid->write_func(out, pid->private_data);
	}
printf("DONE\n");
	return 0;
}

void *create_pid(
	PID_T kp, PID_T ki, PID_T kd, PID_T target,
	PID_T min, PID_T max,
	PID_T (*read_func)(void *private_data),
	void (*write_func)(PID_T value, void *private_data),
	void *private_data, int delay
)
{
	pthread_attr_t attr;
	struct pid_rtos *pid = malloc(sizeof(*pid));
	if (!pid)
		return 0;

	if (pthread_attr_init(&attr)) {
		free(pid);
		return 0;
	}
	
	if (sem_init(&pid->semaphore, 1, 0)) {
		free(pid);
		return 0;
	}

	pid_init(&pid->pid, kp, ki, kd);
	pid_set_limits(&pid->pid, min, max);
	pid_set_target(&pid->pid, target);

	pid->private_data = private_data;
	pid->read_func = read_func;
	pid->write_func = write_func;
	pid->delay = delay;

	if (pthread_create(&pid->thread, &attr, pid_task, pid)) {
		sem_destroy(&pid->semaphore);
		free(pid);
		return 0;
	}

	return (void *)pid;
}

void update_pid(void *ptr, PID_T new_target)
{
	struct pid_rtos *pid = (struct pid_rtos *)ptr;

	pid_set_target(&pid->pid, new_target);
}

void destroy_pid(void *ptr)
{
	struct pid_rtos *pid = (struct pid_rtos *)ptr;

	sem_post(&pid->semaphore);
}
