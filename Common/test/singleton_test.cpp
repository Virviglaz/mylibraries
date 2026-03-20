#include "singleton.h"
#include <thread>
#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <vector>

class Tracker
{
public:
	std::string name;

	Tracker() = default;

	explicit Tracker(std::string n) : name(std::move(n))
	{
		std::cout << "[Created]  " << name << "\n";
	}

	~Tracker()
	{
		std::cout << "[Destroyed] " << name << "\n";
		name = {};
	}

	Tracker(const Tracker &other) : name(other.name + " (copy)")
	{
		std::cout << "[Copied]   " << other.name << " -> " << name << "\n";
	}

	Tracker(Tracker &&other) noexcept : name(std::move(other.name))
	{
		other.name = "empty";
		std::cout << "[Moved]    " << name << "\n";
	}

	Tracker &operator=(const Tracker &other)
	{
		if (this != &other)
		{
			std::cout << "[Assigned Copy] " << other.name << " to " << name << "\n";
			name = other.name + " (copy)";
		}
		return *this;
	}
};

class SingletonTest : public SingletonBase<SingletonTest>, public Tracker
{
public:
	SingletonTest(Token) : Tracker("Singleton")
	{
		std::cout << "SingletonTest initialized" << std::endl;
	}

	~SingletonTest()
	{
		std::cout << "SingletonTest destroyed" << std::endl;
	}

	void DoSome(const std::string &work)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::cout << work << std::endl;
	}
};

#define MAX_THREADS	10

int do_signleton_test()
{
	auto ptr = SingletonTest::getInstance();

	std::vector<std::thread> tasks;
	tasks.reserve(MAX_THREADS);

	for (int i = 0; i != MAX_THREADS; i++)
	{
		tasks.emplace_back(std::thread([](const std::string &s)
									   { SingletonTest::getInstance()->DoSome(s); }, std::to_string(i)));
	};

	for (auto &t : tasks)
		t.join();

	ptr->DoSome("Last");

	return 0;
}
