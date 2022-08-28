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

#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "esp_err.h"
#include "esp_check.h"
/* ESP32 drivers */
#include "driver/gpio.h"
#include "hal/gpio_ll.h"

#include <stdint.h>
#include <limits>

#ifndef ERR_CHK
#define ERR_CHK(x)		if (x) return;
#endif

#ifndef ERR_RTN
#define ERR_RTN(x)		res = x; if (res) return res;
#endif

#define GPIO_GET(pin)		gpio_ll_get_level(&GPIO, pin)

template <class T>
class encoder
{
public:
	/**
	 * @brief Construct a new encoder object
	 *
	 * @param a	GPIO number of ENC_A input
	 * @param b	GPIO number of ENC_B input
	 * @param min	Optional limit minimum value
	 * @param max	Optional limit maxinum value
	 * @param int_type	Optional interrupt trigger direction
	 */
	encoder(gpio_num_t a, gpio_num_t b,
		T min = std::numeric_limits<T>::min(),
		T max = std::numeric_limits<T>::max(),
		gpio_int_type_t int_type = GPIO_INTR_NEGEDGE) {
		enc_a = a;
		enc_b = b;
		min_val = min;
		max_val = max;
		gpio_uninstall_isr_service();
		ERR_CHK(gpio_install_isr_service(0));
		ERR_CHK(gpio_config(enc_a, int_type));
		ERR_CHK(gpio_config(enc_b, int_type));
		ERR_CHK(gpio_isr_handler_add(a, encoder::isr_a, this));
		ERR_CHK(gpio_isr_handler_add(b, encoder::isr_b, this));
	}

	/**
	 * @brief Destroy the encoder object
	 */
	~encoder() {
		gpio_isr_handler_remove(enc_a);
		gpio_reset_pin(enc_a);
		gpio_reset_pin(enc_b);
	}

	/**
	 * @brief Read the current value.
	 *
	 * @return Returns the actual value.
	 */
	T get_value() {
		T tmp;
		portDISABLE_INTERRUPTS();
		tmp = value;
		portENABLE_INTERRUPTS();
		return tmp;
	}

	/**
	 * @brief Read the noise counter value.
	 *
	 * @return Returns noise counter value.
	 */
	T get_noise_level() {
		T tmp;
		portDISABLE_INTERRUPTS();
		tmp = noise;
		portENABLE_INTERRUPTS();
		return tmp;
	}

	/**
	 * @brief Define the encoder step size.
	 *
	 * @param new_step	Set the single step value.
	 */
	void set_step(T new_step = 1) {
		portDISABLE_INTERRUPTS();
		step = new_step;
		portENABLE_INTERRUPTS();
	}

	/**
	 * @brief Invert the encoder direction
	 */
	void invert() {
		portDISABLE_INTERRUPTS();
		step = -step;
		portENABLE_INTERRUPTS();
	}

	/**
	 * @brief Update the current value.
	 *
	 * @param new_value	New value.
	 */
	void set_value(T new_value) {
		portDISABLE_INTERRUPTS();
		value = new_value;
		portENABLE_INTERRUPTS();
	}

	/**
	 * @brief Set the limits.
	 *
	 * @param new_min	Minimum value.
	 * @param new_max	Maximum value.
	 */
	void set_limits(T new_min, T new_max) {
		portDISABLE_INTERRUPTS();
		min_val = new_min;
		max_val = new_max;
		portENABLE_INTERRUPTS();
	}
private:
	static esp_err_t gpio_config(gpio_num_t g, gpio_int_type_t int_type)
	{
		esp_err_t res;
		ERR_RTN(gpio_reset_pin(g));
		ERR_RTN(gpio_set_direction(g, GPIO_MODE_INPUT));
		ERR_RTN(gpio_set_pull_mode(g, GPIO_PULLUP_ONLY));
		ERR_RTN(gpio_pullup_en(g));
		ERR_RTN(gpio_set_intr_type(g, int_type));
		return 0;
	}

	IRAM_ATTR static void isr_a(void *par)
	{
		encoder *e = static_cast<encoder *>(par);
		if (e->prev == ENC_A) {
			e->noise++;
			return;
		}
		e->prev = ENC_A;

		if (GPIO_GET(e->enc_b))
			e->value += e->step;
		else
			e->value -= e->step;

		if (e->value > e->max_val)
			e->value = e->max_val;
		else if (e->value < e->min_val)
			e->value = e->min_val;
	}

	IRAM_ATTR static void isr_b(void *par)
	{
		encoder *e = static_cast<encoder *>(par);
		if (e->prev == ENC_B) {
			e->noise++;
			return;
		}
		e->prev = ENC_B;
	}

	enum { ENC_A, ENC_B } prev;
	gpio_num_t enc_a;
	gpio_num_t enc_b;
	T value = 0;
	T step = 1;
	T min_val;
	T max_val;
	T noise = 0;
};

#endif /* __ENCODER_H__ */
