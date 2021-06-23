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
 * PID driver with FreeRTOS support
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "pid_FreeRTOS.h"

struct pid_rtos {
	struct pid pid;
	void *private_data;
	PID_T (*read_func)(void *private_data);
	void (*write_func)(PID_T value, void *private_data);
	SemaphoreHandle_t mutex;
	uint delay;
};

static void pid_task(void *args)
{
	struct pid_rtos *pid_rtos = (struct pid_rtos *)args;

	while (1) {
		PID_T in, out;
		if (xSemaphoreTake(pid_rtos->mutex, pid_rtos->delay)
			== pdTRUE) {
			vSemaphoreDelete(pid_rtos->mutex);
			free(pid_rtos);
			vTaskDelete(0);
		}

		in = pid_rtos->read_func(pid_rtos->private_data);
		out = pid_calc(&pid_rtos->pid, in);
		pid_rtos->write_func(out, pid_rtos->private_data);
	}
}

void *create_pid(
	PID_T kp, PID_T ki, PID_T kd, PID_T target,
	PID_T min, PID_T max,
	PID_T (*read_func)(void *private_data),
	void (*write_func)(PID_T value, void *private_data),
	void *private_data, uint stack_size, uint priority,
	uint delay
)
{
	struct pid_rtos *pid_rtos = malloc(sizeof(*pid_rtos));
	if (!pid_rtos)
		return 0;

	pid_rtos->mutex = xSemaphoreCreateBinary();
	if (!pid_rtos->mutex) {
		free(pid_rtos);
		return 0;
	}

	pid_init(&pid_rtos->pid, kp, ki, kd);
	pid_set_limits(&pid_rtos->pid, min, max);
	pid_set_target(&pid_rtos->pid, target);

	pid_rtos->private_data = private_data;
	pid_rtos->read_func = read_func;
	pid_rtos->write_func = write_func;
	pid_rtos->delay = delay;

	if (xTaskCreate(pid_task, 0, stack_size, pid_rtos, priority, 0)
		!= pdPASS) {
		free(pid_rtos);
		return 0;
	}

	return (void *)pid_rtos;
}

void destroy_pid(void *pid)
{
	struct pid_rtos *pid_rtos = (struct pid_rtos *)pid;

	xSemaphoreGive(pid_rtos->mutex);
}
