#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct server *server_t;
typedef struct client *client_t;

typedef struct {
	int (*connected)(client_t client);
	int (*receive)(client_t client, void *data, size_t size);
	void (*disconnected)(client_t client);
	int (*error)(const char *str, int error, void *user);
	size_t read_timeout_s;
} server_event_t;

server_t server_start(uint16_t port, server_event_t *ops, size_t size, void *user);
int send_to_client(client_t client, void *msg, size_t size);
const char *get_client_ip(client_t client);
int get_client_port(client_t client);
void *get_user_data(client_t client);
int server_stop(server_t server);
int server_wait_for_err(server_t server);

#ifdef __cplusplus
}
#endif

#endif /* __SERVER_H__ */
