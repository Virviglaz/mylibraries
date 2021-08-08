#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "server.h"

struct client_t {
	int socket;
	struct sockaddr_in cli_addr;
	rcv_handler handler;
	pthread_t *thread;
	size_t size;
	void *user;
};

struct server_t {
	int socket;
	pthread_t *thread;
	socklen_t clilen;
	rcv_handler handler;
	size_t size;
	void *user;
};

static void *client_handler(void *ptr)
{
	struct client_t *client = ptr;
	void *buffer;
	ssize_t size;

	buffer = malloc(client->size);
	if (!buffer)
		goto err_nomem;

	do {
		size = read(client->socket, buffer,
			client->size);
		if (size)
			client->handler(client, buffer, size);
	} while (size > 0);

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
		client_t client = malloc(sizeof(*client));
		if (!client)
			return 0;

		clientfd = accept(server->socket,
			(struct sockaddr *)&client->cli_addr, &server->clilen);

		client->handler = server->handler;
		client->user = server->user;
		client->socket = clientfd;
		client->size = server->size;
		client->thread = malloc(sizeof(pthread_t));
		if (!client->thread)
			break;

		if (pthread_create(client->thread, NULL,
			client_handler, (void *)client))
			break;

	} while (clientfd > 0 && server->socket > 0);
	close(server->socket);

	return 0;
}

server_t server_start(int port, rcv_handler handler, int size, void *user)
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
	server->user = user;
	server->thread = malloc(sizeof(pthread_t));

	if (pthread_create(server->thread, NULL, server_handler,
		(void *)server)) {
		free(server->thread);
		free(server);
		goto err;
	}

	return server;
err:
	close(sockfd);
	return 0;
}

int send_to_client(client_t client, void *msg, size_t size)
{
	int err = send(client->socket, msg, size, 0);

	return err < 0 ? errno : 0;
}

char *get_client_ip(client_t client)
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
	free(server->thread);
	free(server);

	return 0;
}
