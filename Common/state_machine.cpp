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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT
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
 * State machine C++ implementation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include "state_machine.h"

using namespace StateMachine;

int Machine::DoStep()
{
    if (mode_change_n_) {
        uint32_t next = current->enter(current->user_data_);
        if (next) {
            State* next_state = find_state_by_hash(next);
            if (!next_state)
                return -EINVAL;
            current = next_state;
            return 0;
        } else
            mode_change_n_ = false;
    }

    uint32_t next = current->work(current->user_data_);
    if (next) {
        State* next_state = find_state_by_hash(next);
        if (!next_state)
            return -EINVAL;

        // Transition to next state
        next = current->exit(current->user_data_);
        if (next) {
            next_state = find_state_by_hash(next);
            if (!next_state)
                return -EINVAL;
        }
        current = next_state;
        mode_change_n_ = true;
        return 0;
    }

    if (mode_change_n_) {
        uint32_t next = current->exit(current->user_data_);
        if (next) {
            State* next_state = find_state_by_hash(next);
            if (!next_state)
                return -EINVAL;
            current = next_state;
            return 0;
        }
    }

    return 0;
}

State *Machine::find_state_by_hash(uint32_t hash) const noexcept
{
    State* const *p = first;
    for (uint32_t i = 0; i < state_count_; i++, p++) {
        if ((*p)->state_hash_ == hash)
            return *p;
    }
    return nullptr;
}
