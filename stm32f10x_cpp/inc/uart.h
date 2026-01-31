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

#ifndef STM32F10X_UART_CPP_H
#define STM32F10X_UART_CPP_H

#include <cstdint>
#include "interfaces.h"
#include <stm32f10x.h>

class UART_Device : public UART_InterfaceBase
{
public:
	explicit UART_Device() = default;
	virtual ~UART_Device() = default;

	/**
	 * Constructor
	 *
	 * @param port_number UART port number
	 * @param baudrate UART baudrate
	 * @param peripheral_clock Peripheral clock frequency
	 */
	explicit constexpr
	UART_Device(uint8_t port_number,
				uint32_t baudrate,
				uint32_t peripheral_clock) :
				UART_InterfaceBase(port_number),
				brr_value_(UART_BRR_SAMPLING8(peripheral_clock, baudrate)),
				uart_instance_(Get_UART_Instance(port_number)) {}

	/**
	 * Initialize UART peripheral
	 *
	 * @return Reference to the UART device object
	 */
	virtual UART_InterfaceBase &Init();

	/**
	 * Transfer data over UART
	 *
	 * @note This method sends and receives data byte by byte in a blocking manner.
	 * @param tx_data Data to send
	 * @param rx_data Buffer to store received data
	 * @param length Length of data to transfer
	 *
	 * @return 0 on success, negative value on error
	 */
	virtual int SendReceive(const uint8_t *tx_data,
							uint8_t *rx_data,
							uint32_t length) override;

private:
	static constexpr uint16_t UART_BRR_SAMPLING8(uint32_t pclk, uint32_t baud)
	{
		uint16_t div = (uint16_t)((pclk + baud / 2) / baud);
		return ((uint16_t)((div & ~0x7) << 1) | (uint16_t)(div & 0x07));
	}

	static constexpr USART_TypeDef* Get_UART_Instance(uint8_t port_number)
	{
		USART_TypeDef *instances[] = { USART1, USART2, USART3 };
		return (port_number > 0 && (port_number <= 3)) ?
			instances[port_number] : nullptr;
	}

	uint32_t brr_value_;
	USART_TypeDef *uart_instance_;
};

#endif // STM32F10X_UART_CPP_H
