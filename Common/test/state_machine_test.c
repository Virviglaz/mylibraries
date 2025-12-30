#include "state_machine.h"
#include <assert.h>
#include <stdio.h>

static void validate_state(int step_num)
{
	static int current_step = 0;

	assert(step_num == current_step++);
}

static int st1_enter(void)
{
    printf("State 1 Enter Success\n");
    validate_state(0);
    return 0;
}

static int st1_work(void)
{
    printf("State 1 Work Success\n");
    validate_state(1);
    return 2;
}

static int st1_exit(void)
{
    printf("State 1 Exit Success\n");
    validate_state(2);
    return 0;
}

static int st2_enter(void)
{
    printf("State 2 Enter Success\n");
    validate_state(3);
    return 3;
}

static int st2_work(void)
{
    printf("State 2 Work Error\n");
    validate_state(0);
    return 3;
}

static int st2_exit(void)
{
    printf("State 2 Exit Success\n");
    validate_state(4);
    return 0;
}

static int st3_enter(void)
{
    printf("State 3 Enter Success\n");
    validate_state(5);
    return 0;
}

static int st3_work(void)
{
    printf("State 3 Work Success\n");
    validate_state(6);
    return 2;
}

static int st3_exit(void)
{
    printf("State 3 Exit Success\n");
    validate_state(7);
    return 1;
}

static machine_state_t states[] = {
    { st1_enter, st1_work, st1_exit },
    { st2_enter, st2_work, st2_exit },
    { st3_enter, st3_work, st3_exit },
};

int do_state_machine_c_test(void)
{
    static state_machine_t m;

    state_machine_init(&m, states);

    for (int i = 0; i != 3; ++i)
        state_machine_do_step(&m);

    return 0;
}
