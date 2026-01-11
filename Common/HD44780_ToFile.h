/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2026 Pavel Nadein
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
 * HD44780 4/8 bit LCD/OLED driver for C++
 * Output to file implementation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "HD44780.h"
#include "file_ops.h"
#include <memory>

#ifndef __cplusplus
#error "This header file is only for C++ compilation units"
#endif

#ifndef HD44780_TOFILE_H
#define HD44780_TOFILE_H

/**
 * @brief HD44780 Base Class with output to file implementation
 */
class HD44780_ToFileBase : public HD44780_Base
{
public:
	/**
	 * @brief Constructor with output file name
	 *
	 * @param filename Output file name
	 */
	explicit HD44780_ToFileBase(const char *filename = "/tmp/hd44780_output.txt",
								uint8_t rows = 2,
								uint8_t row_size = 16);

	virtual ~HD44780_ToFileBase() = default;

	virtual HD44780_Base &Init(LCD_Type type = HD44780_Base::LCD_Type::LCD, Font font = HD44780_Base::Font::ENGLISH_RUSSIAN_FONT) override;
	virtual HD44780_Base &SetBacklight(bool is_enabled) override { return *this; }
	virtual HD44780_Base &SetCursorPos(uint8_t row, uint8_t col) override;
	virtual HD44780_Base &Clear() override;
	virtual int Print(const char *format, ...) override __attribute__ ((format (printf, 2, 3)));
private:
	class DummyInterface : public HD44780_InterfaceBase
	{
	public:
		void Write(uint16_t data) override {}
	} dummy_interface;

	const char *filename_;
	std::unique_ptr<File> output_file;
	std::unique_ptr<File::Mmap> file_map;
	char *mapped_ptr = nullptr;
	char *cursor_pos_ = nullptr;
	uint8_t rows_;
	uint8_t row_size_;
};

#endif // HD44780_TOFILE_H
