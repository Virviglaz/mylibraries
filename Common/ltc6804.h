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
 * ESP32 LTC6804-2 battery monitor/ballancer driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __LTC6804_H__
#define __LTC6804_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Interface function. Enables the communitaction with the IC.
 *
 * @param tx	Pointer to transfer data.
 * @param rx	Pointer to received data.
 * @param size	Size of data in bytes.
 *
 * @note This function should extablish the communitation. Normally SPI used.
 */
typedef void (*io_func_t)(uint8_t *tx, uint8_t *rx, uint32_t size);

/**
 * @brief Delay implementation function for polling method.
 */
typedef void (*delay_ms_func)(uint32_t ms);

/**
 * @brief Enum defines the discharge timeout.
 */
typedef enum {
	TIMEOUT_DISABLED,
	TIMEOUT_30_SECONDS,
	TIMEOUT_1_MINUTE,
	TIMEOUT_2_MINUTES,
	TIMEOUT_3_MINUTES,
	TIMEOUT_4_MINUTES,
	TIMEOUT_5_MINUTES,
	TIMEOUT_10_MINUTES,
	TIMEOUT_15_MINUTES,
	TIMEOUT_20_MINUTES,
	TIMEOUT_30_MINUTES,
	TIMEOUT_40_MINUTES,
	TIMEOUT_60_MINUTES,
	TIMEOUT_75_MINUTES,
	TIMEOUT_90_MINUTES,
	TIMEOUT_120_MINUTES,
} discharge_timeout_min;

/**
 * @brief LTC6804-2 configuration settings.
 */
typedef struct {
	/* Each cell undervoltage threshold value in volts */
	double under_voltage;

	/* Each cell overvoltage threshold value in volts */
	double over_voltage;

	/* Discharge timeout */
	discharge_timeout_min timeout;

	/* Enables the ADC, wake-up the IC */
	bool refon;

	/*
	 * ADC Mode Option:
	 *
	 * 0 -> Selects Modes 27kHz, 7kHz or 26Hz with MD[1:0] Bits (Default).
	 * 1 -> Selects Modes 14kHz, 3kHz or 2kHz with MD[1:0] Bits.
	 */
	bool fast_adc;
} ltc6804_init_conf;

/**
 * @brief Enum specifies the ADC mode (fast/normal/filtered).
 */
enum adc_mode {
	ADC_27_14_kHz = 1,
	ADC_7_3_kHz = 2,
	ADC_26Hz_2kHz = 3,
};

/** Miscellaneous ADC conversion results struct. */
typedef struct {
	double sum_meas;		/* SOC: Sum of Cells Measurement */
	double die_temp;		/* ITMP: Internal Die Temperature */
	double analog_supp_v;		/* Analog Power Supply Voltage */
	double digital_supp_v;		/* Digital Power Supply Voltage */
	bool cell_overvoltage[12];	/* overvoltage flag */
	bool cell_undervoltage[12];	/* undervoltage flag */
	uint8_t chip_revision;		/* 4-bits chip revision */
} misc_meas;

/** Cell Voltages ADC conversion results struct (ADCV). */
typedef struct {
	double cell[12];		/* CxV: Cell voltages */
} cell_meas;

/** Combined ADC conversion results struct (ADCVAX). */
typedef struct {
	double cell[12];		/* CxV: Cell voltages */
	double gpio[2];			/* GxV: GPIO voltages */
} comb_meas;

/** Auxiliary ADC conversion results struct (ADAX). */
typedef struct {
	double gpio[5];			/* GxV: GPIO voltages */
	double ref_voltage;		/* REF: 2nd Reference Voltage */
} aux_meas;

/**
 * @brief Initialize the SPI and optionally initialize the IC.
 *
 * @Note: This function can be called multiple times to reinitialize the IC.
 *
 * @param cfg Pointer to configuration struct. Use NULL if not used.
 * @param io_func	Pointer to interface function (normally SPI).
 */
int ltc6804_init(ltc6804_init_conf *cfg, io_func_t io_func, delay_ms_func wait);

/**
 * @brief Starts the ADC conversion for all channels including AUX and REF.
 *
 * @param mode			ADC mode.
 * @param discharge_en 		Allow discharge during the measure.
 */
void ltc6804_start_cell_adc_conv(enum adc_mode mode, bool discharge_en);

/**
 * @brief Start GPIO(AUX) inputs ADC conversion.
 *
 * @param mode			ADC mode.
 */
void ltc6804_start_gpio_adc_conv(enum adc_mode mode);

/**
 * @brief Start combined ADC conversion.
 *
 * @param mode			ADC mode.
 */
