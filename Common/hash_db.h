/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2025 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
 * Hash based database header file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef HASH_DB_H
#define HASH_DB_H

#ifdef __cplusplus

#include <stdint.h>

/**
 * HashDB class providing simple hash based database functionality.
 */
class HashDB {
public:
    /**
     * Constructor allocating database memory.
     *
     * @param max_size		Maximum size of the database in bytes.
     * @param name_hash		Hash of the database name.
     */
    HashDB(uint32_t max_size,
           uint32_t name_hash = 0);

    /**
     * Constructor using preallocated database memory.
     *
     * @param location		Pointer to preallocated memory.
     * @param max_size		Maximum size of the database in bytes.
     * @param name_hash		Hash of the database name.
     */
    HashDB(uint32_t *location,
           uint32_t max_size,
           uint32_t name_hash = 0);

    /**
     * Destructor freeing database memory.
     */
    ~HashDB();

    /**
     * Compute hash for a given string at compile time.
     *
     * @param s		String to hash.
     *
     * @return Computed hash value.
     */
    template <uint32_t N>
    static constexpr uint32_t Hash(const char (&s)[N]) noexcept {
        uint32_t h = 5381u;
        for (uint32_t i = 0; i < N - 1; ++i) {
            h = (h * 33u) ^ static_cast<uint8_t>(s[i]);
        }
        return h;
    }

    /**
     * Write data to hash database.
     *
     * @param name_hash		Hash of the entry name.
     * @param data          Data to write.
     *
     * @return 0 on success, errno code on failure.
     */
    template <class T>
    int Write(uint32_t name_hash, const T& data) noexcept {
        return write_int(name_hash, (void *)&data, sizeof(data));
    }

    /**
     * Read data from hash database.
     *
     * @param name_hash		Hash of the entry name.
     * @param data          Reference to store read data.
     *
     * @return 0 on success, errno code on failure.
     */
    template <class T>
    int Read(uint32_t name_hash, T &data) noexcept {
        return read_int(name_hash, &data, sizeof(data));
    }

    /**
     * Get maximum size of the database.
     *
     * @return Maximum size in bytes.
     */
    uint32_t GetMaxSize() const {
        return max_size_;
    }

    /**
     * Get used size of the database.
     *
     * @return Used size in bytes.
     */
    uint32_t GetUsedSize() const {
        db_header_t *header = reinterpret_cast<db_header_t *>(location_);
        return header->db_size;
    }

    /**
     * Check if database name matches.
     *
     * @param name		Name to check.
     *
     * @return true if name matches, false otherwise.
     */
    bool CheckName(uint32_t name_hash) const noexcept {
        db_header_t *header = reinterpret_cast<db_header_t *>(location_);
        return header->name_hash == name_hash;
    }

private:
    uint32_t *location_;
    const uint32_t max_size_;

    struct db_header_t {
        uint32_t name_hash;
        uint32_t db_size;
    };

    void initialize_database(const uint32_t name_hash);
    int write_int(const uint32_t name_hash, const void *data, const uint32_t size);
    int read_int(const uint32_t name_hash, void *data, const uint32_t size);
};

#endif /* __cplusplus */

#endif /* HASH_DB_H */
