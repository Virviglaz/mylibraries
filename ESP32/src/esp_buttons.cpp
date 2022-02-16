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
 * ESP32 momentary button driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "esp_buttons.h"
#include <errno.h>

#ifndef BUTTONS_TASK_SIZE
#define BUTTONS_TASK_SIZE		2048
#endif

#define _delay_ms(x)			vTaskDelay(pdMS_TO_TICKS(x))

class button
{
public:
	button(gpio_num_t pin,
		btn_callback_func c,
		enum btn_trig_edge trig,
		void *p) :
			pin(pin),
			callback(c),
			trig(trig),
			param(p),
			next(NULL),
			prev_state(trig == POSEDGE) {}
	gpio_num_t pin;
	btn_callback_func callback;
	enum btn_trig_edge trig;
	void *param;
	button *next;
	bool prev_state;
};

/*
 * Creates a new task polling the gpio and calling the callback if triggered
 * @param	interval_ms Poll interval in ms, default is 100ms (10Hz)
 * @param	name Task name
 * @param	core Core num to pin the task, -1 if let the RTOS choose
 * @param	prio Task priority
 */
buttons::buttons(uint interval_ms, const char *name, int core, uint prio) : \
		interval_ms(interval_ms)
{
	if (core < 0 || core > 1)
		xTaskCreate(buttons::handler, name,
			BUTTONS_TASK_SIZE, this, prio, &handle);
	else
		xTaskCreatePinnedToCore(buttons::handler, name,
			BUTTONS_TASK_SIZE, this, prio, &handle, core);
}

buttons::~buttons()
{
	button *b = first;

	while (b->next) {
		button *p = b;
		b = b->next;
		delete(p);
	}

	vTaskDelete(handle);
}

void buttons::handler(void *param)
{
	buttons *s = static_cast<buttons *>(param);
	/* should always sleep first call (contructor) */
	vTaskSuspend(NULL);

	while (1) {
		button *btn = s->first;

		_delay_ms(s->interval_ms);

		do {
			bool state = gpio_get_level(btn->pin);
			if (btn->prev_state != state) {
				if (state != (btn->trig == NEGEDGE))
					btn->callback(btn->param);
				btn->prev_state = state;
			}

			btn = btn->next;
		} while (btn);
	}
}

/*
 * Add new button to list and assign the callback.
 * @param pin		Gpio number
 * @param func		Callback function 'void func(void *param)'
 * @param trig		Trigger action on positive or negative edge
 * @param param		User parameter
 *
 * @return 		0 on success, error code if failed
 */
int buttons::add(gpio_num_t pin, btn_callback_func func,
		enum btn_trig_edge trig, void *param)
{
	button *btn = new button(pin, func, trig, param);
	if (!btn)
		return ENOMEM;

	gpio_reset_pin(pin);

	if (gpio_set_direction(pin, GPIO_MODE_INPUT))
		goto error;
	/* POSEDGE ===> PULLDOWN, NEGEDGE ===> PULLUP */
	if (gpio_set_pull_mode(pin, trig == NEGEDGE ? \
		GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY))
		goto error;
	if (gpio_pullup_en(pin))
		goto error;

	if (!first) {
		/* first time call */
		first = btn;

		/* run the task */
		vTaskResume(handle);
	} else {
		button *b = first;

		/* go to the last node */
		while (b->next)
			b = b->next;
		b->next = btn;
	}

	return 0;

error:
	gpio_reset_pin(pin);
	delete(btn);
	return EINVAL;
}
