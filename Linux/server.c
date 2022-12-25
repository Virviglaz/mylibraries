#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "server.h"

#ifndef LISTEN_BACKLOG_VALUE
#define LISTEN_BACKLOG_VALUE			32
#endif

static const char *no_mem_err = "No memory";
static const char *thread_err = "Thread create error";

struct client {
	int socket;
	struct sockaddr_in cli_addr;
	server_event_t *ops;
	pthread_t thread;
	size_t size;
	void *user;
};

struct server {
	int socket;
	pthread_t thread;
	socklen_t clilen;
	server_event_t *ops;
	size_t size;
	void *user;
};

static void *client_handler(void *ptr)
{
	client_t client = (client_t)ptr;
	server_event_t *ops = client->ops;
	void *buffer = malloc(client->size);
	size_t size;

	if (!buffer) {
		if (ops->error)
			ops->error(no_mem_err, ENOMEM, client->user);
		goto err_nomem;
	}

	if (ops->connected && ops->connected(client))
		goto err_free;

	do {
		size = (size_t)read(client->socket, buffer,
			client->size);

		if (size && ops->receive && ops->receive(client, buffer, size))
			break;
	} while (size > 0);

err_free:
	free(buffer);
err_nomem:
	close(client->socket);

	if (ops->disconnected)
		ops->disconnected(client);

	free(client);

	pthread_detach(pthread_self());
	pthread_exit(0);
	return 0;
}

static void *server_handler(void *ptr)
{
	server_t server = (server_t)ptr;
	int clientfd;
	server_event_t *ops = server->ops;
	int res;

	res = listen(server->socket, LISTEN_BACKLOG_VALUE);
	if (res) {
		res = errno;
		if (ops->error)
                                ops->error("Socket listen error", res, server->user);
		goto err;
	}

	do {
		client_t client = malloc(sizeof(*client));
		if (!client) {
			res = ENOMEM;
			if (ops->error)
				ops->error(no_mem_err, ENOMEM, server->user);
			goto err;
		}

		clientfd = accept(server->socket,
			(struct sockaddr *)&client->cli_addr, &server->clilen);
		if (clientfd < 0) {
			res = errno;
			if (ops->error)
				ops->error("Socket accept error", res,
					server->user);
			free(client);
			goto err;
		}

#ifdef SOCKET_TIMEOUT
	{
		struct timeval timeout = {
			.tv_sec = SOCKET_TIMEOUT,
			.tv_usec = 0,
	};

		setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
		sizeof timeout);
		setsockopt(clientfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
		sizeof timeout);
	}
#endif /* SOCKET_TIMEOUT */

		client->ops = server->ops;
		client->user = server->user;
		client->socket = clientfd;
		client->size = server->size;

		res = pthread_create(&client->thread, NULL, client_handler,
			(void *)client);
		if (res) {
			if (ops->error)
				ops->error(thread_err, res, server->user);
			close(clientfd);
			free(client);
			goto err;
		}

	} while (clientfd > 0 && server->socket > 0);

err:
	close(server->socket);

	pthread_exit(&res);
	return 0;
}

server_t server_start(uint16_t port, server_event_t *ops, size_t size, void *user)
{
	server_t server = (server_t)malloc(sizeof(*server));

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port),
	};
	int sockfd;
	int res;

	if (!ops)
		return NULL;

	if (!server) {
		if (ops->error)
			ops->error(no_mem_err, ENOMEM, user);
		return NULL;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		res = errno;
		if (ops->error)
			ops->error("Open socket error", res, user);
		goto err_free;
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) {
		res = errno;
		if (ops->error)
			ops->error("Bind socked error", res, user);
		goto err_close;
	}

	server->socket = sockfd;
	server->ops = ops;
	server->size = size;
	server->user = user;

	res = pthread_create(&server->thread, NULL, server_handler,
		(void *)server);
	if (res) {
		if (ops->error)
			ops->error(thread_err, res, user);
		goto err_free;
	}

	return server;
err_free:
	free(server);
err_close:
	close(sockfd);
	return NULL;
}

int send_to_client(client_t client, void *msg, size_t size)
{
	int err = (int)send(client->socket, msg, size, 0);

	return err < 0 ? errno : 0;
}

const char *get_client_ip(client_t client)
{
	return inet_ntoa(client->cli_addr.sin_addr);
}

int get_client_port(client_t client)
{
	return ntohs(client->cli_addr.sin_port);
}

void *get_user_data(client_t client)
{
	return client->user;
}

int server_stop(server_t server)
{
	if (!server)
		return EINVAL;

	close(server->socket);

	return server_wait_for_err(server);
}

int server_wait_for_err(server_t server)
{
	int res;
	union {
		void *retval;
		int err;
	} ret;

        if (!server)
                return EINVAL;

	res = pthread_join(server->thread, &ret.retval);

	return res ? res : ret.err;
}

