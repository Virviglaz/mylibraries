/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/* ESP */
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "esp_ota_ops.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "nvs_flash.h"

/* STD */
#include <string.h>

/* wifi & ota */
#include "wifi.h"
#include "ota.h"

#define DEFAULT_CHECK_INTERVAL_MS		5000
#define DEFAULT_MESSAGE_SIZE			1000
#define DEFAULT_UNIQ_MESSAGE_WORD		0xDEADBEEF
#define HANDLER_TASK_STACK_SIZE			0x2000
#define VERSION_STRING_LEN			16
#define OTA_PAGE_SIZE				1000
#define OTA_CMD_GET_LATEST_VERSION		0xFE3378A0
#define OTA_CMD_GET_PAGE_DATA			0x47BF1975
#define MESSAGE_CMD				0x3789A3B0
#define OTA_HEADER_SIZE	(OTA_PAGE_SIZE + sizeof(struct ota_header))

static const char *tag = "ota";

struct ota_header {
	uint32_t magic_word;
	uint32_t cmd;
	uint32_t serial_number;
	char vers[VERSION_STRING_LEN];
	uint32_t fw_file_size;
	uint32_t page_size;
	uint32_t page_num;
	uint8_t data[];
};

static ota_t *s;
static uint32_t fw_file_size = 0;
static uint32_t total_bytes = 0;
static bool is_confirmed = false;
static bool is_running = false;
static SemaphoreHandle_t wait;

static int get_latest_version(int sockfd, char *server_version,
	uint32_t *fw_size, uint32_t *page_size)
{
	struct ota_header msg = {
		.magic_word = s->uniq_magic_word,
		.cmd = OTA_CMD_GET_LATEST_VERSION,
		.serial_number = s->serial_number,
	};
	int bytes_read;
	int bytes_written;

	strcpy(msg.vers, s->version);

	bytes_written = send(sockfd, &msg, sizeof(msg), 0);
	if (bytes_written != sizeof(msg))
		return -1;

	bytes_read = read(sockfd, &msg, sizeof(msg));
	if (bytes_read != sizeof(msg))
		return -1;
	if (server_version)
		strcpy(server_version, msg.vers);
	*fw_size = msg.fw_file_size;
	*page_size = msg.page_size;
	fw_file_size = msg.fw_file_size;
	total_bytes = 0;

	return 0;
}

static struct ota_header *get_next_page(int sockfd, uint32_t page,
	uint32_t page_size, uint32_t *buffer)
{
	struct ota_header msg = {
		.magic_word = s->uniq_magic_word,
		.cmd = OTA_CMD_GET_PAGE_DATA,
		.serial_number = s->serial_number,
		.page_size = page_size,
		.page_num = page,
	};
	struct ota_header *ret_msg = (struct ota_header *)buffer;
	int message_size = page_size + sizeof(msg);
	int bytes_written;
	int bytes_read;

	bytes_written = send(sockfd, &msg, sizeof(msg), 0);
	if (bytes_written != sizeof(msg))
		return 0;

	bytes_read = read(sockfd, ret_msg, message_size);
	if (bytes_read <= 0)
		return 0;

	if (ret_msg->magic_word != s->uniq_magic_word)
		return 0;

	if (ret_msg->page_num != page)
		return 0;

	return ret_msg;
}

/* Return true if this is an OTA app partition */
static bool is_ota_partition(const esp_partition_t *p)
{
	return (p != NULL
		&& p->type == ESP_PARTITION_TYPE_APP
		&& p->subtype >= ESP_PARTITION_SUBTYPE_APP_OTA_0
		&& p->subtype < ESP_PARTITION_SUBTYPE_APP_OTA_MAX);
}

