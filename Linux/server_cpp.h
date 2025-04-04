/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2024 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * C++ Posix server implementation.
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __SERVER_CPP_H__
#define __SERVER_CPP_H__

#include <cstring>
#include <thread>
#include <set>
#include <functional>
#include <arpa/inet.h>
#include <stdint.h>
#include <memory>
#include <vector>
#include <array>
#include <optional>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <cassert>

class Server;
class Client;
enum class Protocol { TCP, UDP };

/**
 * @brief Set of callback functions.
 * @note on_receive is mandatory.
 */
struct ServerEvents
{
	/** Called when client is connected. [Required] */
	std::function<void(const std::string& ip)> on_connect;

	/** Called when client is disconnected. [Optional] */
	std::function<void()> on_disconnect;

	/** Called when data received. [Optional] */
	std::function<void(Client&& client, char *data, ssize_t size)> on_receive;

	/** Called when on error. [Optional] */
	std::function<void(const char *err_str, int err)> on_error;
};

/**
 * Client class. Created internally, provided by the receive callback.
 */
class Client
{
	int _sockfd = -1;
	bool _close_required { false };
public:
	/**
	 * @brief Constructor of client class to connect to server.
	 */
	Client() = default;

	/**
	 * @brief Constructor used by Server when client is connected.
	 *
	 * @param sockfd	Socket of connected client.
	 */
	Client(int sockfd) : _sockfd(sockfd) {}

	~Client() { Close(); }

	/**
	 * @brief Send data back to client.
	 *
	 * @param src		Pointer to the buffer to send.
	 * @param size		Amount of bytes to send.
	 *
	 * @return		0 on success, error code on error.
	 */
	ssize_t Send(const char *src, ssize_t size) {
		return write(_sockfd, src, size) == size ? 0 : errno;
	}

	/**
	 * @brief Send data back to client.
	 *
	 * @param src		Source string.
	 *
	 * @return		0 on success, error code on error.
	 */
	ssize_t Send(const std::string& src) {
		return Send(src.c_str(), src.length() + 1);
	}

	/**
	 * @brief Send vector of data to client.
	 * @tparam T		Type of data (1-byte size).
	 * @param src		Source vector.
	 *
	 * @return		0 on success, error code on error.
	 */
	template <typename T = char>
	ssize_t Send(const std::vector<T>& src) {
		return Send(static_cast<const char *>(src.begin()), src.size());
	}

	/**
	 * @brief Send array of bytes to client.
	 *
	 * @tparam T		Type of data (1-byte size).
	 * @tparam S		Amount of bytes to send.
	 * @param src		Source of byte array.
	 *
	 * @return		0 on success, error code on error.
	 */
	template <typename T = char, std::size_t S>
	ssize_t Send(const std::array<T, S>& src) {
		return Send(static_cast<const char *>(src.begin()), src.size());
	}

	/**
	 * @brief Connect to remote host by IP address.
	 *
	 * @param ip		IP address string { xxx.xxx.xxx.xxx }.
	 * @param port		Post number.
	 * @param proto		Protocol [TCP/UDP].
	 *
	 * @return		0 on success, error code on error.
	 */
	int Connect(	const std::string& ip,
			uint16_t port,
			Protocol proto = Protocol::TCP) {
		_sockfd = socket(AF_INET,
			proto == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
		if (_sockfd < 0)
			return errno;

		struct sockaddr_in serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);

		if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
			Close();
			return errno;
		}

		if (connect(_sockfd, (struct sockaddr *)&serv_addr,
				sizeof(serv_addr)) < 0) {
			Close();
			return errno;
		}

		_close_required = true;
		return 0;
	}

	/**
	 * @brief Wait and read data from remote host.
	 *
	 * @param max_size	Amount of bytes to read;
	 *
	 * @return Vector with data.
	 */
	std::vector<char> Read(ssize_t max_size = 1500) {
		assert(max_size > 0);
		char buffer[max_size];
		ssize_t s = read(_sockfd, buffer, max_size);
		if (s <= 0)
			return std::vector<char>{ };
		return std::vector<char> { buffer, buffer + s };
	}

	/**
	 * @brief Terminate current connection.
	 */
	void Close() {
		if (_close_required && _sockfd >= 0) {
			close(_sockfd);
			_sockfd = -1;
			_close_required = false;
		}
	}
};

class Server
{
public:
	/**
	 * @brief C++ POSIX Server.
	 *
	 * @param port			Port number.
	 * @param events		List of callback.
	 * @param msg_size		Maximum message size [default = 1500].
	 * @param max_connections	Maximum amount of incoming connections [32].
	 */
	Server(	uint16_t port,
		ServerEvents events,
		std::size_t msg_size = 1500,
		int max_connections = 32,
		Protocol protocol = Protocol::TCP) :
		_port(port),
		_events(std::move(events)),
		_msg_size(msg_size),
		_max_connections(max_connections),
		_protocol(protocol) {
		assert(_max_connections > 0);
		assert(port > 0);
		assert(!events.on_receive);
		assert(msg_size > 0);
	}

