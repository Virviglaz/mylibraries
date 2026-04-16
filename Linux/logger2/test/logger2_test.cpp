#include "server.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE 1500
#endif

int main()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::cout << "Starting Logger2 test..." << std::endl;
	Network::Client clientTCP("127.0.0.1", 7000, Network::ServerBase::Protocol::TCP);
	Network::Client clientUDP("127.0.0.1", 8000, Network::ServerBase::Protocol::UDP);

	struct
	{
		uint32_t magic;
		char data[MAX_MESSAGE_SIZE - sizeof(uint32_t)];
	} __attribute__((packed)) msg_to_send = {12345, "Hello, Logger2!"};

	clientTCP.Send(reinterpret_cast<const char *>(&msg_to_send), 20);
	auto response = clientTCP.ReadString();
	std::cout << "Received response: " << response << std::endl;

	msg_to_send.magic = 67890;
	clientUDP.Send(reinterpret_cast<const char *>(&msg_to_send), 20);

	return response == "Data received" ? 0 : 1;
}
