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

#ifndef __SERVER_CPP_H__
#define __SERVER_CPP_H__

#include <stdint.h>
#include <cstddef>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <vector>
#include <string>

namespace Network {

class MessageBase
{
public:
	virtual ~MessageBase() = default;

	MessageBase() = delete;

	/**
	 * @brief Constructor of MessageBase class.
	 *
	 * @param sockfd	Socket of connected client.
	 * @param data		Pointer to received data.
	 * @param size		Size of received data.
	 */
	MessageBase(int sockfd, char *data, size_t size)
		: _sockfd(sockfd), data(data), size(size) {}

	/**
	 * @brief Send reply to client.
	 *
	 * @param src		Pointer to the buffer to send.
	 * @param size		Amount of bytes to send.
	 */
	void Reply(const char *src, size_t size);

	/**
	 * @brief Send reply to client.
	 *
	 * @param msg		String message to send.
	 */
	void Reply(const std::string &msg);

	/**
	 * @brief Send reply to client.
	 *
	 * @param msg		Vector with data to send.
	 */
	void Reply(const std::vector<char> &msg);

	/**
	 * @brief Get pointer to received data.
	 *
	 * @return Pointer to data buffer.
	 */
	char *GetData() const;

	/**
	 * @brief Get size of received data.
	 *
	 * @return Size of data buffer.
	 */
	size_t GetSize() const;
private:
	int _sockfd;
	char *data;
	size_t size;
};

class ServerBase
{
public:
	enum Protocol { TCP, UDP };

	explicit ServerBase() = delete;
	~ServerBase();

	/**
	 * @brief Constructor of ServerBase class.
	 *
	 * @param port			Port number.
	 * @param msg_size		Maximum message size [default = 1500].
	 * @param max_connections	Maximum amount of incoming connections [32].
	 * @param protocol		Protocol type (TCP/UDP) [default = TCP].
	 */
	ServerBase(uint16_t port,
			   size_t msg_size = 1500,
			   size_t max_connections = 32,
			   Protocol protocol = Protocol::TCP);

	/**
	 * @brief Start server thread.
	 *
	 * Throws std::runtime_error on error.
	 */
	void Start();

	/**
	 * @brief Stop server thread.
	 */
	void Stop();

	/**
	 * @brief Get number of active connections.
	 *
	 * @return Amount of active incoming connections.
	 */
	size_t GetNumberOfClients();

	/**
	 * @brief Get maximum allowed connections.
	 *
	 * @return Maximum allowed connections.
	 */
	size_t GetMaxConnections() const;

	ServerBase (const ServerBase&) = delete;
	ServerBase (ServerBase&&) = delete;
	ServerBase operator=(ServerBase&) = delete;

	/** Event handlers to override */
	virtual void OnConnect(const std::string& ip) {}
	virtual void OnReceive(MessageBase &msg) = 0;
	virtual void OnDisconnect() {}
protected:
	uint16_t _port;
	size_t _msg_size;
	size_t _max_connections;
	Protocol _protocol;
	std::thread _server_thread;
	int _sockfd;
	std::mutex guard;
	std::unordered_set<int> clients_list;
};

class Client
{
public:
	explicit Client() = default;
	~Client();

	/**
	 * @brief Constructor of client class to connect to server.
	 * @param ip		IP address of server.
	 * @param port		Port number of server.
	 * @param protocol		Protocol type (TCP/UDP) [default = TCP].
	 */
	Client(const std::string& ip, uint16_t port,
		ServerBase::Protocol protocol = ServerBase::Protocol::TCP);

	/**
	 * @brief Send data to server.
	 *
	 * @param src		Pointer to the buffer to send.
	 * @param size		Amount of bytes to send.
	 * @return		0 on success, error code on error.
	 */
	void Send(const char *src, size_t size);

	/**
	 * @brief Send data to server.
	 *
	 * @param msg		String message to send.
	 */
	void Send(const std::string &msg);

	/**
	 * @brief Send data to server.
	 *
	 * @param msg		Vector with data to send.
	 */
	void Send(const std::vector<char> &msg);

	/**
	 * @brief Read data from server.
	 *
	 * @param dst		Pointer to the buffer to read data to.
	 * @param max_size	Amount of bytes to read;
	 * @return		Amount of bytes read.
	 */
	size_t Read(char *dst, size_t max_size = 1500);

	/**
	 * @brief Read data from server.
	 *
	 * @param out_str	String to store read data.
	 * @param max_size	Amount of bytes to read;
	 * @return		Amount of bytes read.
	 */
	size_t Read(std::string &out_str, size_t max_size = 1500);

	/**
	 * @brief Read data from server.
	 *
	 * @param max_size	Amount of bytes to read;
	 * @return		String with read data.
	 */
	std::string ReadString(size_t max_size = 1500);

	/**
	 * @brief Read data from server.
	 *
	 * @param max_size	Amount of bytes to read;
	 * @return		Vector with data.
	 */
	std::vector<char> ReadVector(size_t max_size = 1500);

	/**
	 * @brief Terminate current connection.
	 */
	void Close();
private:
	int _sockfd;
};

} /* namespace Network */

#endif /* __SERVER_CPP_H__ */
