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

#include "HD44780_ToFile.h"
#include <stdexcept>
#include <cstdarg>

HD44780_ToFileBase::HD44780_ToFileBase(const char *filename,
									   uint8_t rows,
									   uint8_t row_size) : HD44780_Base(dummy_interface, HD44780_Base::BUS_8B, 0, 0, 0),
														   filename_(filename),
														   rows_(rows),
														   row_size_(row_size)
{ }

HD44780_Base &HD44780_ToFileBase::Init(LCD_Type type, Font font)
{
	output_file = std::make_unique<File>(filename_);

	off_t expected_size = rows_ * (row_size_ + 3); // +3 for \0\r\n
	if (output_file->GetStats().GetSize() < expected_size)
		Clear();

	file_map = std::make_unique<File::Mmap>(output_file->MapFile(0));
	mapped_ptr = static_cast<char *>(file_map->GetPtr());
	cursor_pos_ = mapped_ptr;

	return *this;
}

HD44780_Base &HD44780_ToFileBase::SetCursorPos(uint8_t row, uint8_t col)
{
	if (row > rows_ || col > row_size_) {
		throw std::out_of_range("Cursor position out of range");
	}

	cursor_pos_ = mapped_ptr + row * (row_size_ + 3) + col;

	return *this;
}

HD44780_Base &HD44780_ToFileBase::Clear()
{
	output_file->Seek(0, File::SeekAt::SET);

	for (int i = 0; i < rows_; i++) {
		for (int j = 0; j < row_size_; j++) {
			const char fill_byte = ' ';
			output_file->Write(&fill_byte, 1);
		}

		const char newline[] = { "\0\r\n" };
		output_file->Write(newline, sizeof(newline));
	}

	cursor_pos_ = mapped_ptr;

	return *this;
}

int HD44780_ToFileBase::Print(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int written = vsnprintf(cursor_pos_, row_size_ + 1, format, args);
	va_end(args);

	if (written < 0) {
		throw std::runtime_error("Error formatting string");
	}

	// Move cursor position forward
	cursor_pos_ += written;

	return written;
}
