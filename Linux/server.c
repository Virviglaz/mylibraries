#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include "server.h"

struct params_t {
	int socket;
	srv_handler handler;
	pthread_t *this;
	int size;
	int pooling_int_us;
};

static void *client_handler(void *ptr)
{
	struct params_t *param = ptr;
	char *buffer;
	int size;
	int err;
	socklen_t len = sizeof(err);

	buffer = malloc(param->size);
	if (!buffer)
		goto nomem;

	do {
		if (getsockopt(param->socket, SOL_SOCKET, SO_ERROR, &err, &len))
			break;

		size = read(param->socket, buffer, param->size);
		if (size)
			param->handler(buffer, size, param->socket);
		usleep(param->pooling_int_us);

	} while (!err);

nomem:
	close(param->socket);
	free(buffer);
	free(param->this);
	free(param);
}

int server_start(int port, srv_handler handler, int size, int int_ms)
{
	int sockfd, clientfd;
	socklen_t clilen;
	struct sockaddr_in cli_addr = { 0 };
	struct sockaddr_in serv_addr = { 0 };

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return -EINVAL;
	}

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) {
		close(sockfd);
		return -EINVAL;
	}
		
	listen(sockfd, 10);
	do {
		struct params_t *param;
		clientfd = accept(sockfd,
			(struct sockaddr *) &cli_addr, &clilen);

		param = malloc(sizeof(*param));
		if (!param)
			break;

		param->handler = handler;
		param->socket = clientfd;
		param->size = size;
		param->pooling_int_us = int_ms * 1000;
		param->this = malloc(sizeof(pthread_t));
		if (!param->this)
			break;

		if (pthread_create(param->this, NULL,
			client_handler, (void *)param))
			break;

	} while (clientfd > 0);
	close(sockfd);

	return 0;
}
