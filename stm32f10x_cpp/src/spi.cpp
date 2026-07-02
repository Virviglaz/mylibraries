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
 * STM32F10x SPI [serial peripheral interface] source file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "spi.h"

static uint16_t calc_clock_div(uint32_t bus_freq, uint32_t expected_freq)
{
	uint32_t freq;
	uint16_t div = 1;
	uint16_t n = 0;

	do {
		div <<= 1;
		n++;
		freq = bus_freq / div;
	} while (freq > expected_freq);

	return n - 1;
}

SPI_InterfaceBase &SPI_Bus::Init(SPI_ClockPolarity mode, uint32_t clk_freq)
{
	uint16_t clock_div;

	switch (spiBus_)
	{
	case SPI_Bus::SpiBus::SPI_1:
		spi_ = SPI1;
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; // Clock enable for SPI1, GPIOA and AFIO
		GPIOA->CRL &= ~(0xFFF00000); // Clear PA5 (MSCK), PA6(MISO), PA7(MOSI) configuration bits
		GPIOA->CRL |=  (0xB4B00000); // PA5 = 0xB (AF PP), PA6 = 0x4 (In Float), PA7 = 0xB (AF PP)
		clock_div = calc_clock_div(SystemCoreClock, clk_freq);
		break;
	case SPI_Bus::SpiBus::SPI_2:
		spi_ = SPI2;
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // Clock enable for SPI2
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // Clock enable for GPIOB and AFIO
		GPIOB->CRH &= ~(0xFFF00000); // Clear PB13(MSCK), PB14(MISO), PB15(MOSI) configuration bits
		GPIOB->CRH |=  (0xB4B00000); // PB13 = 0xB (AF PP), PB14 = 0x4 (In Float), PB15 = 0xB (AF PP)
		clock_div = calc_clock_div(SystemCoreClock / 2, clk_freq);
		break;
	}

	spi_->CR1 = (static_cast<uint32_t>(mode) << 1) | (clock_div << 3) | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
	spi_->CR1 |= SPI_CR1_SPE;

	return *this;
}

int SPI_Bus::Transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
	while (length--) {
		while (!(spi_->SR & SPI_SR_TXE)); // Wait until Transmit Buffer is Empty (TXE)
		spi_->DR = *tx_data++; // Send data
		while (!(spi_->SR & SPI_SR_RXNE));
		if (rx_data) {
			*rx_data++ = spi_->DR; // Read received data
		} else {
			(void)spi_->DR; // Read and discard received data if rx_data is NULL
		}
	}

	while (spi_->SR & SPI_SR_BSY);

	return 0;
}

SPI_DeviceBase &SPI_Device::Init()
{
	nssPin_.Set(true); // De-assert NSS
	return *this;
}

SPI_DeviceBase &SPI_Device::Transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t length)
{
	nssPin_.Set(false); // Assert NSS (active low)
	ifs_.Transfer(tx_data, rx_data, length);
	nssPin_.Set(true); // De-assert NSS

	return *this;
}
