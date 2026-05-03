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
 * File operation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "file_ops.h"
#include <stdexcept>
#include <system_error>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <stdint.h>
#include <algorithm>
#include <charconv>

/******************** STATIC FUNCTIONS SECTION *********************/
static std::vector<std::string_view> splitString(std::string_view str, char delimiter = ',')
{
	std::vector<std::string_view> result;

	size_t pos = 0;
	size_t next = 0;

	while ((next = str.find(delimiter, pos)) != std::string_view::npos)
	{
		result.push_back(str.substr(pos, next - pos));
		pos = next + 1;
	}

	result.push_back(str.substr(pos, str.size() - pos - 1));

	return result;
}

static bool isFloat(const std::string_view str)
{
	return str.find('.') != std::string_view::npos;
}

/***************** END OF STATIC FUNCTIONS SECTION ******************/

File::File(const char *path, OpenFlags flags)
{
	Open(path, flags);
}

void File::Open(const char *path, OpenFlags flags)
{
	if (fileInternal)
		throw std::runtime_error("File is already open");

	openFlags = flags;
	int _flags = static_cast<int>(flags);
	if (_flags == WRITE_ONLY || _flags == READ_WRITE)
		_flags |= O_CREAT;

	int fd = open(path, _flags, S_IRUSR | S_IWUSR);

	if (fd < 0)
		throw std::system_error(errno, std::generic_category());

	fileInternal = std::make_shared<FileInternal>(fd);
}

File::FileInternal::~FileInternal()
{
	if (fd >= 0)
		close(fd);
	fd = -1;
}

void File::Close()
{
	/* Delete mapping if used */
	if (fileMapping) {
		fileMapping.reset();
		fileMapping = nullptr;
	}

	/* Close file if opened */
	if (fileInternal) {
		fileInternal.reset();
		fileInternal = nullptr;
	}
}

File &File::Seek(off_t offset, enum SeekAt seekAt)
{
	checkFileOpen();

	int whence = 0;

	switch (seekAt)
	{
	case File::SeekAt::SET:
		whence = SEEK_SET;
		break;
	case File::SeekAt::CURRENT:
		whence = SEEK_CUR;
		break;
	case File::SeekAt::END:
		whence = SEEK_END;
		break;
	}

	off_t ret = lseek(fileInternal->fd, offset, whence);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	return *this;
}

File &File::Write(const void *data, size_t size)
{
	checkFileOpen();

	if (openFlags == READ_ONLY)
		throw std::runtime_error("File is not open for writing");

	ssize_t ret = write(fileInternal->fd, data, size);

	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	if (static_cast<size_t>(ret) != size)
		throw std::length_error("Invalid data size written");

	return *this;
}

File &File::Read(void *dst, size_t size)
{
	checkFileOpen();

	if (openFlags == WRITE_ONLY)
		throw std::runtime_error("File is not open for reading");

	ssize_t ret = read(fileInternal->fd, dst, size);

	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	if (static_cast<size_t>(ret) != size)
		throw std::length_error("Invalid data size read");

	return *this;
}

std::string File::Read()
{
	std::ostringstream oss;
	oss << (*this);
	return oss.str();
}

File &File::Sync()
{
	checkFileOpen();

	if (openFlags == READ_ONLY)
		throw std::runtime_error("File is not open for writing");

	int ret = fsync(fileInternal->fd);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	return *this;
}

File::Stats File::GetStats()
{
	checkFileOpen();

	return Stats(fileInternal->fd);
}

std::shared_ptr<File::Mmap> File::MapFile(size_t size, off_t offset)
{
	checkFileOpen();

	if (!fileMapping)
		fileMapping = std::shared_ptr<File::Mmap>(new File::Mmap(fileInternal->fd, size, offset, openFlags));
	return fileMapping;
}

File::Stats::Stats(int fd)
{
	if (fstat(fd, &s) < 0)
		throw std::system_error(errno, std::generic_category());
}

time_t File::Stats::GetLastAccessTime() const
{
	return s.st_atime;
}

time_t File::Stats::GetLastModTime() const
{
	return s.st_mtime;
}

time_t File::Stats::GetLastStatusChangeTime() const
{
	return s.st_ctime;
}

off_t File::Stats::GetSize() const
{
	return s.st_size;
}

mode_t File::Stats::GetMode() const
{
	return s.st_mode;
}

File::Mmap::Mmap(int fd, size_t size, off_t offset, OpenFlags flags)
{
	if (size == 0) {
		struct stat s;
		if (fstat(fd, &s) == -1)
			throw std::system_error(errno, std::generic_category());
		size = s.st_size;
	}

	int mmap_flags = 0;

	switch (flags)
	{
	case WRITE_ONLY:
		mmap_flags = PROT_WRITE;
		break;
	case READ_ONLY:
		mmap_flags = PROT_READ;
		break;
	case READ_WRITE:
		mmap_flags = PROT_READ | PROT_WRITE;
		break;
	}

	ptr = mmap(0, size, mmap_flags, MAP_SHARED, fd, offset);

	if (ptr == MAP_FAILED)
		throw std::system_error(errno, std::generic_category());

	size_ = size;
}

