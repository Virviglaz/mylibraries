/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2026 Pavel Nadein
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

#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdexcept>
#include "server.h"

ServerBase::ServerBase(uint16_t port, size_t msg_size, size_t max_connections, Protocol protocol)
	: _port(port), _msg_size(msg_size), _max_connections(max_connections), _protocol(protocol)
{
}

ServerBase::~ServerBase()
{
	Stop();
}

class ServerInternal final: public ServerBase
{
public:
	using ServerBase::ServerBase;

	void AddClient(int sockfd)
	{
		guard.lock();
		clients_list.insert(sockfd);
		guard.unlock();
	}

	void RemoveClient(int sockfd)
	{
		guard.lock();
		clients_list.erase(sockfd);
		guard.unlock();
	}

	int AcceptClient()
	{
		if (_sockfd < 0)
			return -1;

		socklen_t socklen;
		struct sockaddr_in cli_addr;
		int clientfd = accept(_sockfd, (struct sockaddr *)&cli_addr, &socklen);

		return clientfd;
	}

	std::string GetClientIP(int sockfd)
	{
		socklen_t socklen;
		struct sockaddr_in cli_addr;
		getpeername(sockfd, (struct sockaddr *)&cli_addr, &socklen);
		return std::string{ inet_ntoa(cli_addr.sin_addr) };
	}

	size_t GetMaxMsgSize() const
	{
		return _msg_size;
	}

	void CloseAllClients()
	{
		guard.lock();
		for (const auto& client: clients_list)
			close(client);
		guard.unlock();
	}
};

static void client_handler(ServerBase *server, int sockfd)
{
	ServerInternal *srv = static_cast<ServerInternal *>(server);

	char *buffer = new char[srv->GetMaxMsgSize()];
	if (!buffer)
	{
		/* Memory error, close connection */
		srv->RemoveClient(sockfd);
		close(sockfd);
		return;
	}

	while (true)
	{
		auto size = read(sockfd, buffer, srv->GetMaxMsgSize());
		if (size <= 0)
			break;

		server->OnReceive(buffer, size);
	}

	/* Client is terminated, remove from list */
	srv->RemoveClient(sockfd);

	/* Gently close */
	close(sockfd);

	server->OnDisconnect();
	delete[] buffer;
}

static void tcp_handler(ServerBase *server)
{
	ServerInternal *srv = static_cast<ServerInternal *>(server);

	while (true)
	{
		int clientfd = srv->AcceptClient();

		/* Socket failed due to termination */
		if (clientfd < 0)
			return;

		/* Call event when connected */
		server->OnConnect(srv->GetClientIP(clientfd));

		size_t num_clients = server->GetNumberOfClients();

		if (num_clients >= server->GetMaxConnections())
		{
			/* Too many connections, close socket */
			close(clientfd);
			continue;
		}

		/* Register client */
		srv->AddClient(clientfd);

		if (num_clients == 1)
		{
			/* For single connection use this thread */
			client_handler(server, clientfd);
		}
		else
		{
			/* Create client thread */
			std::thread(client_handler, server, clientfd).detach();
		}
	};
}

static void udp_handler(ServerBase *server)
{
	throw std::runtime_error("UDP protocol is not implemented yet");
}

void ServerBase::Start()
{
	_sockfd = socket(AF_INET, _protocol == Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
	if (_sockfd < 0)
		throw std::runtime_error(
			"ServerBase::Start: Open socket error: " + std::to_string(errno));

	/* Bind socket */
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(_port);

	if (bind(_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		close(_sockfd);
		throw std::runtime_error(
			"ServerBase::Start: Bind socket error: " + std::to_string(errno));
	}

	/* Start listening for incoming connections. */
	if (_protocol == Protocol::TCP)
	{
		int res = listen(_sockfd, _max_connections);
		if (res)
		{
			close(_sockfd);
			throw std::runtime_error(
				"ServerBase::Start: Socket listen error: " + std::to_string(errno));
		}
	}

	/* Start server thread */
	_server_thread = std::thread(_protocol == Protocol::TCP ? tcp_handler : udp_handler, this);
}

void ServerBase::Stop()
{
	/* Close all client connections */
	ServerInternal *srv = static_cast<ServerInternal *>(this);
	srv->CloseAllClients();

	/* Close socket to terminate server thread */
	if (_sockfd >= 0)
	{
		shutdown(_sockfd, SHUT_RD);
		_sockfd = -1;
	}

	/* Wait for server thread termination */
	if (_server_thread.joinable())
		_server_thread.join();
}

size_t ServerBase::GetNumberOfClients()
{
	std::lock_guard<std::mutex> lock(guard);
	return clients_list.size();
}

size_t ServerBase::GetMaxConnections() const
{
	return _max_connections;
}

Client::Client(const std::string& ip, uint16_t port, ServerBase::Protocol protocol)
{
	_sockfd = socket(AF_INET, protocol == ServerBase::Protocol::TCP ? SOCK_STREAM : SOCK_DGRAM, 0);
	if (_sockfd < 0)
		throw std::runtime_error(
			"Client::Client: Open socket error: " + std::to_string(errno));

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
	{
		close(_sockfd);
		throw std::runtime_error(
			"Client::Client: Invalid address/ Address not supported: " + ip);
	}

	if (connect(_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		close(_sockfd);
		throw std::runtime_error(
			"Client::Client: Connection Failed: " + std::to_string(errno));
	}
}

Client::~Client()
{
	Close();
}

void Client::Send(const char *src, size_t size)
{
	ssize_t sent = write(_sockfd, src, size);
	if (sent < 0 || static_cast<size_t>(sent) != size)
		throw std::runtime_error(
			"Client::Send: Send failed: " + std::to_string(errno));
}

size_t Client::Read(char *dst, size_t max_size)
{
	ssize_t received = read(_sockfd, dst, max_size);
	if (received < 0)
		throw std::runtime_error(
			"Client::Read: Read failed: " + std::to_string(errno));
	return static_cast<size_t>(received);
}

void Client::Close()
{
	if (_sockfd >= 0)
	{
		close(_sockfd);
		_sockfd = -1;
	}
}