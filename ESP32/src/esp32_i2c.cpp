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
 * ESP32 i2c driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "esp32_i2c.h"
#include "esp_log.h"
#include "driver/gpio.h"

/*
 * Initialize the I2c bus.
 *
 * @param sda_pin	Number of SDA pin.
 * @param scl_pin	Number of SCL pin.
 * @param freq		Bus frequency. Typically 100..400kHz.
 * @param pullup	Internal pullup enable. Default = yes.
 */
i2c::i2c(int sda_pin, int scl_pin, enum i2c_freq freq, bool pullup, int bus_num)
{
	if (bus_num > I2C_NUM_MAX - 1) {
		ESP_LOGE("I2C", "No more I2c busses available");
		return;
	}

	bus_name[0] = 'I';
	bus_name[1] = '2';
	bus_name[2] = 'C';
	bus_name[3] = bus_num + '0';
	bus_name[4] = 0;

	i2c_config_t cfg;
	cfg.mode = I2C_MODE_MASTER;
	cfg.sda_io_num = sda_pin;
	cfg.sda_pullup_en = pullup;
	cfg.scl_io_num = scl_pin;
	cfg.scl_pullup_en = pullup;
	cfg.master = { .clk_speed = freq };

	gpio_reset_pin((gpio_num_t)sda_pin);
	gpio_reset_pin((gpio_num_t)scl_pin);

	esp_err_t res = i2c_param_config(bus_num, &cfg);
	if (res) {
		ESP_LOGE(bus_name, "configuration failed: %s",
			esp_err_to_name(res));
		return;
	}

	res = i2c_driver_install(bus_num, cfg.mode, 0, 0, 0);
	if (res) {
		ESP_LOGE(bus_name, "driver install failed: %s",
			esp_err_to_name(res));
		return;
	}

	handle = i2c_cmd_link_create();
	if (!handle)
		ESP_LOGE(bus_name, "driver install failed: no memory");

	bus = bus_num;
}

i2c::~i2c()
{
	i2c_cmd_link_delete(handle);
	i2c_driver_delete(bus);
}

/*
 * Write data from buffer to device register reg at bus address addr.
 *
 * @param addr		I2c device address.
 * @param reg		Pointer to register value.
 * @param reg_size	Size of register value.
 * @param buf		Pointer to data buffer.
 * @param size		Buffer size.
 */
esp_err_t i2c::write(uint8_t addr, uint8_t *reg, uint16_t reg_size,
	uint8_t *buf, uint16_t size)
{
	esp_err_t res = i2c_master_start(handle);
	if (res) {
		ESP_LOGE(bus_name, "start error: %s", esp_err_to_name(res));
		return res;
	}

	res = i2c_master_write_byte(handle, addr | I2C_MASTER_WRITE,
		I2C_MASTER_ACK);
	if (res) {
		ESP_LOGE(bus_name, "I2C write address error: %s",
			esp_err_to_name(res));
		return res;
	}

	res = i2c_master_write(handle, reg, reg_size, I2C_MASTER_ACK);
	if (res) {
		ESP_LOGE(bus_name, "I2C write error: %s",
			esp_err_to_name(res));
		return res;
	}

	res = i2c_master_write(handle, buf, size, I2C_MASTER_ACK);
	if (res) {
		ESP_LOGE(bus_name, "I2C write error: %s",
			esp_err_to_name(res));
		return res;
	}

	res = i2c_master_stop(handle);
	if (res) {
		ESP_LOGE(bus_name, "I2C stop error: %s", esp_err_to_name(res));
		return res;
	}

	res = i2c_master_cmd_begin(bus, handle, portMAX_DELAY);
	if (res)
		ESP_LOGE(bus_name, "I2C reading failed: %s",
			esp_err_to_name(res));

	return res;
}

/*
 * Write data from buffer to device register reg at bus address addr.
 *
 * @param addr		I2c device address.
 * @param reg		Register address value.
 * @param buf		Pointer to data buffer.
 * @param size		Buffer size.
 */
esp_err_t i2c::write_reg(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t size)
{
	return write(addr, &reg, sizeof(reg), buf, size);
}

/*
 * Read data from i2c device to buffer.
 *
 * @param addr		I2c device address.
 * @param reg		Register address value.
 * @param buf		Pointer to data buffer.
 * @param size		Buffer size.
 */
esp_err_t i2c::read_reg(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t size)
{
	esp_err_t res = i2c_master_start(handle);
	if (res) {
		ESP_LOGE(bus_name, "I2C start error: %s", esp_err_to_name(res));
		return res;
	}

	/* write address */
	res = i2c_master_write_byte(handle, addr | I2C_MASTER_WRITE,
		I2C_MASTER_ACK);
	if (res) {
		ESP_LOGE(bus_name, "I2C write address error: %s",
			esp_err_to_name(res));
		return res;
	}

	/* write destanation */
	res = i2c_master_write_byte(handle, reg, I2C_MASTER_ACK);
	if (res) {
		ESP_LOGE(bus_name, "I2C write byte error: %s",
			esp_err_to_name(res));
		return res;
	}

	/* repeated start */
	res = i2c_master_start(handle);
	if (res) {
		ESP_LOGE(bus_name, "I2C repeated start error: %s",
			esp_err_to_name(res));
		return res;
	}

	/* write address for reading */
	res = i2c_master_write_byte(handle, addr | I2C_MASTER_READ,
		I2C_MASTER_ACK);
	
	if (size > 1) {
		res = i2c_master_read(handle, buf, size - 1, I2C_MASTER_ACK);
		if (res) {
			ESP_LOGE(bus_name, "I2C reading error: %s",
				esp_err_to_name(res));
			return res;
		}
		buf += size - 1;
		size = 1;
	}

	/* last byte read with NACK */
	if (size) {
		res = i2c_master_read(handle, buf, 1, I2C_MASTER_NACK);
		if (res) {
			ESP_LOGE(bus_name, "I2C reading error: %s",
				esp_err_to_name(res));
			return res;
		}
	}

	res = i2c_master_stop(handle);
	if (res) {
		ESP_LOGE(bus_name, "I2C stop error: %s", esp_err_to_name(res));
		return res;
	}

	res = i2c_master_cmd_begin(bus, handle, portMAX_DELAY);
	if (res)
		ESP_LOGE(bus_name, "I2C reading failed: %s",
			esp_err_to_name(res));

	return res;
}

const char *i2c::get_bus_name()
{
	return bus_name;
}