	~Server() { Stop(); }

	/**
	 * @brief Start server thread.
	 *
	 * @return 0 on success, error code on error.
	 */
	int Start() {
		/* Check instance is already running */
		if (_sockfd.has_value()) {
			int res = EALREADY;
			if (_events.on_error)
				_events.on_error("Socket already opened", res);
			return res;
		}

		/* Register socket */
		_sockfd = socket(AF_INET,
				_protocol == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
		if (_sockfd.value() < 0) {
			int res = errno;
			if (_events.on_error)
				_events.on_error("Open socket error", res);
			_sockfd.reset();
			return res;
		}

		/* Bind socket */
		struct sockaddr_in serv_addr;
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_port = htons(_port);

		if (bind(_sockfd.value(), (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0) {
			int res = errno;
			if (_events.on_error)
				_events.on_error("Bind socked error", res);
			close_socket(_sockfd);
			return res;
		}

		/* Start listening for incoming connections. */
		if (_protocol == Protocol::TCP) {
			int res = listen(_sockfd.value(), _max_connections);
			if (res) {
				res = errno;
				if (_events.on_error)
					_events.on_error("Socket listen error", res);
				return res;
			}
		}

		/* Start server thread */
		_server_thread = std::thread(server_handler, this);

		return 0;
	}



	/**
	 * @brief Stop server thread.
	 *
	 * @return 0 on success, error code on error.
	 */
	int Stop() {

		if (_sockfd.has_value() == false) /* Not started */
			return 0;

		unregister_clients(this);

		/* Terminate server socket */
		close_socket(_sockfd);

		/* Wait server terminated */
		_server_thread.join();

		_sockfd.reset();

		return 0;
	}

	/**
	 * @brief Get number of active connections.
	 *
	 * @return Amount of active incoming connections.
	 */
	auto CheckConnections() {
		guard.lock();
		auto ret = clients_list.size();
		guard.unlock();
		return ret;
	}


	Server (const Server&) = delete;
	Server operator=(Server) = delete;

private:
	uint16_t _port;
	ServerEvents _events;
	std::size_t _msg_size;
	std::thread _server_thread;
	std::optional<int> _sockfd;
	int _max_connections;
	std::set<int> clients_list;
	std::mutex guard;
	Protocol _protocol;

	static void close_socket(std::optional<int>& socket_fd) {
		if (socket_fd.has_value())
			close_socket(socket_fd.value());
	}

	static void close_socket(int socket_fd) {
		shutdown(socket_fd, SHUT_RD);
	}

	static int server_handler(Server *server) {
		struct sockaddr_in cli_addr;

		/* Accept the connection and create client (called in the loop) */
		while (server->_protocol == Protocol::TCP) {
			socklen_t socklen;
			int clientfd = accept(server->_sockfd.value(),
					(struct sockaddr *)&cli_addr, &socklen);

			/* Socket failed due to termination */
			if (clientfd < 0)
				break;

			/* Get IP address */
			std::string ip { inet_ntoa(cli_addr.sin_addr) };

			/* Call event when connected */
			if (server->_events.on_connect)
				server->_events.on_connect(ip);

			/* Register client */
			server->guard.lock();
			server->clients_list.insert(clientfd);
			server->guard.unlock();

			if (server->_max_connections == 1) {
				/* For single connection use this thread */
				client_handler(server, clientfd);
			} else {
				/* Create client thread */
				std::thread(client_handler, server, clientfd).detach();
			}
		}

		while (server->_protocol == Protocol::UDP) {
			char buffer[server->_msg_size];
			socklen_t si_client_len = sizeof(cli_addr);
			auto size = recvfrom(server->_sockfd.value(),
					buffer, server->_msg_size, 0,
					(struct sockaddr *)&cli_addr,
					&si_client_len);

			if (size > 0) {
				std::string ip { inet_ntoa(cli_addr.sin_addr) };

				/* Call event when connected */
				if (server->_events.on_connect)
					server->_events.on_connect(ip);

				server->_events.on_receive(
					std::move(Client(server->_sockfd.value())),
					buffer, size);
			} else {
				if (server->_events.on_disconnect)
					server->_events.on_disconnect();
				break;
			}
		}

		unregister_clients(server);

		return 0;
	}

	static void unregister_clients(Server *server) {
		/* Unregister all clients */
		server->guard.lock();
		for (const auto& client: server->clients_list)
			close_socket(client);
		server->guard.unlock();
	}

	static void client_handler(Server *server, int sockfd) {
		char buffer[server->_msg_size];

		while (true) {
			auto size = read(sockfd, buffer, server->_msg_size);
			if (size <= 0)
				break;

			server->_events.on_receive(
					std::move(Client(sockfd)),
					buffer, size);
		}

		/* Client is terminated, remove from list */
		server->guard.lock();
		server->clients_list.erase(sockfd);
		server->guard.unlock();

		/* Gently close */
		close(sockfd);

		if (server->_events.on_disconnect)
			server->_events.on_disconnect();
	}
};

#endif /* __SERVER_CPP_H__ */
