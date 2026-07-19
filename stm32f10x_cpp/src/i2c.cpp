/*
 * This file is provided under a MIT license.  When using or
 *   redistributing this file, you may do so under either license.
 *
 *   MIT License
 *
 *   Copyright (c) 2026 Pavel Nadein
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * STM32F10x I2C source file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "i2c.h"
#include <cerrno>

int I2C_Bus::Init(bool fastMode) {
	if (m_bus == I2C1) {
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

		GPIOB->CRL &= ~((0xF << 24) | (0xF << 28));
		GPIOB->CRL |= ((0xF << 24) | (0xF << 28));
	} else if (m_bus == I2C2) {
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

		GPIOB->CRH &= ~((0xF << 8) | (0xF << 12));
		GPIOB->CRH |= ((0xF << 8) | (0xF << 12));
	} else {
		return -EINVAL;
	}

	m_bus->CR1 |= I2C_CR1_SWRST;
	m_bus->CR1 &= ~I2C_CR1_SWRST;

	SystemCoreClockUpdate();
	uint32_t pclk1 = SystemCoreClock;
	uint32_t apb1_prescaler = (RCC->CFGR & RCC_CFGR_PPRE1) >> 8;
	if (apb1_prescaler & 0x4) {
		pclk1 >>= ((apb1_prescaler & 0x3) + 1); // Divide by 2, 4, 8, or 16
	}

	uint32_t pclk1_mhz = pclk1 / 1000000;
	if (pclk1_mhz < 2) return -EIO;
	if (fastMode && (pclk1_mhz < 10)) return -EIO;

	m_bus->CR2 = pclk1_mhz & I2C_CR2_FREQ;

	if (fastMode) {
		// FIX: Correct standard formula for Fast Mode (Duty cycle 1:2)
		uint32_t ccr_val = pclk1 / (400000 * 3);
		if (ccr_val < 1) ccr_val = 1;
		m_bus->CCR = I2C_CCR_FS | (ccr_val & I2C_CCR_CCR);
		m_bus->TRISE = ((300 * pclk1_mhz) / 1000) + 1;
	} else {
		// FIX: Correct standard formula for Standard Mode
		uint32_t ccr_val = pclk1 / (100000 * 2);
		if (ccr_val < 4) ccr_val = 4;
		m_bus->CCR = ccr_val & I2C_CCR_CCR;
		m_bus->TRISE = pclk1_mhz + 1;
	}

	m_bus->CR1 |= I2C_CR1_PE;

	return 0;
}

int I2C_Bus::start(uint8_t address)
{
	if (m_bus->SR2 & I2C_SR2_BUSY)
		return -EBUSY;

	m_bus->CR1 |= I2C_CR1_START;

	while (!(m_bus->SR1 & I2C_SR1_SB));

	m_bus->DR = address << 1; // Master Transmitter (Write bit = 0)

	while (true) {
		if (m_bus->SR1 & I2C_SR1_AF) {
			m_bus->SR1 &= ~I2C_SR1_AF;
			m_bus->CR1 |= I2C_CR1_STOP; // Clear bus on NACK
			return -EIO;
		}

		if (m_bus->SR1 & I2C_SR1_ADDR) {
			(void)m_bus->SR2; // Clear ADDR flag
			return 0;
		}
	}
}

int I2C_Bus::restart(uint8_t address, size_t data_length)
{
	m_bus->CR1 |= I2C_CR1_START;
	while (!(m_bus->SR1 & I2C_SR1_SB));

	m_bus->DR = (address << 1) | 1; // Master Receiver (Read bit = 1)

	// Per RM0008 26.3.3 "Master receiver": for 1- and 2-byte reception the
	// ACK/POS bits MUST be programmed *before* ADDR is cleared, because the
	// ACK/NACK decision for the very first incoming byte(s) is latched by
	// hardware at the exact moment ADDR is cleared. The previous code only
	// touched ACK later, inside read(), well after ADDR had already been
	// cleared - that ordering is why 1-byte/2-byte reads would hang or
	// silently NACK/ACK the wrong byte.
	if (data_length == 1) {
		m_bus->CR1 &= ~I2C_CR1_ACK; // NACK the only byte that will be received
		m_bus->CR1 &= ~I2C_CR1_POS;
	} else if (data_length == 2) {
		m_bus->CR1 |= I2C_CR1_ACK;
		m_bus->CR1 |= I2C_CR1_POS;  // NACK is applied to the 2nd (last) byte
	} else {
		m_bus->CR1 |= I2C_CR1_ACK;  // Stream-ACK bytes as they arrive
		m_bus->CR1 &= ~I2C_CR1_POS;
	}

	// Wait for ADDR, but also bail out if the slave NACKs the repeated
	// START address - the previous code never checked AF here and would
	// hang forever in that case.
	while (true) {
		if (m_bus->SR1 & I2C_SR1_AF) {
			m_bus->SR1 &= ~I2C_SR1_AF;
			m_bus->CR1 |= I2C_CR1_STOP;
			return -EIO;
		}
		if (m_bus->SR1 & I2C_SR1_ADDR)
			break;
	}

	(void)m_bus->SR1;
	(void)m_bus->SR2; // Clear ADDR flag

	if (data_length == 1) {
		// STOP must be queued immediately after ADDR is cleared, before the
		// single incoming byte finishes arriving - otherwise the master
		// keeps clocking the bus waiting for a NACK/STOP decision and the
		// transaction stalls.
		m_bus->CR1 |= I2C_CR1_STOP;
	}

	return 0;
}

int I2C_Bus::write(uint8_t data)
{
	// FIX: Wait for TXE *before* writing to ensure data register is ready
	while (!(m_bus->SR1 & I2C_SR1_TXE));
	m_bus->DR = data;

	// Wait for transmission to verify acknowledgment
	while (!(m_bus->SR1 & I2C_SR1_TXE));

	if (m_bus->SR1 & I2C_SR1_AF) {
		m_bus->SR1 &= ~I2C_SR1_AF;
		m_bus->CR1 |= I2C_CR1_STOP;
		return -EIO;
	}

	return 0;
}

void I2C_Bus::stop()
{
	// FIX: Must wait for the last byte to physically leave the shift register
	while (!(m_bus->SR1 & I2C_SR1_TXE));
	if (m_bus->SR1 & I2C_SR1_BTF) {
		// BTF set means bus is completely done with the current byte
	}
	m_bus->CR1 |= I2C_CR1_STOP;
}

int I2C_Bus::Write(uint8_t device_addr,
                   const uint8_t *reg_addr,
                   size_t reg_addr_length,
                   const uint8_t *data,
                   size_t data_length)
{
	int status = start(device_addr);
	if (status != 0) {
		return status;
	}

	if (reg_addr && reg_addr_length > 0) {
		for (size_t i = 0; i < reg_addr_length; i++) {
			status = write(reg_addr[i]);
			if (status) { stop(); return status; }
		}
	}

	if (data && data_length > 0) {
		for (size_t i = 0; i < data_length; i++) {
			status = write(data[i]);
			if (status) { stop(); return status; }
		}
	}

	// FIX: Wait for final Byte Transfer Finished before setting STOP bit
	while (!(m_bus->SR1 & I2C_SR1_BTF));
	stop();

	return 0;
}

int I2C_Bus::Read(uint8_t device_addr,
                  const uint8_t *reg_addr,
                  size_t reg_addr_length,
                  uint8_t *data,
                  size_t data_length)
{
	int res = start(device_addr);
	if (res) {
		return res; // Return error if start failed
	}

	// Send the register address bytes (if provided)
	if (reg_addr && reg_addr_length > 0) {
		for (size_t i = 0; i < reg_addr_length; i++) {
			res = write(reg_addr[i]);
			if (res) {
				stop(); // Ensure to stop the bus on error
				return res; // Return error if write failed
			}
		}
	}

	if (!data || data_length == 0) {
		stop();
		return 0;
	}

	res = restart(device_addr, data_length);
	if (res) {
		return res; // Repeated START/address was NACKed
	}

	if (data_length == 1) {
		// ACK was cleared and STOP already queued by restart() for this
		// case (must happen before ADDR is cleared / before the byte
		// arrives - see restart()). Just wait for the byte to land.
		while (!(m_bus->SR1 & I2C_SR1_RXNE));
		data[0] = (uint8_t)m_bus->DR;
	} else if (data_length == 2) {
		// POS+ACK were set by restart() to NACK the 2nd (last) byte. Wait
		// for BTF (both bytes fully shifted in: 1st in DR, 2nd in the
		// shift register), then queue STOP and read both bytes back-to-back.
		while (!(m_bus->SR1 & I2C_SR1_BTF));
		m_bus->CR1 |= I2C_CR1_STOP;
		data[0] = (uint8_t)m_bus->DR;
		data[1] = (uint8_t)m_bus->DR;
		m_bus->CR1 &= ~I2C_CR1_POS;
	} else {
		// N >= 3: stream the first N-3 bytes normally (ACK stays set by
		// restart()), then apply the exact BTF-based handshake from
		// RM0008 26.3.3 to land the NACK+STOP precisely on the last byte.
		// (The previous code cleared ACK inside the same loop iteration it
		// waited on RXNE for the last byte - by then that byte's ACK/NACK
		// had typically already been latched by hardware, one byte too
		// late, which is why longer reads could hang or over-read.)
		size_t i = 0;
		for (; i + 3 < data_length; i++) {
			while (!(m_bus->SR1 & I2C_SR1_RXNE));
			data[i] = (uint8_t)m_bus->DR;
		}

		// 3 bytes remain: data[i] = N-2, data[i+1] = N-1, data[i+2] = N
		while (!(m_bus->SR1 & I2C_SR1_BTF));   // N-2 in DR, N-1 fully shifted in
		m_bus->CR1 &= ~I2C_CR1_ACK;            // NACK byte N (not yet started)
		data[i] = (uint8_t)m_bus->DR;          // read N-2; N-1 moves into DR, N starts shifting in

		while (!(m_bus->SR1 & I2C_SR1_BTF));   // N-1 in DR, N fully shifted in (NACKed)
		m_bus->CR1 |= I2C_CR1_STOP;            // queue STOP right after the NACKed byte
		data[i + 1] = (uint8_t)m_bus->DR;      // read N-1

		while (!(m_bus->SR1 & I2C_SR1_RXNE));
		data[i + 2] = (uint8_t)m_bus->DR;      // read N
	}

	return 0;
}


