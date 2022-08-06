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
 * ESP32 rotary encoder driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "esp_encoder.h"
#include "esp_err.h"
#include "hal/gpio_ll.h"

#define GPIO_GET(pin)		gpio_ll_get_level(&GPIO, pin)

static esp_err_t gpio_config(gpio_num_t g)
{
	esp_err_t res;

	res = gpio_reset_pin(g);
	if (res)
		return res;

	res = gpio_set_direction(g, GPIO_MODE_INPUT);
	if (res)
		return res;

	res = gpio_set_pull_mode(g, GPIO_PULLUP_ONLY);
	if (res)
		return res;

	res = gpio_pullup_en(g);
	if (res)
		return res;

	res = gpio_set_intr_type(g, GPIO_INTR_POSEDGE);

	return res;
}

IRAM_ATTR void encoder::check_limit(encoder *e)
{
	if (e->value > e->max)
		e->value = e->max;
	if (e->value < e->min)
		e->value = e->min;
}

IRAM_ATTR void encoder::isr_a(void *params)
{
	encoder *e = static_cast<encoder *>(params);
	
	if (e->prev == ENC_A)
		return;
	e->prev = ENC_A;

	e->seq <<= 2;
	e->seq |= GPIO_GET(e->enc_b) << 1;
	e->seq &= 0xF;

	if (e->seq == 4)
		e->value += e->step;

	check_limit(e);
}

IRAM_ATTR void encoder::isr_b(void *params)
{
	encoder *e = static_cast<encoder *>(params);

	if (e->prev == ENC_B)
		return;
	e->prev = ENC_B;

	e->seq <<= 2;
	e->seq |= GPIO_GET(e->enc_a) << 0;
	e->seq &= 0xF;

	if (e->seq == 8)
		e->value -= e->step;

	check_limit(e);
}

#define ERR_RTN(x)	if (x) return;
encoder::encoder(gpio_num_t a, gpio_num_t b)
{
	enc_a = a;
	enc_b = b;

	gpio_uninstall_isr_service();
	ERR_RTN(gpio_install_isr_service(0));
	ERR_RTN(gpio_config(enc_a));
	ERR_RTN(gpio_config(enc_b));
	ERR_RTN(gpio_isr_handler_add(enc_a, encoder::isr_a, this));
	ERR_RTN(gpio_isr_handler_add(enc_b, encoder::isr_b, this));

	init_done = true;
}

encoder::~encoder()
{
	gpio_isr_handler_remove(enc_a);

	gpio_reset_pin(enc_a);
	gpio_reset_pin(enc_b);
}

/*
 * Read the current value
 * @return Returns the actual value.
 */
int32_t encoder::get_value()
{
	return value;
}

/*
 * Define the encoder step size
 * @param new_step	Set the single step value.
 */
void encoder::set_step(int32_t new_step)
{
	step = new_step;
}

/* Invert the encoder direction */
void encoder::invert()
{
	step = -step;
}

/*
 * Update the current value
 * @param new_value	New value.
 */
void encoder::set_value(int32_t new_value)
{
	value = new_value;
}

/*
 * Set the limits.
 * @param new_min	Minimum value.
 * @param new_max	Maximum value.
 */
void encoder::set_limits(int32_t new_min, int32_t new_max)
{
	min = new_min;
	max = new_max;
}