static void start_update_process(int sockfd)
{
	char server_version[VERSION_STRING_LEN];
	esp_err_t ret = ESP_OK;
	esp_ota_handle_t handle;
	const esp_partition_t *partition;
	uint32_t page_num = 0;
	uint32_t fw_size;
	uint32_t page_size;
	uint32_t bytes_left;
	uint32_t bytes_written = 0;
	uint64_t timestamp = esp_timer_get_time();
	uint64_t prev_timestamp_s = timestamp / 1000000u;
	(void)ret;

	ESP_GOTO_ON_ERROR(get_latest_version(sockfd, server_version,
		&fw_size, &page_size), fail, tag, "get latest version failed");

	if (!strcmp(server_version, s->version))
		return; /* sw is actual */

	ESP_LOGI(tag, "Starting SW update from %s to %s",
		s->version, server_version);

	ESP_GOTO_ON_ERROR(nvs_flash_init(), fail, tag, "nvs init failed");

	partition = esp_ota_get_next_update_partition(0);
	if (!partition) {
		ESP_LOGE(tag, "No OTA partition found. SW update aborted");
		return;
	}
	partition = esp_partition_verify(partition);
	if (!partition) {
		ESP_LOGE(tag, "Partition validation failed");
		return;
	}

	if (!is_ota_partition(partition)) {
		ESP_LOGE(tag, "Not an OTA partition");
		return;
	}

	const esp_partition_t* running_partition =
		esp_ota_get_running_partition();
	if (partition == running_partition) {
		ESP_LOGE(tag, "Cannot update running partition");
		return;
	}

	/*ESP_GOTO_ON_ERROR(esp_partition_erase_range(partition, 0,
				partition->size), fail, tag,
				"fail to erase partition");*/

	ESP_GOTO_ON_ERROR(esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &handle),
		fail, tag, "ota begin failed");

	bytes_left = fw_size;

	uint32_t *buffer = malloc(OTA_HEADER_SIZE);
	if (!buffer)
		goto no_mem;

	if (s->gpio_ota_workaround)
		s->gpio_ota_workaround();

	while (bytes_left) {
		struct ota_header *msg = get_next_page(sockfd,
			page_num, page_size, buffer);
		if (!msg)
			goto abort;

		ESP_GOTO_ON_ERROR(esp_ota_write(handle, msg->data,
			msg->page_size), abort, tag, "ota write error");

		bytes_left -= msg->page_size;
		bytes_written += msg->page_size;
		page_num++;
		total_bytes += msg->page_size;

		uint64_t new_timestamp_s = esp_timer_get_time() / 1000000u;
		if (new_timestamp_s != prev_timestamp_s) {
			ESP_LOGI(tag, "Updating SW %lu bytes -> %lu/%lu",
				msg->page_size, bytes_written, fw_size);
			prev_timestamp_s = new_timestamp_s;
		}
	};

	ESP_GOTO_ON_ERROR(esp_ota_end(handle), abort, tag, "Ota end failed");
	ESP_GOTO_ON_ERROR(esp_ota_set_boot_partition(partition), abort, tag,
		"setting boot partition error");

	close(sockfd);

	ESP_LOGI(tag, "Firmware update is done in %llu seconds, rebooting...",
		(esp_timer_get_time() - timestamp) / 1000000u);

	esp_restart();

abort:
	free(buffer);
no_mem:
	esp_ota_abort(handle);
fail:
	if (s->gpio_ota_cancel_workaround)
		s->gpio_ota_cancel_workaround();
	return;
}

static int connect_to_ota_server(void)
{
	struct sockaddr_in dest_addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr(s->server_ip),
		.sin_port = htons(s->server_port),
	};
	int sockfd;
	int err;

	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
		return sockfd;

	err = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err) {
		close(sockfd);
		return -1;
	}

	return sockfd;
}

static void handler(void *args)
{
	vTaskDelay(pdMS_TO_TICKS(s->check_interval_ms));

	while (1) {
		if (xSemaphoreTake(wait,
			pdMS_TO_TICKS(s->check_interval_ms)) == pdTRUE)
			break;

		if (wifi_is_connected()) {
			int sockfd = connect_to_ota_server();
			if (sockfd > 0) {
				start_update_process(sockfd);
				close(sockfd);
			}
		}
	}

	vSemaphoreDelete(wait);
	is_running = false;
	vTaskDelete(NULL);
}

int ota_start(ota_t *settings)
{
	if (is_running) {
		ESP_LOGE(tag, "Software updater is already running");
		return EINVAL;
	}

	s = settings;
	wait = xSemaphoreCreateBinary();

	if (!s->check_interval_ms)
		s->check_interval_ms = DEFAULT_CHECK_INTERVAL_MS;
	if(!s->message_size)
		s->message_size = DEFAULT_MESSAGE_SIZE;
	if (!s->uniq_magic_word)
		s->uniq_magic_word = DEFAULT_UNIQ_MESSAGE_WORD;

	if (!s->version) {
		static char vers[32];
		const esp_app_desc_t *app_desc = esp_app_get_description();
		strcpy(vers, app_desc->version);
		s->version = vers;
	}

	if (xTaskCreate(handler, tag,
		HANDLER_TASK_STACK_SIZE, 0, 1, 0) != pdTRUE)
		return EINVAL;

	is_running = true;
	return 0;
}

int ota_confirm(void)
{
	if (is_running) {
		ESP_LOGE(tag, "Software updater is running, can't confirm");
		return EINVAL;
	}

	if (esp_ota_mark_app_valid_cancel_rollback() != ESP_OK) {
		ESP_LOGE(tag, "Can't confirm running image is correct");
		return EINVAL;
	}

	is_confirmed = true;
	return 0;
}

int ota_stop(void)
{
	if (!is_running) {
		ESP_LOGE(tag, "Software updater is not running");
		return EINVAL;
	}

	xSemaphoreGive(wait);
	return 0;
}
