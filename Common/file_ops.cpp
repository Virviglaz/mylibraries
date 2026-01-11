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

File::File(const char *path, int flags)
{
	fd = open(path, flags, S_IRUSR | S_IWUSR);

	if (fd < 0)
		throw std::system_error(errno, std::generic_category());
}

File::~File()
{
	Close();
}

void File::Close()
{
	if (fd >= 0)
		close(fd);
	fd = -1;
}

File &File::Seek(off_t offset, enum SeekAt seekAt)
{
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

	off_t ret = lseek(fd, offset, whence);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	return *this;
}

File &File::Write(const void *data, size_t size)
{
	ssize_t ret = write(fd, data, size);

	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	if (static_cast<size_t>(ret) != size)
		throw std::length_error("Invalid data size written");

	return *this;
}

File &File::Read(void *dst, size_t size)
{
	ssize_t ret = read(fd, dst, size);

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
	int ret = fsync(fd);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	return *this;
}

File::File(File &&other) noexcept
{
	fd = other.fd;
	other.fd = -1;
}

File::Stats File::GetStats()
{
	return Stats(fd);
}

File::Mmap File::MapFile(size_t size, off_t offset, int flags)
{
	return Mmap(fd, size, offset, flags);
}

File::Stats::Stats(int fd)
{
	if (fstat(fd, &s) < 0)
		throw std::system_error(errno, std::generic_category());
}

time_t File::Stats::GetLastAccessTime()
{
	return s.st_atime;
}

time_t File::Stats::GetLastModTime()
{
	return s.st_mtime;
}

time_t File::Stats::GetLastStatusChangeTime()
{
	return s.st_ctime;
}

off_t File::Stats::GetSize()
{
	return s.st_size;
}

mode_t File::Stats::GetMode()
{
	return s.st_mode;
}

File::Mmap::Mmap(int fd, size_t size, off_t offset, int flags)
{
	if (size == 0) {
		struct stat s;
		if (fstat(fd, &s) == -1)
			throw std::system_error(errno, std::generic_category());
		size = s.st_size;
	}

	ptr = mmap(0, size, PROT_READ | PROT_WRITE, flags, fd, offset);

	if (ptr == MAP_FAILED)
		throw std::system_error(errno, std::generic_category());

	size_ = size;
}

File::Mmap::~Mmap()
{
	if (ptr && size_)
		munmap(ptr, size_);
}

File::Mmap::Mmap(File::Mmap &&other) noexcept
{
	ptr = other.ptr;
	size_ = other.size_;
	other.ptr = nullptr;
	other.size_ = 0;
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
	if (f.fd < 0)
		return out;

	off_t ret = lseek(f.fd, 0, SEEK_SET);
	if (ret < 0)
		throw std::system_error(errno, std::generic_category());

	struct stat st;
	if (fstat(f.fd, &st) == -1)
		return out;

	off_t filesize = st.st_size;
	if (filesize <= 0)
		return out;

	void *ptr = mmap(0, filesize, PROT_READ, MAP_SHARED, f.fd, 0);
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
	if (f.fd < 0)
		return in;

	// Truncate file to zero and seek to start
	if (ftruncate(f.fd, 0) == -1)
		return in;
	if (lseek(f.fd, 0, SEEK_CUR) == (off_t)-1)
		return in;

	while(in.good())
	{
		char c = in.get();
		if (!in)
			break;
		ssize_t w = write(f.fd, &c, 1);
		if (w <= 0)
			return in;
	}

	return in;
}
