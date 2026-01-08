#include <iostream>
#include <cstring>
#include <atomic>
#include <cassert>
#include "server.h"
#include "rand.h"
#include "utils.h"

using namespace Network;
static std::atomic<int> inc_step {0};
static std::mutex cout_mutex;
static std::string message = "Hello, Server!";

class TCP_ServerTest final: public ServerBase
{
public:
	using ServerBase::ServerBase;
	void OnConnect(const std::string& ip) override
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "TCP Client connected: " << ip << std::endl;
		inc_step++;
	}

	void OnReceive(MessageBase &msg) override
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "Received TCP data " << msg.GetSize() << " bytes." << std::endl;
		inc_step++;
		msg.Reply("Hello, TCP Client!");
		inc_step++;
	}

	void OnDisconnect() override
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "TCP Client disconnected" << std::endl;
		inc_step++;
	}
};

class UDP_ServerTest final: public ServerBase
{
public:
	using ServerBase::ServerBase;
	void OnConnect(const std::string& ip) override
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "UDP Client connected: " << ip << std::endl;
		inc_step++;
	}

	void OnReceive(MessageBase &msg) override
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "Received UDP data " << msg.GetSize() << " bytes." << std::endl;
		inc_step++;
		std::string received_str(msg.GetData(), msg.GetSize());
		assert(received_str == message);
	}

	void OnDisconnect() override
	{
		std::lock_guard<std::mutex> lock(cout_mutex);
		std::cout << "UDP Client disconnected" << std::endl;
		inc_step++;
	}
};

std::vector<char> generate_data(size_t size)
{
	std::vector<char> data(size);

	BenchmarkTimer timer("Random data generation");
	for (size_t i = 0; i < size; ++i)
	{
		data[i] = static_cast<char>(tinymt32_generate() % 256);
	}
	return data;
}

int main(int argc, char *argv[])
{
	tinymt32_init(12345);

	auto data = generate_data(1024 * 1024); // 1 MB of random data

	TCP_ServerTest tcp_server(8081, data.size(), 32, ServerBase::Protocol::TCP);
	UDP_ServerTest udp_server(8082, 1500, 32, ServerBase::Protocol::UDP);

	tcp_server.Start();
	udp_server.Start();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	Client tcp_client("127.0.0.1", 8081, ServerBase::Protocol::TCP);
	Client udp_client("127.0.0.1", 8082, ServerBase::Protocol::UDP);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	tcp_client.Send(message);
	udp_client.Send(message);

	{
		BenchmarkTimer timer("TCP data send");
		tcp_client.Send(data);
	}

	{
		BenchmarkTimer timer("UDP data send");
		udp_client.Send(message);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	auto tcp_response = tcp_client.ReadString();
	std::cout << "TCP Server response: " << tcp_response << std::endl;
	inc_step++;

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	tcp_client.Close();
	udp_client.Close();

	tcp_server.Stop();
	udp_server.Stop();

	return inc_step > 10 ? 0 : 1;
}

