#include <iostream>
#include <cassert>
#include "utils.h"

class TestTracer : public ClassTracer
{
public:
	TestTracer() : ClassTracer("TestTracer", true) {}
};

void func1(TestTracer t)
{
	t.PrintTestMessage("Pass by value to func1");
}

void func2(TestTracer &t)
{
	t.PrintTestMessage("Pass by reference to func2");
}

void func3(TestTracer &&t)
{
	t.PrintTestMessage("Pass by rvalue reference to func3");
}


int main(int argc, char *argv[])
{
	TestTracer tracer;
	func1(tracer);
	func2(tracer);
	func3(std::move(tracer));

	return 0;
}

