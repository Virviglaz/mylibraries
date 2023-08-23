#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/inotify.h>

#include "server.h"

#define VERSION_STRING_LEN			16
#define OTA_CMD_GET_LATEST_VERSION		0xFE3378A0
#define OTA_CMD_GET_PAGE_DATA			0x47BF1975
#define OTA_PAGE_SIZE				1000
#define OTA_HEADER_SIZE	(OTA_PAGE_SIZE + sizeof(struct ota_version_header))

struct {
	char *filename;
	uint32_t port;
	uint32_t magic_word;
	uint32_t filesize;
	uint8_t *firmware;
	char *version_string;
} ota_ops = {
	.firmware = NULL,
};

struct ota_version_header {
	uint32_t magic_word;
	uint32_t cmd;
	uint32_t serial_number;
	char vers[VERSION_STRING_LEN];
	uint32_t fw_file_size;
	uint32_t page_size;
	uint32_t page_num;
	uint32_t data[];
};

struct message_header {
	uint32_t magic_word;
	uint32_t cmd;
	uint32_t serial_number;
	char vers[VERSION_STRING_LEN];
	uint32_t reserved[4];
	uint32_t message_size;
	char message[];
};

static void get_version(client_t client, void *data, size_t size)
{
	struct ota_version_header *msg = data;

	strcpy(msg->vers, ota_ops.version_string);
	msg->fw_file_size = ota_ops.filesize;
	msg->page_size = OTA_PAGE_SIZE;

	send_to_client(client, data, size);
}

static void get_next_page(client_t client, void *data, size_t size)
{
	struct ota_version_header *msg = data;
	uint32_t offset;
	uint32_t left;
	uint8_t *src;

	offset = msg->page_size * msg->page_num;
	left = ota_ops.filesize - offset;
	src = ota_ops.firmware + offset;

	if (offset >= ota_ops.filesize) {
		fprintf(stderr, "Unexpected offset value!\n");
		return;
	}

	if (left < msg->page_size)
		msg->page_size = left;

	memcpy(msg->data, src, msg->page_size);

	send_to_client(client, data, msg->page_size + sizeof(*msg));
}

static int connected(client_t client)
{
	printf("Client connected from %s\n", get_client_ip(client));

	return 0;
}

static int handler(client_t client, void *data, size_t size)
{
	struct ota_version_header *msg = data;

	if (size < sizeof(*msg))
		return 0;

	if (msg->magic_word != ota_ops.magic_word)
		return 0;

	switch (msg->cmd) {
	case OTA_CMD_GET_LATEST_VERSION:
		get_version(client, data, size);
		break;
	case OTA_CMD_GET_PAGE_DATA:
		get_next_page(client, data, size);
		break;
	default:
		fprintf(stderr, "OTA unexpected request\n");
	}

	return 0;
}

static int error(const char *str, int error, void *user)
{
	fprintf(stderr, "Error: %s, %s\n", str, strerror(error));
	return error;
}

static void disconnected(client_t client)
{
	printf("Client %s disconnected\n", get_client_ip(client));
}

static server_event_t server_ops = {
	.connected = connected,
	.receive = handler,
	.disconnected = disconnected,
	.error = error,
	.read_timeout_s = 0,
};

static char *version(uint8_t *firmware)
{
	return (char *)firmware + 48;
}

static server_t ota_server;
static bool is_running = true;
static int inotfd;
static void abort_handler(int sig)
{
	(void)sig;
	is_running = false;
	close(inotfd);
	printf("Server terminated\n");
}

static int get_firmware(const char *name, uint8_t **firmware, uint32_t *size)
{
	uint8_t *tmp;
	int res;

	/* file size */
	struct stat st;
	if (stat(name, &st)) {
		res = errno;
		fprintf(stderr, "%s stat failed: %s\n", name, strerror(res));
		return res;
	}

	tmp = malloc(st.st_size);
	if (!tmp) {
		fprintf(stderr, "No memory!\n");
		return ENOMEM;
	}

	if (size)
		*size = st.st_size;

	/* open file */
	int fd = open(name, O_RDONLY);
	if (fd < 0) {
		res = errno;
		fprintf(stderr,
			"Error opening file %s: %s\n", name, strerror(res));
		free(tmp);
		return res;
	}

	/* read file */
	uint32_t ret = read(fd, tmp, st.st_size);
	close(fd);

	if (ret != st.st_size) {
		fprintf(stderr, "Unexpected file read size!\n");
		free(tmp);
		return EINVAL;
	}

	*firmware = tmp;
	return 0;
}

/**
 * @brief OTA server
 * 
 * @param binary file path
 * @param server port
 * @param magic word
 * @example ./ota test.bin 5006 0xDEADBEEF
 */
int main(int argc, char** argv)
{
	int res;
	int watch_desc;
	size_t bufsiz;
	inotfd = inotify_init();
	if (inotfd < 0) {
		int err = errno;
		fprintf(stderr, "inotify_init failed: %s\n",
			strerror(err));
		return err;
	}

	if (argc != 4 || strlen(argv[1]) <= 2	/* filename */
		|| strlen(argv[2]) <= 2	/* port */
		|| strlen(argv[3]) <= 6) { /* magic word */
		fprintf(stderr, "Usage: %s (file name) (port) " \
			"(magic word)\n", argv[0]);
		return EINVAL;
	}

	ota_ops.filename = argv[1];
	ota_ops.port = strtoul(argv[2], NULL, 0);
	ota_ops.magic_word = strtoul(argv[3], NULL, 0);

	watch_desc = inotify_add_watch(inotfd, ota_ops.filename, IN_MODIFY);
	bufsiz = sizeof(struct inotify_event) + 0xff + 1;
	struct inotify_event* event = malloc(bufsiz);

	signal(SIGCHLD, SIG_IGN);
	signal(SIGABRT, abort_handler);
	signal(SIGINT, abort_handler);

restart:
	free(ota_ops.firmware);
	res = get_firmware(ota_ops.filename, &ota_ops.firmware,
		&ota_ops.filesize);
	if (res) {
		free(ota_ops.firmware);
		return res;
	}

	/* start server */
	ota_server = server_start(ota_ops.port, &server_ops,
		OTA_HEADER_SIZE, 0);
	if (!ota_server) {
		fprintf(stderr, "Start OTA server failed\n");
		free(ota_ops.firmware);
		return EFAULT;
	}

	ota_ops.version_string = version(ota_ops.firmware);

	printf("Server parameters:\n");
	printf("File: %s\n", ota_ops.filename);
	printf("File size: %u bytes\n", ota_ops.filesize);
	printf("Port: %u\n", ota_ops.port);
	printf("Magic word: 0x%.8X\n", ota_ops.magic_word);
	printf("Version string: %s\n", ota_ops.version_string);

	while (is_running) {
		/* wait for an event to occur */
		if (read(inotfd, event, bufsiz) < 0)
			break;
		printf("Firmware file modified, restarting server\n");
		server_stop(ota_server);
		goto restart;
	}

	free(ota_ops.firmware);
	res = server_stop(ota_server);
	close(watch_desc);

	if (res)
		fprintf(stderr, "Error: %s\n", strerror(res));
	return res;
}
