/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2023-2025 Pavel Nadein
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
 * State machine C/C++ implementation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#ifdef __cplusplus

#include <cstdint>
#include <cerrno>

namespace StateMachine
{

/**
 * @brief Defines machine state.
 *
 * @note Inherit this class to define specific states.
 * Each state must have unique state_hash value.
 * Override enter, work and exit methods to define state behavior.
 * Each method must return next state hash or 0 to stay in current state.
 */
class State
{
public:
	/** @brief Constructor.
	 *
	 * @param state_hash Unique state hash.
	 * @param user_data User data associated with this state.
	*/
	State(uint32_t state_hash, void *user_data) :
		state_hash_(state_hash),
		user_data_(user_data) {}
	virtual uint32_t enter(void *args) { return 0; }
	virtual uint32_t work(void *args)  { return EINVAL; }
	virtual uint32_t exit(void *args)  { return 0; }

	/* allow Machine to access protected members directly */
	friend class Machine;

	/** Delete copy and move constructors and assignment operators */
	State (const State&) = delete;
	State& operator= (const State&) = delete;
	State (State&&) = delete;
	State& operator= (State&&) = delete;

protected:
	uint32_t state_hash_;
	void *user_data_;
};

/**
 * @brief Defines state machine.
 */
class Machine
{
public:
	/**
	 * @brief Constructor.
	 *
	 * @tparam N Number of states in the array.
	 * @param states Array of state pointers.
	 */
	template <uint32_t N>
	explicit Machine(State* (&states)[N])
		: state_count_(N),
		  first(&states[0]),
		  current(states[0]),
		  mode_change_n_(true)
	{}

	/**
	 * @brief Perform single step of state machine.
	 *
	 * @return 0 on success, negative errno code on failure.
	 */
	int DoStep()
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

	/** Delete copy and move constructors and assignment operators */
	Machine (const Machine&) = delete;
	Machine& operator= (const Machine&) = delete;
	Machine (Machine&&) = delete;
	Machine& operator= (Machine&&) = delete;

private:
	// first now points to an array of State* elements
	State* const *find_first_ptr() const noexcept { return first; }

	State *find_state_by_hash(uint32_t hash) const noexcept
	{
		State* const *p = first;
		for (uint32_t i = 0; i < state_count_; i++, p++) {
			if ((*p)->state_hash_ == hash)
				return *p;
		}
		return nullptr;
	}

	const uint32_t state_count_;
	State* const *first;   // pointer to first element of State* array
	State* current;
	bool mode_change_n_;
};

};

#else /* __cplusplus */

/** C implementation */

/**
 * @brief Defines machine state.
 */
typedef struct {
	/** @brief Enter mode callback */
	int (*enter)(void);

	/** @brief Execute mode callback */
	int (*work)(void);

	/** @brief Exit mode callback */
	int (*exit)(void);
} machine_state_t;

/**
 * @brief Defines state machine.
 *
 * @Note Keep state_n and mode_change_n initialized with zeroes.
 */
typedef struct {
	/** @brief States list. */
	machine_state_t *states;

	/** @brief Number of current state */
	int state_n;

	/** @brief Mode change flag. */
	int mode_changed_n;
} state_machine_t;

void state_machine_init(
	state_machine_t *instance,
	machine_state_t *states_list);

/**
 * @brief Perform single step of state machine.
 *
 * @param instance Pointer to current state machine.
 */
void state_machine_do_step(state_machine_t *instance);

#endif /* __cplusplus */
#endif /* __STATE_MACHINE_H__ */
