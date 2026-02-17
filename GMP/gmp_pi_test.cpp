#include <stdio.h>
#include <gmp.h>
#include "utils.h"

class Pi
{
public:
	Pi(unsigned long precision) : _precision(precision)
	{
		mpf_set_default_prec(precision);
		mpf_init(pi);
		mpf_init(denominator);
		mpf_set_ui(denominator, 1);
		mpf_init(tmp);
	}

	~Pi()
	{
		mpf_clear(pi);
		mpf_clear(denominator);
		mpf_clear(tmp);
	}

	Pi &iterate()
	{
		mpf_ui_div(tmp, 4, denominator);

		if (i % 2 == 0) {
			mpf_add(pi, pi, tmp);
        } else {
            mpf_sub(pi, pi, tmp);
        }

		mpf_add_ui(denominator, denominator, 2);
		i++;

		return *this;
	}

	Pi &calculate()
	{
		for (unsigned long j = 0; j < _precision; ++j)
		{
			iterate();
		}

		return *this;
	}

	Pi &print()
	{
		gmp_printf("Pi is: %.Ff\n", pi);

		return *this;
	}
private:
	mpf_t pi;
	mpf_t denominator;
	mpf_t tmp;
	unsigned long i = 0;
	unsigned long _precision;
};

int main()
{
	BenchmarkTimer b("Pi Calculation", true);
	Pi(10000).calculate().print();

	return 0;
}