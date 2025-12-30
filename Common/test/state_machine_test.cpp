#include "state_machine.h"
#include <iostream>
#include <cassert>
#include <hash.h>

static void validate_state(int step_num)
{
	static int current_step = 0;

	assert(step_num == current_step++);
}

int do_state_machine_test()
{
	using namespace StateMachine;

	class TestState1 : public State
	{
	public:
		TestState1() : State(Hash("TestState1")) {}

		uint32_t enter(void *user_data) override
		{
			std::cout << "Entering State 1 Success" << std::endl;
			validate_state(0);
			return Hash("TestState2"); // Transition to State 2
		}

		uint32_t work(void *user_data) override
		{
			std::cout << "Working in State 1 Error" << std::endl;
			return 1; // Error to trigger exit
		}

		uint32_t exit(void *user_data) override
		{
			std::cout << "Exiting from State 1 Error" << std::endl;
			return 1; // Error to trigger exit
		}
	};

	class TestState2 : public State
	{
	public:
		TestState2() : State(Hash("TestState2")) {}

		uint32_t enter(void *user_data) override
		{
			std::cout << "Entering State 2 Success" << std::endl;
			validate_state(1);
			return 0;
		}

		uint32_t work(void *user_data) override
		{
			std::cout << "Working in State 2 Success" << std::endl;
			validate_state(2);
			return Hash("TestState3"); // Transition to State 3
		}

		uint32_t exit(void *user_data) override
		{
			std::cout << "Exiting State 2 Success" << std::endl;
			validate_state(3);
			return 0;
		}
	};

	class TestState3 : public State
	{
	public:
		TestState3() : State(Hash("TestState3")) {}

		uint32_t enter(void *user_data) override
		{
			std::cout << "Entering State 3 Success" << std::endl;
			validate_state(4);
			return 0;
		}

		uint32_t work(void *user_data) override
		{
			std::cout << "Working in State 3 Success" << std::endl;
			validate_state(5);
			return 0;
		}

		uint32_t exit(void *user_data) override
		{
			std::cout << "Exiting State 3 Success" << std::endl;
			return Hash("TestState1"); // Transition back to State 1
		}
	};

	// instantiate concrete states and pass an array of State* (Machine expects State*[])
	TestState1 s1;
	TestState2 s2;
	TestState3 s3;

	State *states[] = {&s1, &s2, &s3};

	Machine sm(states);

	for (int i = 0; i < 3; ++i)
	{
		int ret = sm.DoStep();
		assert(ret == 0);
	}

	return 0;
}
