#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pid_linux.h"
#include <unistd.h>

struct pdata {
	double value;
	char *name;
};

static double pid_read(void *private_data)
{
	struct pdata *d = private_data;

	return d->value;
}

static void pid_write(PID_T value, void *private_data)
{
	struct pdata *d = private_data;

	d->value = value;

	printf("%s: %f\n", d->name, d->value);
}

static void *add_pid(char *name, int period, double target)
{
	struct pdata *p = malloc(sizeof(*p));

	p->name = name;
	p->value = 0;

	printf("Adding new PID %s\n", name);

	return create_pid(0.1, 0.1, 0.1, target, 0, 1000,
		pid_read, pid_write, p, period);
}

int main(void)
{
	void *pid = add_pid("PID_1", 50, 1000);
	if (!pid) {
		printf("PID create error!\n");
		return -1;
	}

	sleep(10);
	destroy_pid(pid);
	sleep(1);

	return 0;
}
