#include <string>
#include <jsoncpp/json/json.h>
#include <list>
#include <memory>
#include <future>
#include <thread>
#include <chrono>
#include <mutex>
#include "file_ops.h"
#include "server.h"

#ifndef MAX_MESSAGE_SIZE
#define MAX_MESSAGE_SIZE 1500
#endif

const std::string config_path = "config.json";

class LoggerServer final : public Network::ServerBase
{
public:
	explicit LoggerServer(const std::string &name,
						  uint16_t port,
						  Protocol protocol,
						  const std::string &log_file_path,
						  uint32_t magic_number)
		: ServerBase(port, MAX_MESSAGE_SIZE, 32, protocol), _name(name), magic_number(magic_number)
	{
		log_file = std::make_unique<File>(log_file_path.c_str(), O_CREAT | O_WRONLY | O_APPEND);
	}

	void OnReceive(Network::MessageBase &msg) override
	{
		struct {
			uint32_t magic;
			char data[MAX_MESSAGE_SIZE - sizeof(uint32_t)];
		} __attribute__((packed)) *received_msg = reinterpret_cast<decltype(received_msg)>(msg.GetData());

		if (received_msg->magic != magic_number) {
			std::cerr << "Received message with invalid magic number: " << received_msg->magic << std::endl;
			return;
		}

		std::cout << "Received data: " << received_msg->data << std::endl;
		{
			std::lock_guard<std::mutex> lock(log_mutex);
			std::string timestamp = get_time();
			log_file->Write(timestamp.c_str(), timestamp.size()).Write(": ", 2).Write(received_msg->data, msg.GetSize() - sizeof(received_msg->magic)).Write("\r\n", 2).Sync();
		}

		if (_protocol == Protocol::TCP)
			msg.Reply("Data received");
	}
private:
	static std::string get_time(std::time_t time = std::time(nullptr))
	{
		char timeString[20];
		std::strftime(timeString, sizeof(timeString), "%F %T", std::localtime(&time));

		return std::string(timeString);
	}

	std::string _name;
	std::unique_ptr<File> log_file;
	uint32_t magic_number;
	std::mutex log_mutex;
};

/**
 * Test main function that reads config, starts logger servers, and waits indefinitely.
 * To run the test, execute the program with the argument "--test" to start the servers
 * and then exit after a short delay. Otherwise, it will wait indefinitely for incoming messages.
 */
int main(int argc, char *argv[])
{
	std::list<std::unique_ptr<LoggerServer>> servers;
	Json::Value root;
	Json::CharReaderBuilder reader_builder;
	std::unique_ptr<Json::CharReader> reader(reader_builder.newCharReader());
	std::string errs;

	/* Read and parse config file */
	{
		std::string config_content = File(config_path.c_str(), O_RDONLY).Read();
		if (!reader->parse(config_content.c_str(), config_content.c_str() + config_content.size(), &root, &errs)) {
			std::cerr << "Failed to parse config: " << errs << std::endl;
			return 1;
		}
	}
	auto loggers = root["Loggers"];

	for (const auto &logger : loggers) {
		std::string name = logger["Name"].asString();
		std::uint32_t magic_number = logger["MagicNumber"].asUInt();
		std::string protocol = logger["Protocol"].asString();
		std::uint16_t port = static_cast<uint16_t>(logger["Port"].asUInt());
		std::string log_file = logger["FilePath"].asString();
		Network::ServerBase::Protocol proto = protocol == "TCP" ? Network::ServerBase::Protocol::TCP : Network::ServerBase::Protocol::UDP;

		std::cout << "Logger: " << name << " | Port: " << port << " | Protocol: " << protocol << " | MagicNumber: " << magic_number << " | LogFile: " << log_file << std::endl;
		servers.emplace_back(std::make_unique<LoggerServer>(name, port, proto, log_file, magic_number));
	}

	for (auto &server : servers) {
		try
		{
			server->Start();
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error starting server: " << e.what() << '\n';
		}
	}

	if (argc > 1 && std::string(argv[1]) == "--test") {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	} else {
		std::promise<void>().get_future().wait();
	}

	return 0;
}
