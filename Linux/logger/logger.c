#include "server.h"
#include "log.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>

static int err;
static const char* dst;
static uint32_t token;
static void error(const char* str, int error, void* user)
{
	err = error;

	fprintf(stderr, "Error: %s, %s\n", str, strerror(error));
}

static int receive(client_t client, void* data, size_t size)
{
	const char* msg = (const char*)data;
	uint32_t ack = 1;

	if (token) {
		/* filter the message */
		if (*(uint32_t*)data != token)
			return 0;
		msg += sizeof(uint32_t);
	}

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
	if (argc != 4) {
		printf("Usage: %s {log_file} {port} {token}\n", argv[0]);
		return EINVAL;
	}

	dst = argv[1];
	uint16_t port = (uint16_t)strtoul(argv[2], NULL, 10);
	token = (uint32_t)strtoul(argv[3], NULL, 16);

	signal(SIGCHLD, SIG_IGN);

	server_t logserver = server_start(port, &server_ops, 256, NULL);

	if (!logserver) {
		fprintf(stderr, "Start message server failed\n");
		return err;
	}

	printf("Message server started at port %u with token 0x%.8X\n",
		port, token);

	return server_wait_for_err(logserver);
}