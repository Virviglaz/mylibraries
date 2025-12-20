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
 * Hash based database source file.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include "hash_db.h"

HashDB::HashDB(uint32_t max_size,
               uint32_t name_hash) 
    : max_size_(max_size) {
    location_ = (uint32_t *)malloc(max_size_);
    initialize_database(name_hash);
}

HashDB::HashDB(uint32_t *location,
               uint32_t max_size,
               uint32_t name_hash)
    : location_(location), max_size_(max_size) {
    initialize_database(name_hash);
}

HashDB::~HashDB() {
    free(location_);
}

void HashDB::initialize_database(const uint32_t name_hash) {
    db_header_t *header = reinterpret_cast<db_header_t *>(location_);
    header->name_hash = name_hash;
    header->db_size = sizeof(db_header_t);
}

int HashDB::write_int(const uint32_t name_hash, const void *data, const uint32_t size) {
    db_header_t *header = reinterpret_cast<db_header_t *>(location_);
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

int HashDB::read_int(const uint32_t name_hash, void *data, const uint32_t size) {
    db_header_t *header = reinterpret_cast<db_header_t *>(location_);
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
