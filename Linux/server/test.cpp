#include "server.h"
#include <iostream>
#include <cstring>
#include <atomic>

static std::atomic<int> inc_step {0};

class ServerTest final: public ServerBase
{
public:
	using ServerBase::ServerBase;
	void OnConnect(const std::string& ip) override
	{
		std::cout << "Client connected: " << ip << std::endl;
		inc_step++;
	}

	void OnReceive(const char *data, size_t size) override
	{
		std::cout << "Received data (" << size << " bytes): "
		          << std::string(data, size) << std::endl;
		inc_step++;
	}

	void OnDisconnect() override
	{
		std::cout << "Client disconnected" << std::endl;
		inc_step++;
	}
};

int main(int argc, char *argv[])
{
	ServerTest server(8081);
	try
	{
		server.Start();
		std::cout << "Server started on port 8081" << std::endl;
	}
	catch (const std::runtime_error& e)
	{
		std::cerr << "Failed to start server: " << e.what() << std::endl;
		return 1;
	}

	Client client("127.0.0.1", 8081);
	
	const char *message = "Hello, Server!";
	client.Send(message, strlen(message));
	std::this_thread::sleep_for(std::chrono::seconds(2));
	client.Close();
	server.Stop();

	return inc_step == 3 ? 0 : 1;
}

