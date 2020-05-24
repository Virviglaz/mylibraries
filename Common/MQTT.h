#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>

enum mqtt_err_t {
	MQTT_SUCCESS = 0,
	MQTT_AUTH_ERROR,
	MQTT_PING_FAIL,
	MQTT_NO_MESSAGES,
	MQTT_FORMAT_ERR,
	MQTT_NO_MESSAGES_FOR_THIS_TOPIC,
};

struct mqtt_client_t {
	uint8_t (*send_receive) (uint8_t *buf, uint8_t len);
	const char *clientId, *username, *password;
	uint8_t *buf;
};

enum mqtt_err_t mqtt_connect(struct mqtt_client_t *client);
enum mqtt_err_t mqtt_ping(struct mqtt_client_t *client);
enum mqtt_err_t mqtt_send(struct mqtt_client_t *client,
	char *topic, char *msg);
enum mqtt_err_t mqtt_subscribe(struct mqtt_client_t *client,
	char *topic);
enum mqtt_err_t mqtt_check(struct mqtt_client_t *client,
	char *topic, char *msg);
enum mqtt_err_t mqtt_get(struct mqtt_client_t *client,
	char *topic, char *msg);

#endif /* MQTT_H */
