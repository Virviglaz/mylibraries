#include "server.h"
#include <iostream>
#include <cstring>
#include <atomic>
#include <cassert>

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
		std::cout << "Received TCP data (" << msg.GetSize() << " bytes): "
		          << std::string(msg.GetData(), msg.GetSize()) << std::endl;
		std::string received_str(msg.GetData(), msg.GetSize());
		assert(received_str == message);
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
		std::cout << "Received UDP data (" << msg.GetSize() << " bytes): "
		          << std::string(msg.GetData(), msg.GetSize()) << std::endl;
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

int main(int argc, char *argv[])
{
	TCP_ServerTest tcp_server(8081, 1500, 32, ServerBase::Protocol::TCP);
	UDP_ServerTest udp_server(8082, 1500, 32, ServerBase::Protocol::UDP);

	tcp_server.Start();
	udp_server.Start();

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	Client tcp_client("127.0.0.1", 8081, ServerBase::Protocol::TCP);
	Client udp_client("127.0.0.1", 8082, ServerBase::Protocol::UDP);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	tcp_client.Send(message);
	udp_client.Send(message);

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	auto tcp_response = tcp_client.ReadString();
	std::cout << "TCP Server response: " << tcp_response << std::endl;
	inc_step++;

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	tcp_client.Close();
	udp_client.Close();

	tcp_server.Stop();
	udp_server.Stop();

	return inc_step == 8 ? 0 : 1;
}

