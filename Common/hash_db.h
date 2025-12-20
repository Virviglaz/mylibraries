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
#include <stdlib.h>
#include <errno.h>
#include <string>

/**
 * HashDB class providing simple hash based database functionality.
 */
class HashDB {
public:
    /**
     * Constructor allocating database memory.
     *
     * @param name	        Name of the database.
     * @param max_size		Maximum size of the database in bytes.
     */
    HashDB(const char *name,
        const uint32_t max_size)
        : max_size_(max_size) {
        location_ = (uint32_t *)malloc(max_size_);
        initialize_database(name);
    }

    /**
     * Constructor using preallocated memory.
     *
     * @param name		    Name of the database.
     * @param location		Pointer to location where database is stored.
     * @param max_size		Maximum size of the database in bytes.
     */
    HashDB(const char *name,
           uint32_t *location,
           const uint32_t max_size)
        : location_(location), max_size_(max_size) {
            initialize_database(name);
    }

    /**
     * Destructor freeing database memory.
     */
    ~HashDB() {
        free(location_);
    }

    /**
     * Write data to hash database.
     *
     * @param name		Name of the data entry.
     * @param data		Reference to data to write.
     *
     * @return 0 on success, errno code on failure.
     */
    template <class T>
    int write(const char *name, const T& data) {
        return write_int(name, (void *)&data, sizeof(data));
    }

    /**
     * Write data to hash database.
     *
     * @param name		Name of the data entry.
     * @param data		Reference to data to write.
     *
     * @return 0 on success, errno code on failure.
     */
    template <class T>
    int write(const std::string& name, const T& data) {
        return write<T>(name.c_str(), data);
    }

    /**
     * Read data from hash database.
     *
     * @param name		Name of the data entry.
     * @param data		Reference to data to read into.
     *
     * @return 0 on success, errno code on failure.
     */
    template <class T>
    int read(const char *name, T &data) {
        return read_int(name, &data, sizeof(data));
    }

    /**
     * Read data from hash database.
     *
     * @param name		Name of the data entry.
     * @param data		Reference to data to read into.
     *
     * @return 0 on success, errno code on failure.
     */
    template <class T>
    int read(const std::string& name, T &data) {
        return read<T>(name.c_str(), data);
    }

    /**
     * Get maximum size of the database.
     *
     * @return Maximum size in bytes.
     */
    uint32_t get_max_size() const {
        return max_size_;
    }

    /**
     * Get used size of the database.
     *
     * @return Used size in bytes.
     */
    uint32_t get_used_size() const {
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
    bool check_name(const char *name) {
        db_header_t *header = reinterpret_cast<db_header_t *>(location_);
        return header->name_hash == hash(name);
    }

private:
    uint32_t *location_;
    const uint32_t max_size_;

    struct db_header_t {
        uint32_t name_hash;
        uint32_t db_size;
    };

    void initialize_database(const char *name) {
        db_header_t *header = reinterpret_cast<db_header_t *>(location_);
        header->name_hash = name ? hash(name) : 0;
        header->db_size = sizeof(db_header_t);
    }

    constexpr uint32_t hash(const char *s, const uint32_t off = 0) {
        return !s[off] ? 5381 : (hash(s, off+1)*33) ^ s[off];
    }

    int write_int(const char *name, const void *data, const uint32_t size) {
        db_header_t *header = reinterpret_cast<db_header_t *>(location_);
        uint32_t name_hash = hash(name);
        uint32_t offset = sizeof(db_header_t);
        bool found = false;

        // Check if entry already exists
        while (offset < header->db_size) {
            db_header_t *entry = reinterpret_cast<db_header_t *>(
                reinterpret_cast<uint8_t *>(location_) + offset);
            if (entry->name_hash == name_hash) {
                found = true;
                break; // Entry exists, will overwrite
            }
            offset += sizeof(db_header_t) + entry->db_size;
        }

        // Check if there is enough space
        if (header->db_size + sizeof(db_header_t) + size > max_size_) {
            return ENOSPC; // Not enough space
        }

        // Write new entry
        db_header_t *new_entry = reinterpret_cast<db_header_t *>(
            reinterpret_cast<uint8_t *>(location_) + offset);
        new_entry->name_hash = name_hash;
        new_entry->db_size = size;

        memcpy(reinterpret_cast<uint8_t *>(new_entry) + sizeof(db_header_t),
               data,
               size);

        if (!found)
            header->db_size += sizeof(db_header_t) + size;

        return 0; // Success
    }

    int read_int(const char *name, void *data, const uint32_t size) {
        db_header_t *header = reinterpret_cast<db_header_t *>(location_);
        uint32_t name_hash = hash(name);
        uint32_t offset = sizeof(db_header_t);

        while (offset < header->db_size) {
            db_header_t *entry = reinterpret_cast<db_header_t *>(
                reinterpret_cast<uint8_t *>(location_) + offset);
            if (entry->name_hash == name_hash) {
                if (entry->db_size != size)
                    return EINVAL; // Size mismatch

                memcpy(data,
                       reinterpret_cast<uint8_t *>(entry) + sizeof(db_header_t),
                       size);
                return 0; // Success
            }
            offset += sizeof(db_header_t) + entry->db_size;
        }

        return ENOENT; // Entry not found
    }
};

#endif /* __cplusplus */

#endif /* HASH_DB_H */
