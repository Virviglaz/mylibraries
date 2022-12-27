#include "logger.h"
#include "wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "free_rtos_h.h"
#include <stdarg.h>
#include <errno.h>

static logger_init_t *conf = NULL;
static QueueHandle_t msg_queue = 0;
static int sockfd = 0;

static void send_to_server(void)
{
	char *msg;

	if (!sockfd && connect_to_server(&sockfd, conf->server, conf->port)) {
		delay(conf->server_poll_timeout);
		sockfd = 0;
		return;
	}

	while (xQueuePeek(msg_queue, &msg,
		pdMS_TO_TICKS(conf->server_poll_timeout)) == pdTRUE) {
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
			delay(conf->server_poll_timeout);
	}
}

static void send_message_to_server(void *buf, int max_size)
{
	const char *msg = (const char *)buf;
	char *p;
	int size;

	if (!msg_queue || !conf)
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
	if (uxQueueMessagesWaiting(msg_queue) == conf->message_queue_size ||
		xPortGetFreeHeapSize() <= conf->minimum_free_memory) {
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
	static void *buf = NULL;
	static void *buf_token; /* same buffer excluding token */
	int res;

	if (!lock)
		lock = xSemaphoreCreateMutex();
	xSemaphoreTake(lock, portMAX_DELAY);

	if (!buf) {
		int size = conf->printf_buffer_size;
		if (conf->token)
			size += sizeof(uint32_t);
		buf = malloc(size);
		if (!buf)
			return ENOMEM;
		buf_token = buf;
		if (conf->token) /* if set, use token */
			*(uint32_t *)buf = conf->token;
		buf = (uint32_t *)buf + 1;
	}

	res = vsnprintf(buf, conf->printf_buffer_size, format, arg);

	send_message_to_server(buf_token, conf->printf_buffer_size);

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
		msg_queue = queue_create(conf->message_queue_size,
			sizeof(void *));
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

void message_logger_init(logger_init_t *init)
{
	if (conf) /* check init is already done */
		return;

	conf = init;
	init_queue();

	task_create(handler, __FILE__, conf->task_heap_size, 0, 1, 0);
}

/* switch to remote logging without initializing the handling task */
void message_logger_early_init(logger_init_t *init)
{
	if (conf) /* check init is already done */
		return;

	conf = init;
	init_queue();

	switch_printf(true);

	forced_to_use_remote = true;
}

