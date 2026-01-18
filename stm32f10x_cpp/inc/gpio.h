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
 * STM32F10x GPIO peripheral interface.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef STM32F10X_GPIO_H
#define STM32F10X_GPIO_H

#include "interfaces.h"
#include "devices.h"

class GPIO_Interface : public GPIO_InterfaceBase
{
public:
        /** Default constructor */
        explicit GPIO_Interface() = delete;
        virtual ~GPIO_Interface() = default;

        /**
         * Read pin state
         *
         * @param pin GPIO pin number
         *
         * @return Pin state
         */
        int Read(uint16_t port, uint16_t pin) override;

        /**
         * Write pin state
         *
         * @param pin GPIO pin number
         * @param state Pin state
         */
        void Write(uint16_t port, uint16_t pin, int state) override;
};

class GPIO_Device : public GPIO_DeviceBase
{
public:
        /** Default constructor */
        explicit GPIO_Device() = delete;
        virtual ~GPIO_Device() = default;

        /**
         * Constructor
         *
         * @param port GPIO port number
         * @param pin GPIO pin number
         * @param dir Pin direction
         */
        explicit
        GPIO_Device(uint16_t port, uint16_t pin, dir dir)
                : GPIO_DeviceBase(port, pin, dir), direction_(dir) {}

        /**
         * Initialize GPIO pin
         *
         * @return Reference to the GPIO device
         */
        GPIO_Device &Init();

        /**
         * Set pin state
         *
         * @param state Pin state
         */
        GPIO_Device &Set(uint16_t state) override;

        /**
         * Get pin state
         *
         * @return Pin state
         */
        int Get() override;
private:
        GPIO_Device::dir direction_;
};

#endif /* STM32F10X_GPIO_H */