File::Mmap::~Mmap()
{
	if (ptr && size_)
		munmap(ptr, size_);
}

void *File::Mmap::GetPtr()
{
	return ptr;
}

/*
 * Write file contents to ostream.
 */
std::ostream &operator<<(std::ostream &out, const File &f)
{
	if (f.fileInternal->fd < 0)
		return out;

	off_t ret = lseek(f.fileInternal->fd, 0, SEEK_SET);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	struct stat st;
	if (fstat(f.fileInternal->fd, &st) == -1)
		return out;

	off_t filesize = st.st_size;
	if (filesize <= 0)
		return out;

	void *ptr = mmap(0, filesize, PROT_READ, MAP_SHARED, f.fileInternal->fd, 0);
	if (ptr == MAP_FAILED)
		throw std::system_error(errno, std::generic_category());

	uint8_t *buffer = static_cast<u_int8_t *>(ptr);
	while (filesize--) {
		char c = static_cast<char>(*buffer++);
		if (!c)
			break;
		out.put(c);
		if (!out)
			break;
	}

	munmap(ptr, st.st_size);

	return out;
}

/*
 * Read bytes from istream and replace file contents (truncate + write).
 */
std::istream &operator>>(std::istream &in, File &f)
{
	if (f.fileInternal->fd < 0)
		return in;

	// Truncate file to zero and seek to start
	if (ftruncate(f.fileInternal->fd, 0) == -1)
		return in;
	if (lseek(f.fileInternal->fd, 0, SEEK_CUR) == (off_t)-1)
		return in;

	while(in.good())
	{
		char c = in.get();
		if (!in)
			break;
		ssize_t w = write(f.fileInternal->fd, &c, 1);
		if (w <= 0)
			return in;
	}

	return in;
}

void File::checkFileOpen() const
{
	if (!fileInternal)
		throw std::runtime_error("File is not open");
}

TextFile::TextFile(const char *path) : File(path, READ_ONLY)
{
	std::string_view content = Read();

	/* Determine line-endings */
	auto r_cnt = std::count(content.begin(), content.end(), '\r');
	auto n_cnt = std::count(content.begin(), content.end(), '\n');

	if (r_cnt > 0 && n_cnt > 0) {
		if (r_cnt != n_cnt)
			throw std::runtime_error("Inconsistent line endings detected in file");
		lineEnding = CRLF;
	}

	lineCount = n_cnt + 1; // Add 1 for the last line if it doesn't end with a newline character
}

std::string_view TextFile::Read()
{
	Reset();

	return std::string_view(readPos, fileSize);
}

File &TextFile::Reset()
{
	fileSize = GetStats().GetSize();
	readPos = MapFile(fileSize)->GetPtrAs<char>();
	endPos = readPos + fileSize;

	if (!fileSize)
		throw std::length_error("File is empty");

	return *this;
}

std::string_view TextFile::ReadLine()
{
	if (readPos == endPos)
		return {}; // End of file reached

	char *ptr = readPos;

	while (ptr < endPos) {
		if (lineEnding == CRLF && *ptr == '\r') {
			ptr++; // Skip '\r'
			break;
		} else if (lineEnding == LF && *ptr == '\n') {
			break;
		}
		ptr++;
	}

	if (ptr < endPos)
		ptr++; // Move past the line-ending character(s)

	off_t size = ptr - readPos;

	std::string_view ret(readPos, size);

	readPos += size;

	return ret;
}

std::vector<std::string_view> TextFile::ReadLines()
{
	std::vector<std::string_view> lines(GetLineCount());

	int lineIndex = 0;
	while (true) {
		std::string_view line = ReadLine();
		if (line.empty())
			break;
		lines[lineIndex++] = line;
	}

	return lines;
}

CSVFile::ParsedValue::ParsedValue(const std::string_view str)
{
	if (isFloat(str))
	{
		float floatValue = 0.0f;
		auto result = std::from_chars(str.data(), str.data() + str.size(), floatValue);
		if (result.ec == std::errc())
		{
			value = floatValue;
			return;
		}
	}

	int intValue = 0;
	auto result = std::from_chars(str.data(), str.data() + str.size(), intValue);
	if (result.ec == std::errc())
	{
		value = intValue;
		return;
	}

	value = std::string(str);
}

std::vector<CSVFile::ParsedValue> CSVFile::ParseNextLine()
{
	std::string_view line = ReadLine();
	if (line.empty())
		return {};

	std::vector<std::string_view> columns = splitString(line);
	std::vector<ParsedValue> parsedValues;
	parsedValues.reserve(columns.size());

	for (const auto &col : columns) {
		parsedValues.emplace_back(col);
	}

	return parsedValues;
}

std::vector<std::vector<CSVFile::ParsedValue>> CSVFile::Parse()
{
	std::vector<std::vector<ParsedValue>> allValues(GetLineCount());

	Reset(); // Reset read position to the beginning of the file
	int lineIndex = 0;
	while (true) {
		std::vector<ParsedValue> lineValues = ParseNextLine();
		if (lineValues.empty())
			break;
		allValues[lineIndex++] = std::move(lineValues);
	}

	return allValues;
}
