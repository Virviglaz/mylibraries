#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>

typedef struct server_t *server_t;
typedef struct client_t *client_t;

typedef struct {
	int (*connected)(client_t client);
	int (*receive)(client_t client, void *data, size_t size);
	void (*disconnected)(client_t client);
	void (*error)(const char *str, int error, void *user);
} server_event_t;

server_t server_start(int port, server_event_t *ops, int size, void *user);
int send_to_client(client_t client, void *msg, size_t size);
const char *get_client_ip(client_t client);
int get_client_port(client_t client);
void *get_user_data(client_t client);
int server_stop(server_t server);

#endif /* __SERVER_H__ */
