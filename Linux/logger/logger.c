#include "server.h"
#include "log.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

static int err;
static const char* dst;
static void error(const char* str, int error, void* user)
{
	err = error;

	fprintf(stderr, "Error: %s, %s\n", str, strerror(error));
}

static int receive(client_t client, void* data, size_t size)
{
	const char* msg = (const char*)data;
	uint32_t ack = 1;

	if (size > 256)
		return EINVAL;

	printf("%s", msg);
	log_to_file(dst, "%s", msg);

	send_to_client(client, &ack, sizeof(ack));

	return 0;
}

static server_event_t server_ops = {
	.connected = 0,
	.receive = receive,
	.disconnected = 0,
	.error = error,
};

int main(int argc, char* argv[])
{
	if (argc != 3) {
		printf("Usage: %s {log_file} {port}\n", argv[0]);
		return EINVAL;
	}

	dst = argv[1];
	uint16_t port = (uint16_t)atoi(argv[2]);

	signal(SIGCHLD, SIG_IGN);

	server_t logserver = server_start(port, &server_ops, 256, NULL);

	if (!logserver) {
		fprintf(stderr, "Start message server failed\n");
		return err;
	}

	printf("Message server started at port %u\n", port);

	return server_wait_for_err(logserver);
}

