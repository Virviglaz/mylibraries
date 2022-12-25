#include "logger.h"
#include "wifi.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "free_rtos_h.h"
#include <stdarg.h>

#define MESSAGE_QUEUE_SIZE			1000
#define MESSAGE_SERVER_POLL_TIMEOUT		10000
#define VPRINTF_BUFFER_SIZE			256
#define MINIMUM_FREE_MEMORY_SIZE		0x6000 /* 24KB */

static QueueHandle_t msg_queue = 0;
static const char *log_server = NULL;
static uint32_t server_port;
static int sockfd = 0;

static void send_to_server(void)
{
	char *msg;

	if (!sockfd && connect_to_server(&sockfd, log_server, server_port)) {
		delay(MESSAGE_SERVER_POLL_TIMEOUT);
		sockfd = 0;
		return;
	}

	while (xQueuePeek(msg_queue, &msg,
		pdMS_TO_TICKS(MESSAGE_SERVER_POLL_TIMEOUT)) == pdTRUE) {
		int size = strlen(msg) + 1;
		if (write(sockfd, msg, size) == size) {
			uint32_t ack = 0;
			read(sockfd, &ack, sizeof(ack));
			if (!ack)
				break;
			/* Remove message from the queue */
			xQueueReceive(msg_queue, &msg, 0);
			free(msg);
		} else {
			/* Fail to send, retry next time */
			close(sockfd);
			sockfd = 0;
			break;
		}
	}
}

static void handler(void *args)
{
	while (1) {
		if (wifi_is_connected() && uxQueueMessagesWaiting(msg_queue))
			send_to_server();
		else
			delay(MESSAGE_SERVER_POLL_TIMEOUT);
	}
}

static void send_message_to_server(const char *msg, int max_size)
{
	char *p;
	int size;

	if (!msg_queue || !log_server)
		return;

	size = strlen(msg) + 1;

	if (size < max_size) {
		p = malloc(size);
		if (!p)
			return;

		strcpy(p, msg);
	} else {
		/* exception! Buffer overflow! */
		p = malloc(25);
		strcpy(p, "ERROR! BUFFER OVERFLOW!");
	}

	/* queue is full or memory is low, drop the last message */
	if (uxQueueMessagesWaiting(msg_queue) == MESSAGE_QUEUE_SIZE ||
		xPortGetFreeHeapSize() <= MINIMUM_FREE_MEMORY_SIZE) {
		char *d;
		xQueueReceive(msg_queue, &d, 0);
		free(d);
	}

	if (xQueueSend(msg_queue, &p, 0) != pdPASS)
		free(p); /* buffer is full (should not be possible) */
}

/*
 * This function can replaces the in-build printf logging function.
 * If the wifi connection is established we assume that server is ready to
 * receive the messages. All log output will be redirected to the server using
 * TCP/IP protocol by lwip stack using header based message pool.
 */
static int server_vprintf(const char *format, va_list arg)
{
	static SemaphoreHandle_t lock = NULL;
	static char buf[VPRINTF_BUFFER_SIZE];
	int res;

	if (!lock)
		lock = xSemaphoreCreateMutex();
	xSemaphoreTake(lock, portMAX_DELAY);

	res = vsnprintf((void *)buf, sizeof(buf), format, arg);

	send_message_to_server(buf, sizeof(buf));

	xSemaphoreGive(lock);

	return res;
}

static void clean_queue(void)
{
	char *msg;

	while (msg_queue && xQueueReceive(msg_queue, &msg, 0))
		free(msg);
}

static void init_queue(void)
{
	if (!msg_queue)
		msg_queue = queue_create(MESSAGE_QUEUE_SIZE, sizeof(void *));
}

static bool forced_to_use_remote = false;
void switch_printf(bool remote)
{
	/* If forced to use remote ignore the switching */
	if (forced_to_use_remote)
		return;
	/*
	 * The remote connection is lost. We cannot guarantee that the last
	 * message is actual. In order to keep track of only the actual
	 * messages, we have to discard all messages pending in the queue.
	 */
	if (!remote)
		clean_queue();

	esp_log_set_vprintf(remote ? server_vprintf : vprintf);
}

void message_logger_init(const char *server, uint32_t port)
{
	log_server = server;
	server_port = port;

	init_queue();

	task_create(handler, "msg_log", 0x1000, 0, 1, 0);
}

/* switch to remote logging without initializing the handling task */
void message_logger_early_init(const char *server, uint32_t port)
{
	log_server = server;
	server_port = port;

	init_queue();

	switch_printf(true);

	forced_to_use_remote = true;
}