void ltc6804_start_combined_adc_conv(enum adc_mode mode, bool discharge_en);

/**
 * @brief Start internal parameters ADC conversion (ADSTAT).
 *
 * @param mode			ADC mode.
 */
void ltc6804_start_int_params_adc_conv(enum adc_mode mode);

/**
 * @brief Check ADC conversion is node.
 *
 * @return true			Result is ready.
 * @return false		Conversion is in progress.
 */
bool ltc6804_check_conversion_done(void);

/**
 * @brief Wait for ADC conversion is done.
 *
 * @param poll_interval_ms	Poll interval in ms.
 * @return true			Result is ready.
 * @return false		Conversion is in progress.
 *
 * @note This function performs only 10 attempts to read the ready flag.
 */
bool ltc6804_wait_conversion_done(uint32_t poll_interval_ms);

/**
 * @brief Reset cell conversion results registers.
 */
void ltc6804_clear_cell_voltages(void);

/**
 * @brief Reset GPIO(AUX) conversion results registers.
 */
void ltc6804_clear_gpio_voltages(void);

/**
 * @brief Reset status register.
 */
void ltc6804_clear_status_register(void);

/**
 * @brief Reset all status registers.
 */
void ltc6804_clear_all(void);

/**
 * @brief Read CELLs voltages.
 * @note Non-wating call.
 *
 * @param c			Pointer to cell_meas data struct.
 * @return int			0 if success, error code if error.
 */
int ltc6804_read_cells(cell_meas *c);

/**
 * @brief Read CELLs voltages and 2 GPIOs.
 * @note Non-wating call.
 *
 * @param c			Pointer to comb_meas data struct.
 * @return int			0 if success, error code if error.
 */
int ltc6804_read_comb(comb_meas *c);

/**
 * @brief Read GPIO voltages and second reference voltage.
 * @note Non-wating call.
 *
 * @param c			Pointer to aux_meas data struct.
 * @return int			0 if success, error code if error.
 */
int ltc6804_read_aux(aux_meas *c);

/**
 * @brief Read miscellaneous ADC settings.
 * @note Non-wating call.
 *
 * @param c			Pointer to misc_meas struct.
 * @return int			0 if success, error code if error.
 */
int ltc6804_read_int_params(misc_meas *c);

/**
 * @brief Start 12-CELLs ADC conversion and wait for result.
 * @note This is a waiting call.
 *
 * @param c			Pointer to cell_meas struct to store the data.
 * @param mode			ADC mode
 * @param discharge_en		Enable the ballancing discharge during measure.
 * @param poll_interval_ms	Polling interval in ms.
 * @return int			0 if success, error code if error.
 */
int ltc6804_convert_cell(cell_meas *c, enum adc_mode mode, bool discharge_en,
	uint32_t poll_interval_ms);

/**
 * @brief Start combined ADC conversion (CELLs + 2 GPIOs) and wait for result.
 * @note This is a waiting call.
 *
 * @param c			Pointer to comb_meas struct to store the data.
 * @param mode			ADC mode
 * @param discharge_en		Enable the ballancing discharge during measure.
 * @param poll_interval_ms	Polling interval in ms.
 * @return int			0 if success, error code if error.
 */
int ltc6804_convert_comb(comb_meas *c, enum adc_mode mode, bool discharge_en,
	uint32_t poll_interval_ms);


/**
 * @brief Start aux ADC conversion (5 GPIOs + II-reference) and wait for result.
 * @note This is a waiting call.
 *
 * @param c			Pointer to aux_meas struct to store the data.
 * @param mode			ADC mode
 * @param poll_interval_ms	Polling interval in ms.
 * @return int			0 if success, error code if error.
 */
int ltc6804_convert_aux(aux_meas *c, enum adc_mode mode,
	uint32_t poll_interval_ms);

/**
 * @brief Start internal params ADC conversion (ADSTAT).
 * @note This is a waiting call.
 *
 * @param c			Pointer to misc_meas struct to store the data.
 * @param mode			ADC mode
 * @param poll_interval_ms	Polling interval in ms.
 * @return int			0 if success, error code if error.
 */
int ltc6804_convert_misc(misc_meas *c, enum adc_mode mode,
	uint32_t poll_interval_ms);

/**
 * @brief Enables/disables the discharging per cell.
 *
 * @param cell 			0..11 cell number.
 * @param state			true enables the discharge, false disables.
 * @return int			0 if success, error code if error.
 */
int ltc6804_discharge(uint8_t cell, bool state);

#ifdef __cplusplus
}
#endif

#endif /* __LTC6804_H__ */
