/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* STD */
#include <stdint.h>
#include <string.h>

/* ESP32 */
#include "esp_wifi.h"
#include "esp_check.h"
#include "nvs_flash.h"

#include "wifi.h"
static const char *tag = "wifi";
static bool init_done = false;
static bool is_connected = false;
static struct {
	const char *uuid;
	const char *pass;
	const char *host;
} credentials;

static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;

static void fix_connection(void)
{
	esp_wifi_disconnect();
	esp_wifi_connect();

	is_connected = false;
}

static void event_handler(void* arg, esp_event_base_t event_base,
		int32_t event_id, void* event_data)
{
	wifi_config_t sta_cfg;

	if (event_base == WIFI_EVENT) {
		switch (event_id) {
		case WIFI_EVENT_STA_DISCONNECTED:
			fix_connection();
			break;
		case WIFI_EVENT_STA_START:
			fix_connection();
			break;
		case WIFI_EVENT_STA_CONNECTED:
			esp_wifi_get_config(WIFI_IF_STA, &sta_cfg);
			break;
		case WIFI_EVENT_STA_BEACON_TIMEOUT:
			break;
		}
	}

	if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		char ip_address[16];
		ip_event_got_ip_t *event = (ip_event_got_ip_t*)event_data;
		snprintf(ip_address, sizeof(ip_address), IPSTR,
			IP2STR(&event->ip_info.ip));
		ESP_LOGI("Wifi", "Wifi got ip: %s", ip_address);
		is_connected = true;
	}
}

static void handler(void *param)
{
	esp_err_t ret = ESP_OK;

	ESP_GOTO_ON_ERROR(nvs_flash_init(), err, tag, "NVS init failed");

	ESP_GOTO_ON_ERROR(init_done, err, tag, "already initialized");

	ESP_GOTO_ON_ERROR(esp_netif_init(), err, tag, "init failed");

	ESP_GOTO_ON_ERROR(esp_event_loop_create_default(), err, tag,
		"event loop create failed");

	esp_netif_create_default_wifi_sta();

	const wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_GOTO_ON_ERROR(esp_wifi_init(&config), err, tag, "init conf failed");

	uint8_t mac[6];
	ESP_GOTO_ON_ERROR(esp_wifi_get_mac(ESP_IF_WIFI_STA, mac), err, tag,
		"obtaining MAC address failed");

	ESP_GOTO_ON_ERROR(esp_event_handler_instance_register(WIFI_EVENT,
		ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id),
		err, tag, "failed to create WIFI_EVENT handler");

	ESP_GOTO_ON_ERROR(esp_event_handler_instance_register(IP_EVENT,
		IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip),
		err, tag, "failed to create IP_EVENT handler");

	wifi_config_t sta_cfg = { 0 };
	sta_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
	sta_cfg.sta.pmf_cfg.capable = true;
	strcpy((char *)sta_cfg.sta.ssid, credentials.uuid);
	strcpy((char *)sta_cfg.sta.password, credentials.pass);

	ESP_GOTO_ON_ERROR(tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA,
		credentials.host), err, tag, "failed to set hostname");

	ESP_GOTO_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), err, tag,
		"failed to set STA mode");

	ESP_GOTO_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg), err, tag,
		"failed to set STA consig");

	ESP_GOTO_ON_ERROR(esp_wifi_start(), err, tag, "failed to start");

err:
	if (ret)
		ESP_LOGE(tag, "%s", esp_err_to_name(ret));
	else
		init_done = true;

	vTaskDelete(NULL);
}

int wifi_init(const char *uuid, const char *pass, const char *hostname)
{
	credentials.uuid = uuid;
	credentials.pass = pass;
	credentials.host = hostname;

	return xTaskCreate(handler, tag, 0x1000, 0, 1, NULL);
}

bool wifi_is_connected(void)
{
	return is_connected;
}
