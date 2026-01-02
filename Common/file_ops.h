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

#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <fcntl.h>
#include <ctime>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * File operation class.
 */
class File
{
public:
	/**
	 * Construct and open a file.
	 *
	 * @param path File path
	 * @param flags Open flags (O_CREAT | O_RDWR by default)
	 * @throw std::system_error on open failure
	 */
	File(const char *path, int flags = O_CREAT | O_RDWR);

	/**
	 * Destructor closes the file.
	 */
	~File();

	/**
	 * Seek origin enumeration.
	 */
	enum SeekAt
	{
		/**	Set access offset from beginning of file. */
		SET,
	
		/**	Set access offset from current position. */
		CURRENT,

		/**	Set access offset from end of file. */
		END
	};

	/**
	 * Set file access offset.
	 *
	 * @param offset Offset in bytes
	 * @param seekAt Seek origin
	 * @return Reference to this File object
	 * @throw std::system_error on lseek failure
	 */
	File& Seek(off_t offset = 0, enum SeekAt seekAt = SeekAt::END);

	/**
	 * Write raw bytes to the file at the current offset.
	 *
	 * @param data Data buffer to write
	 * @param size Number of bytes to write
	 * @throw std::system_error on write failure
	 * @throw std::length_error if not all bytes were written
	 * @return Reference to this File object
	 */
	File& Write(const void *data, size_t size);

	/**
	 * Read raw bytes from the file at the current offset.
	 *
	 * @param dst Destination buffer
	 * @param size Number of bytes to read
	 * @throw std::system_error on read failure
	 * @throw std::length_error if not all bytes were read
	 * @return Reference to this File object
	 */
	File& Read(void *dst, size_t size);

	/**
	 * Read entire file contents into a string.
	 *
	 * @return File contents as a string
	 */
	std::string Read();

	/**
	 * Synchronize file contents to disk.
	 *
	 * @return Reference to this File object
	 * @throw std::system_error on fsync failure
	 */
	File& Sync();

	/* Nested types */
	class Stats;
	class Mmap;

	/* allow streaming file contents to/from C++ streams */
	friend std::ostream &operator<<(std::ostream &out, const File &f);
	friend std::istream &operator>>(std::istream &in, File &f);

	/**
	 * Get file statistics.
	 */
	Stats GetStats();

	/**
	 * Memory-map a file region.
	 *
	 * @param size Size of the mapping
	 * @param offset Offset in the file to map
	 * @param flags Mapping flags (MAP_SHARED by default)
	 * @return Mmap object representing the mapping
	 */
	Mmap MapFile(size_t size, off_t offset = 0, int flags = MAP_SHARED);

	/**
	 * File statistics class.
	 *
	 * Wraps struct stat and provides accessors.
	 */
	class Stats
	{
	public:
		/**
		 * Class constructor
		 *
		 * @param fd File descriptor
		 * @throw std::system_error on fstat failure
		 */
		Stats(int fd);

		/** Last access time (st_atime). */
		time_t GetLastAccessTime();

		/** Last modification time (st_mtime). */
		time_t GetLastModTime();

		/** Last status change time (st_ctime). */
		time_t GetLastStatusChangeTime();

		/** File size in bytes (st_size). */
		off_t GetSize();

		/** File mode bits (st_mode). */
		mode_t GetMode();

	private:
		struct stat s;
	};

	/**
	 * RAII wrapper for a memory-mapped file region.
	 *
	 * The mapping is released in the destructor via munmap.
	 */
	class Mmap
	{
	public:
		/**
		 * Class constructor
		 *
		 * @param fd File descriptor
		 * @param size Size of the mapping
		 * @param offset Offset in the file to map
		 * @param flags Mapping flags
		 * @throw std::system_error on mmap failure
		 */
		Mmap(int fd, size_t size, off_t offset = 0, int flags = MAP_SHARED);

		/**
		 * Class destructor unmaps the region.
		 */
		~Mmap();

		/** 
		 * Get pointer to the mapped region.
		 *
		 * @return Pointer to the mapped memory
		 */
		void *GetPtr();

	private:
		void *ptr = nullptr;
		size_t size_ = 0;
	};

private:
	int fd;
};

#endif /* FILE_OPS_H */
