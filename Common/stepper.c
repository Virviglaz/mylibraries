int do_step(double speed, double acc, double freq)
{
	static double c, set_speed = 0;
	static int n;

	if (speed && acc && freq) {
		set_speed = speed;
		c = freq * sqrt(2 * speed / acc);
		n = 1;
	}
	else
		c = c - 2 * c / (4 * n++ + 1);

	printf("n = %d\tc = %0.0f\t\t%0.0f\n", n, c, 1000000 / c);
	return n;
}

int main(void)
{
	int i;
	
	do_step(100, 10000, 1000000);

	for (i = 0; i != 200; i++)
		do_step(0, 0, 0);
}
