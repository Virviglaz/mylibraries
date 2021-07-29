#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>

typedef struct server_t *server_t;
typedef struct client_t *client_t;
typedef void(*rcv_handler)(client_t server, void *data, size_t size);

server_t server_start(int port, rcv_handler handler, int size);
int send_to_client(client_t client, void *msg, size_t size);
char *get_client_ip(client_t client);
int get_client_port(client_t client);
int server_stop(server_t server);

#endif /* __SERVER_H__ */
