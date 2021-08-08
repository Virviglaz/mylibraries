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

struct client_t {
	int socket;
	struct sockaddr_in cli_addr;
	server_event_t *ops;
	pthread_t thread;
	size_t size;
	void *user;
};

struct server_t {
	int socket;
	pthread_t thread;
	socklen_t clilen;
	server_event_t *ops;
	size_t size;
	void *user;
};

static void *client_handler(void *ptr)
{
	struct client_t *client = ptr;
	server_event_t *ops = client->ops;
	void *buffer = malloc(client->size);
	ssize_t size;

	if (!buffer) {
		if (ops->error)
			ops->error(no_mem_err, ENOMEM, client->user);
		goto err_nomem;
	}

	if (ops->connected && ops->connected(client))
		goto err_free;

	do {
		size = read(client->socket, buffer,
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

	return 0;
}

static void *server_handler(void *ptr)
{
	struct server_t *server = ptr;
	int clientfd;
	server_event_t *ops = server->ops;
	int res;

	listen(server->socket, LISTEN_BACKLOG_VALUE);

	do {
		client_t client = malloc(sizeof(*client));
		if (!client) {
			if (ops->error)
				ops->error(no_mem_err, ENOMEM, server->user);
			break;
		}

		clientfd = accept(server->socket,
			(struct sockaddr *)&client->cli_addr, &server->clilen);
		if (clientfd < 0) {
			res = errno;
			if (ops->error)
				ops->error("Socket accept error", res,
					server->user);
			free(client);
			break;
		}

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
			break;
		}

	} while (clientfd > 0 && server->socket > 0);

	close(server->socket);

	return 0;
}

server_t server_start(int port, server_event_t *ops, int size, void *user)
{
	server_t server = malloc(sizeof(*server));

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_ANY),
		.sin_port = htons(port),
	};
	int sockfd;
	int res;

	if (!ops)
		return 0;

	if (!server) {
		if (ops->error)
			ops->error(no_mem_err, ENOMEM, user);
		return 0;
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
	return 0;
}

int send_to_client(client_t client, void *msg, size_t size)
{
	int err = send(client->socket, msg, size, 0);

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
	free(server);

	return 0;
}
