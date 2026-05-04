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
 * File operations. Supports binnary and text files, memory mapping, and CSV parsing.
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
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <variant>

/**
 * File operation class.
 */
class File
{
public:
	/**
	 * Open flags enumeration.
	 */
	enum OpenFlags
	{
		/**
		 * Open file for reading only, writing only, or both reading and writing.
		 */
		READ_ONLY = O_RDONLY,

		/**
		 * Open file for writing only. The file must exist and will not be truncated.
		 */
		WRITE_ONLY = O_WRONLY,

		/**
		 * Open file for both reading and writing. The file must exist and will not be truncated.
		 */
		READ_WRITE = O_RDWR,
	};

	/**
	 * Default constructor.
	 */
	File() = default;

	/**
	 * Construct and open a file.
	 *
	 * @param path File path
	 * @param flags Pen open flags (READ_ONLY, WRITE_ONLY, or READ_WRITE)
	 * @throw std::system_error on open failure
	 */
	File(const char *path, OpenFlags flags = READ_WRITE);

	/**
	 * Construct and open a file.
	 *
	 * @param path File path
	 * @param flags Pen open flags (READ_ONLY, WRITE_ONLY, or READ_WRITE)
	 * @throw std::system_error on open failure
	 */
	File(const std::string &path, OpenFlags flags = READ_WRITE) : File(path.c_str(), flags) {}

	/**
	 * Dummy destructor.
	 *
	 * The actual file closing is handled by the FileInternal destructor
	 * when the shared pointer goes out of scope.
	 */
	~File() = default;

	/**
	 * Open a file.
	 *
	 * @param path File path
	 * @param flags Open flags (READ_ONLY, WRITE_ONLY, or READ_WRITE)
	 * @return Reference to this File object
	 * @throw std::system_error on open failure
	 */
	void Open(const char *path, OpenFlags flags = READ_WRITE);

	/**
	 * Close the file.
	 *
	 * @return Reference to this File object
	 */
	void Close();

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
	 * @note The returned Mmap object is managed by a shared pointer to ensure proper cleanup via RAII.
	 * The mapping will be released when the shared pointer goes out of scope.
	 * The mapping is created with MAP_SHARED, so changes to the mapped region will be reflected
	 * in the underlying file and visible to other processes mapping the same file.
	 * Once created, the Mmap object provides access to the mapped memory region and will
	 * automatically unmap it in its destructor.
	 *
	 * @param size Size of the mapping
	 * @param offset Offset in the file to map
	 * @return Shared pointer to the Mmap object representing the mapped region
	 * @throw std::system_error on mmap failure
	 */
	std::shared_ptr<Mmap> MapFile(size_t size, off_t offset = 0);

	/**
	 * File statistics class.
	 *
	 * Wraps struct stat and provides accessors.
	 */
	class Stats
	{
	public:
		Stats() = default;

		/**
		 * Class constructor
		 *
		 * @param fd File descriptor
		 * @throw std::system_error on fstat failure
		 */
		explicit Stats(int fd);

		/** Last access time (st_atime). */
		time_t GetLastAccessTime() const;

		/** Last modification time (st_mtime). */
		time_t GetLastModTime() const;

		/** Last status change time (st_ctime). */
		time_t GetLastStatusChangeTime() const;

		/** File size in bytes (st_size). */
		off_t GetSize() const;

		/** File mode bits (st_mode). */
		mode_t GetMode() const;

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
		 * Default constructor deleted.
		 * Use shared_ptr instead.
		 */
		Mmap() = delete;

		/**
		 * Class constructor
		 *
		 * @param fd File descriptor
		 * @param size Size of the mapping
		 * @param offset Offset in the file to map
		 * @param flags Access flags (READ, WRITE, or RW)
		 * @throw std::system_error on mmap failure
		 */
		Mmap(int fd, size_t size, off_t offset = 0, OpenFlags flags = READ_WRITE);

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

		 /** 
		  * Overload operator-> to allow direct access to the mapped memory.
		  *
		  * @return Pointer to the mapped memory
		  */
		void *operator->() { return GetPtr(); }

		/**
		 * Get pointer to the mapped region cast to a specific type.
		 *
		 * @param T Desired pointer type
		 * @return Pointer to the mapped memory cast to type T
		 */
		template<typename T>
		T *GetPtrAs() { return static_cast<T *>(ptr); }

	private:
		void *ptr = nullptr;
		size_t size_ = 0;
	};

private:
	void checkFileOpen() const;
	struct FileInternal
	{
		FileInternal(int fd) : fd(fd) {}
		~FileInternal();
		int fd;
	};
	std::shared_ptr<FileInternal> fileInternal;
	OpenFlags openFlags;
	std::shared_ptr<Mmap> fileMapping = nullptr;
};

class TextFile : public File
{
public:
	/**
	 * Default constructor.
	 */
	TextFile() = default;

	/**
	 * Destructor.
	 */
	~TextFile() = default;

	/**
	 * Construct and open a text file.
	 *
	 * @param path File path
	 * @throw std::system_error on open failure
	 */
	explicit
	TextFile(const char *path);

	/**
	 * Read file contents into a string up to null character or end of file.
	 *
	 * @return File contents as a string
	 */
	std::string_view Read();

	/**
	 * Reset read offset to the beginning of the file.
	 *
	 * @return Reference to this TextFile object
	 * @throw std::system_error on lseek failure
	 */
	File& Reset();

	/**
	 * Get the number of lines in the file based on the detected line-ending style.
	 *
	 * @return Number of lines in the file
	 */
	size_t GetLineCount() const { return lineCount; }

	/**
	 * Read a single line from the file based on the detected line-ending style.
	 *
	 * @return The next line from the file as a string, or an empty string if end of file is reached
	 */
	std::string_view ReadLine();

	/**
	 * Read all lines from the file into a vector of strings based on the detected line-ending style.
	 * Each line is stored as a separate string in the vector.
	 * @return A vector containing all lines from the file
	 */
	std::vector<std::string_view> ReadLines();
protected:
	char *readPos = nullptr;
	char *endPos = nullptr;
	off_t fileSize = 0;
	size_t lineCount = 0;
	enum lineEnding
	{
		CRLF,
		LF
	} lineEnding = LF;
};

class CSVFile : public TextFile
{
public:
	/**
	 * Default constructor.
	 */
	CSVFile() = default;

	/**
	 * Destructor.
	 */
	~CSVFile() = default;

	/**
	 * Construct and open a CSV file.
	 *
	 * @param path File path
	 * @throw std::system_error on open failure
	 */	explicit
	CSVFile(const char *path) : TextFile(path) {}

	/* Forward declaration */
	class ParsedValue;

	/**
	 * Parse entire CSV file into a vector of vectors of ParsedValue objects, where each inner vector represents a line of parsed values.
	 *
	 * @return A vector of vectors of ParsedValue objects representing the parsed values from all remaining lines
	 * @throw std::system_error on read failure
	 */
	std::vector<std::vector<ParsedValue>> Parse();

	/* Nested types */
	class ParsedValue
	{
	public:
		ParsedValue() = default;
		ParsedValue(const std::string_view str);

		template<typename T>
		T GetValue() const {
			if (std::holds_alternative<T>(value))
				return std::get<T>(value);
			throw std::bad_variant_access();
		}

		std::variant<std::string, int, float> Get() const { return value; }
	private:
		std::variant<std::string, int, float> value;
	};
private:
	std::vector<ParsedValue> ParseNextLine();
};

#endif /* FILE_OPS_H */
