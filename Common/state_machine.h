#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

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

#endif /* __STATE_MACHINE_H__ */
