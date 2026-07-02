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
 * STM32F10x UART peripheral interface.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "uart.h"

UART_InterfaceBase &UART_Device::Init()
{
    // 1. Enable peripheral clocks and configure GPIO pins BEFORE changing USART registers
    switch (reinterpret_cast<uintptr_t>(uart_instance_))
    {
        case USART1_BASE:
            // Enable USART1 clock, GPIOA clock (for PA9/PA10), and Alternate Function clock
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

            // Configure PA9 (TX) as Alternate Function Push-Pull (50MHz -> 0xB)
            // Configure PA10 (RX) as Input Floating (-> 0x4)
            GPIOA->CRH &= ~(0x00000FFF); // Clear configuration for PA9 and PA10
            GPIOA->CRH |=  (0x000004B0); // PA9 = 0xB (AF PP), PA10 = 0x4 (In Float)
            break;

        case USART2_BASE:
            // USART2 is on APB1 bus, while GPIOA is on APB2 bus
            RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
            RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

            // Configure PA2 (TX) as Alternate Function Push-Pull (50MHz -> 0xB)
            // Configure PA3 (RX) as Input Floating (-> 0x4)
            GPIOA->CRL &= ~(0x0000FFFF); // Clear configuration for PA2 and PA3
            GPIOA->CRL |=  (0x00004B00); // PA2 = 0xB (AF PP), PA3 = 0x4 (In Float)
            brr_value_ >>= 1; // Adjust BRR value for APB1 clock (divided by 2)
            break;

        case USART3_BASE:
            // USART3 is on APB1 bus, while GPIOB is on APB2 bus
            RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
            RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

            // Configure PB10 (TX) as Alternate Function Push-Pull (50MHz -> 0xB)
            // Configure PB11 (RX) as Input Floating (-> 0x4)
            GPIOB->CRH &= ~(0x0000FFFF); // Clear configuration for PB10 and PB11
            GPIOB->CRH |=  (0x00004B00); // PB10 = 0xB (AF PP), PB11 = 0x4 (In Float)
            brr_value_ >>= 1; // Adjust BRR value for APB1 clock (divided by 2)
            break;
    }

    // 2. Configure USART hardware parameters now that clock gating is active
    uart_instance_->BRR = brr_value_; // Set baud rate divider value
    uart_instance_->CR2 = 0;           // Clear CR2 (1 stop bit, default configuration)

    // Enable USART (UE), Transmitter (TE), and Receiver (RE)
    uart_instance_->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    return *this;
}

int UART_Device::SendReceive(const uint8_t *tx_data,
							uint8_t *rx_data,
							size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		// Transmit byte
		while (!(uart_instance_->SR & USART_SR_TXE));
		uart_instance_->DR = tx_data ? tx_data[i] : 0xFF;

		// Receive byte
		while (!(uart_instance_->SR & USART_SR_RXNE));
		if (rx_data)
			rx_data[i] = static_cast<uint8_t>(uart_instance_->DR & 0xFF);
		else
			(void)uart_instance_->DR;
	}

	return 0;
}

static USART_TypeDef *printf_uart_instance = nullptr;
void UART_Device::UseForPrintf()
{
	printf_uart_instance = uart_instance_;
}

#include <stdio.h>
// For C++ compiler, wrap with extern "C" to prevent name mangling
extern "C" {
    int _write(int file, char *ptr, int len)
    {
    	if (!printf_uart_instance) {
			// If no UART instance is set for printf, return an error
			return -1;
		}

        // Iterate through every character in the printf string
        for (int i = 0; i < len; i++)
        {
        	if (ptr[i] == '\n') {
				// If newline character, send carriage return first
				while (!(printf_uart_instance->SR & USART_SR_TXE));
				printf_uart_instance->DR = (uint8_t)'\r';
			}

            // Wait until Transmit Data Register is Empty (TXE bit is set)
            // Use your actual UART instance here (e.g., USART1)
            while (!(printf_uart_instance->SR & USART_SR_TXE));

            // Write character to Data Register
            printf_uart_instance->DR = (uint8_t)ptr[i];
        }

        // Return the number of characters written
        return len;
    }
}

