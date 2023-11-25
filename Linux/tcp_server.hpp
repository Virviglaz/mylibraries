/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2023 Pavel Nadein
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
 * TCP Server C++ implementation
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#ifndef __TCP_SERVER_HPP__
#define __TCP_SERVER_HPP__

#include <functional>
#include <stdint.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>

namespace TCP
{
	class Server
	{
		struct Event_ops;
	public:
		class Client
		{
		public:
			Client(int new_clientfd, struct Event_ops *ops) noexcept {
				clientfd = new_clientfd;
				event_ops = ops;
				client_thread = std::thread([&] {
					void *buffer = malloc(event_ops->buffer_size);
					int res = 0;
					ssize_t ret = 1;

					if (event_ops->on_connect)
						res |= event_ops->on_connect(*this);

					while (!res && ret > 0) {
						ret = read(clientfd, buffer, event_ops->buffer_size);
						if (ret > 0 && event_ops->on_receive)
							res |= event_ops->on_receive(*this, buffer, ret);
					};

					if (event_ops->on_disconnect)
						res |= event_ops->on_disconnect(*this);
					free(buffer);
					if (clientfd >= 0)
						close(clientfd);
					Terminate();
				});
				client_thread.detach();
			}


			int Send(void *buffer, ssize_t size) const noexcept {
				int res = send(clientfd, buffer, size, MSG_NOSIGNAL);
				return res < 0 ? errno : 0;
			}

			void Terminate() noexcept {
				if (clientfd >= 0)
					close(clientfd);
				delete(this);
			}

			std::string GetClientIP() const noexcept {
				struct sockaddr_in addr;
				socklen_t len = sizeof(addr);
				getpeername(clientfd, (struct sockaddr *) &addr, &len);
				char ip[16];
				inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
				return std::string(ip);
			}

			uint16_t GetClientPort() const noexcept {
				struct sockaddr_in addr;
				socklen_t len = sizeof(addr);
				getpeername(clientfd, (struct sockaddr *) &addr, &len);
				return addr.sin_port;
			}

			std::string GetSelfIP() const noexcept {
				struct sockaddr_in addr;
				socklen_t len = sizeof(addr);
				getsockname(clientfd, (struct sockaddr *) &addr, &len);
				char ip[16];
				inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
				return std::string(ip);
			}
		private:
			int clientfd = -1;
			struct Event_ops *event_ops;
			std::thread client_thread;
		};

		Server(uint16_t port,
		       size_t buffer_size = 1024,
		       const std::string& name = "",
		       std::function<int(const Client&)> on_connect = nullptr,
		       std::function<int(const Client&)> on_disconnect = nullptr,
		       std::function<int(const Client&, void *, ssize_t)> on_receive = nullptr,
		       std::function<void(const Client&, int)> on_error = nullptr,
		       int max_connections = 32) noexcept :
			       server_port(port),
			       server_name(name) {
			event_ops.on_connect = on_connect;
			event_ops.on_disconnect = on_disconnect;
			event_ops.on_receive = on_receive;
			event_ops.on_error = on_error;
			event_ops.buffer_size = buffer_size;
			max_conn = max_connections;
		}
		~Server() {
			Stop();
		}

		int Start() noexcept {
			struct sockaddr_in serv_addr;
			serv_addr.sin_port = htons(server_port);
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			int res = 0;

			serverfd = socket(AF_INET, SOCK_STREAM, 0);
			if (serverfd < 0)
				return errno;

			if (bind(serverfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
				res = errno;
				close(serverfd);
				return res;
			}

			server_thread = std::thread([&] {
				while (is_running.load(std::memory_order_acquire)
						&& listen(serverfd, max_conn) == 0) {
					int clientfd = accept(serverfd, 0, 0);
					if (clientfd >= 0) {
						Client *new_client = new Client(clientfd, &event_ops);
						(void)new_client;
						clientsfd_list.push_back(clientfd);
					}
				}
			});

			return res;
		}

		void Stop() noexcept {
			if (!is_running)
				return;
			is_running.store(false, std::memory_order_release);
			for (const int clientfd: clientsfd_list) {
				if (clientfd < 0)
					continue;
				shutdown(clientfd, SHUT_RD);
				close(clientfd);
			}
			shutdown(serverfd, SHUT_RD);
			close(serverfd);
		}

		void WaitForStop() noexcept {
			Stop();
			server_thread.join();
		}
	private:
		struct Event_ops
		{
			std::function<int(const Client&)> on_connect;
			std::function<int(const Client&)> on_disconnect;
			std::function<int(const Client&, void *, ssize_t)> on_receive;
			std::function<void(const Client&, int)> on_error;
			ssize_t buffer_size;
		} event_ops;
		uint16_t server_port = 0;
		int max_conn = 0;
		std::string server_name;
		int serverfd = -1;
		std::thread server_thread;
		std::vector<int> clientsfd_list;
		std::atomic<bool> is_running { true };
	};

	class Client
	{
		struct sockaddr_in addr;
		uint16_t client_port = 0;
		int sockfd = 0;
	public:
		Client(const std::string& ip, uint16_t port) noexcept {
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
		}

		Client(const char *ip, uint16_t port) noexcept {
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			inet_pton(AF_INET, ip, &addr.sin_addr);
		}

		~Client() {
			close(sockfd);
		}

		int Connect() noexcept {
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd < 0)
				return errno;

			int res = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
			if (res < 0) {
				close(sockfd);
				return errno;
			}
			return 0;
		}

		int Send(const void *data, ssize_t size) noexcept {
			return send(sockfd, data, size, 0) < 0 ? errno : 0;
		}

		ssize_t Read(void *data, ssize_t size) noexcept {
			return read(sockfd, data, size);
		}

		std::string GetServerIP() const noexcept {
			char ip[16];
			inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
			return std::string(ip);
		}

		std::string GetSelfIP() const noexcept {
			socklen_t len = sizeof(addr);
			getsockname(sockfd, (struct sockaddr *) &addr, &len);
			char ip[16];
			inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
			return std::string(ip);
		}

		uint16_t GetPort() const noexcept {
			return client_port;
		}
	};
}


#endif /* __TCP_SERVER_HPP__ */
