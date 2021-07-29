#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include "server.h"

struct client_t {
	int socket;
	srv_handler handler;
	pthread_t *thread;
	int size;
	int pooling_int_us;
};

struct server_t {
	int socket;
	pthread_t *thread;
	socklen_t clilen;
	srv_handler handler;
	int size;
	int pooling_int_us;
};

static void *client_handler(void *ptr)
{
	struct client_t *client = ptr;
	char *buffer;
	int size;
	int err;
	socklen_t len = sizeof(err);

	buffer = malloc(client->size);
	if (!buffer)
		goto err_nomem;

	do {
		if (getsockopt(client->socket, SOL_SOCKET, SO_ERROR,
			&err, &len))
			break;

		size = read(client->socket, buffer, client->size);
		if (size)
			client->handler(buffer, size, client->socket);
		usleep(client->pooling_int_us);

	} while (!err);

	free(buffer);
err_nomem:
	close(client->socket);
	free(client->thread);
	free(client);

	return 0;
}

static void *server_handler(void *ptr)
{
	struct server_t *server = ptr;
	int clientfd;

	listen(server->socket, 10);
	do {
		struct client_t *clent;
		struct sockaddr_in cli_addr = { 0 };
		clientfd = accept(server->socket, (struct sockaddr *) &cli_addr,
			&server->clilen);

		clent = malloc(sizeof(*clent));
		if (!clent)
			break;

		clent->handler = server->handler;
		clent->socket = clientfd;
		clent->size = server->size;
		clent->pooling_int_us = server->pooling_int_us * 1000;
		clent->thread = malloc(sizeof(pthread_t));
		if (!clent->thread)
			break;

		if (pthread_create(clent->thread, NULL,
			client_handler, (void *)clent))
			break;

	} while (clientfd > 0 && server->socket > 0);
	close(server->socket);

	return 0;
}

server_t server_start(int port, srv_handler handler, int size, int int_ms)
{
	int sockfd;
	struct sockaddr_in serv_addr = { 0 };
	server_t server;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return 0;
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) {
		close(sockfd);
		return 0;
	}

	server = malloc(sizeof(*server));
	if (!server)
		goto err;

	server->socket = sockfd;
	server->handler = handler;
	server->size = size;
	server->pooling_int_us = int_ms;
	server->thread = malloc(sizeof(pthread_t));

	if (pthread_create(server->thread, NULL, server_handler,
		(void *)server)) {
		free(server);
		goto err;
	}

	return server;
err:
	close(sockfd);
	return 0;
}

int server_stop(server_t server)
{
	if (!server)
		return EINVAL;

	close(server->socket);
	server->socket = -1;

	return 0;
}
