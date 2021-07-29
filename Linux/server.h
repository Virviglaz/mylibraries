#ifndef __SERVER_H__
#define __SERVER_H__

typedef void(*srv_handler)(char *data, int size, int socket);
typedef struct server_t *server_t;

server_t server_start(int port, srv_handler handler, int size, int int_ms);
int server_stop(server_t server);

#endif /* __SERVER_H__ */
