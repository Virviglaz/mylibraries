#ifndef SERVER_H
#define SERVER_H

typedef int(*srv_handler)(char *data, int size, int socket);

int server_start(int port, srv_handler handler);

#endif /* SERVER_H */
