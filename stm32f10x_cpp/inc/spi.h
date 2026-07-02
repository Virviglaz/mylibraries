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
 * STM32F10x SPI [serial peripheral interface] header file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __SPI_H__
#define __SPI_H__

#include "devices.h"
#include "stm32f10x.h"

class SPI_Bus : public SPI_InterfaceBase
{
public:
	/** Default constructor */
	explicit SPI_Bus() = delete;
	virtual ~SPI_Bus() = default;

	enum class SpiBus : uint8_t {
		SPI_1 = 0,
		SPI_2 = 1,
	};

	SPI_Bus(SpiBus spiNum) : spiBus_(spiNum) {}

	enum class SPI_ClockPolarity : uint8_t
	{
		Low = 0,
		High = 1
	};

	SPI_InterfaceBase &Init(SPI_ClockPolarity mode, uint32_t clk_freq);

	int Transfer(const uint8_t *tx_data,
				 uint8_t *rx_data,
				 size_t length) override;
protected:
	SpiBus spiBus_;
	SPI_TypeDef *spi_;
};

class SPI_Device : public SPI_DeviceBase
{
public:
	 SPI_Device() = delete;
	~SPI_Device() = default;

	explicit SPI_Device(SPI_InterfaceBase &ifs, GPIO_PinBase &nssPin)
		: SPI_DeviceBase(ifs), nssPin_(nssPin) {}

	SPI_DeviceBase &Init();

	SPI_DeviceBase &Transfer(const uint8_t *tx_data,
							 uint8_t *rx_data,
							 size_t length) override;
protected:
	GPIO_PinBase &nssPin_;
};

#endif /* __SPI_H__ */
