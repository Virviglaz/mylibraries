/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2023 Pavel Nadein
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
 * State machine C implementation
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#ifdef __cplusplus
#include <functional>
#include <vector>
#include <stdbool.h>

class StateMachine; /* Forward declaration */

/**
 * @brief Defines single state item.
 */
class StateItem
{
	std::function<void(StateMachine *)> _work;
	std::function<void(StateMachine *)> _enter;
	std::function<void(StateMachine *)> _exit;

public:
	StateItem(std::function<void(StateMachine *)> work,
		std::function<void(StateMachine *)> enter = nullptr,
		std::function<void(StateMachine *)> exit = nullptr) :
		_work(work),
		_enter(enter),
		_exit(exit) {}

	void do_work(StateMachine *m) {
		_work(m);
	}

	void do_enter(StateMachine *m) {
		if (_enter)
			_enter(m);
	}

	void do_exit(StateMachine *m) {
		if (_exit)
			_exit(m);
	}
};

/**
 * @brief State machine. Using list of StateItem.
 */
class StateMachine
{
	std::vector<StateItem> list;
	unsigned int current = 0;
	bool change_state = true;
public:
	StateMachine() {}

	StateMachine(std::vector<StateItem> states) :
		list(states) {}

	void switch_state(unsigned int state_num) {
		if (state_num <= list.size()) {
			current = state_num;
			change_state = true;
		}
	}

	void run() {
		unsigned int state_num = current;
		if (change_state) {
			change_state = false;
			list[state_num].do_enter(this);
		}

		if (change_state)
			return;

		list[state_num].do_work(this);

		if (change_state)
			list[state_num].do_exit(this);
	}
};

#else /* __cplusplus */

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
	/** @brief Number of current state */
	int state_n;

	/** @brief Mode change flag. */
	int mode_change_n;

	/** @brief States list. */
	machine_state_t states[];
} state_machine_t;

/**
 * @brief Perform single step of state machine.
 *
 * @param instance Pointer to current state machine.
 */
void state_machine_do_step(state_machine_t *instance);

#endif /* __cplusplus */
#endif /* __STATE_MACHINE_H__ */
