#include "MQTT.h"
#include <string.h>
#include <stdio.h>

enum {
	RESERVED		= 0x0,
	CONNECT		= 0x1,
	CONNACK		= 0x2,
	PUBLISH			= 0x3,
	PUBACK			= 0x4,
	PUBREC			= 0x5,
	PUBREL			= 0x6,
	PUBCOMP		= 0x7,
	SUBSCRIBE		= 0x8,
	SUBACK			= 0x9,
	UNSUBSCRIBE		= 0xA,
	UNSUBACK		= 0xB,
	PINGREQ			= 0xC,
	PINGRESP		= 0xD,
	DISCONNECT		= 0xE,
};

struct fixed_header_t {
	uint8_t retain : 1;
	uint8_t qos : 2;
	uint8_t dup : 1;
	uint8_t msg_type : 4;
	uint8_t len;
	uint8_t data[];
};

struct flex_header_t {
	uint8_t flags;
	uint8_t len;
	char data[];
};

enum mqtt_err_t mqtt_connect(struct mqtt_client_t *client)
{
	const uint8_t mqtt_init_str[] =
		{ 'M', 'Q', 'T', 'T', 0x04, 0xC2, 0x00, 0x3C };
	struct fixed_header_t *fxh = (void*)client->buf;
	struct flex_header_t  *flh;
	uint8_t *ptr = (void*)fxh->data;

	/* Fixed header */
	fxh->msg_type = CONNECT;
	fxh->dup = 0;
	fxh->qos = 0;
	fxh->retain = 0;

	/* Init string */
	flh = (void*)ptr;
	flh->flags = 0;
	flh->len = 4;
	memcpy(flh->data, mqtt_init_str,
		sizeof(mqtt_init_str));
	ptr += sizeof(mqtt_init_str) +
		sizeof(struct flex_header_t);

	/* ClientId */
	flh = (void*)ptr;
	flh->flags = 0;
	flh->len = strlen(client->clientId);
	strcpy(flh->data, client->clientId);
	ptr += flh->len + sizeof(struct flex_header_t);

	/* Username */
	flh = (void*)ptr;
	flh->flags = 0;
	flh->len = strlen(client->username);
	strcpy(flh->data, client->username);
	ptr += flh->len + sizeof(struct flex_header_t);

	/* Password */
	flh = (void*)ptr;
	flh->flags = 0;
	flh->len = strlen(client->password);
	strcpy(flh->data, client->password);
	ptr += flh->len;

	/* Update message len in header */
	fxh->len = ptr - client->buf;

	/* Connect to server and try auth */
	client->send_receive(client->buf, fxh->len +
		sizeof(struct fixed_header_t));

	/* Return result */
	return client->buf[0] == (CONNACK << 4) ? \
		MQTT_SUCCESS : MQTT_AUTH_ERROR;
}

enum mqtt_err_t mqtt_ping(struct mqtt_client_t *client)
{
	const char ping_str[2] = { PINGREQ << 4, 0x00 };

	memcpy((void*)client->buf, ping_str,
		sizeof(ping_str));

	client->send_receive(client->buf, sizeof(ping_str));

	return client->buf[0] == (PINGRESP << 4) ? \
		MQTT_SUCCESS : MQTT_PING_FAIL;
}

enum mqtt_err_t mqtt_send(struct mqtt_client_t *client,
	char *topic, char *msg)
{
	struct fixed_header_t *fxh = (void*)client->buf;
	struct flex_header_t  *flh;

	fxh->msg_type = PUBLISH;
	fxh->dup = 0;
	fxh->qos = 1;
	fxh->retain = 0;
	fxh->len = sizeof(struct fixed_header_t) + \
		strlen(topic) + strlen(msg) + \
			sizeof(uint16_t);

	/* Topic string */
	flh = (void*)fxh->data;
	flh->flags = 0;
	flh->len = strlen(topic);
	strcpy(flh->data, topic);

	static uint16_t packet_id = 0;
	packet_id++;
	memcpy(flh->data + flh->len, &packet_id,
		sizeof(uint16_t));
	strcpy(flh->data + flh->len + sizeof(uint16_t),
		msg);
	client->send_receive(client->buf, fxh->len + \
		sizeof(struct fixed_header_t));

	/* Return result */
	return client->buf[0] == (PUBACK << 4) ? \
		MQTT_SUCCESS : MQTT_AUTH_ERROR;
}

enum mqtt_err_t mqtt_subscribe(struct mqtt_client_t *client,
	char * topic)
{
	struct fixed_header_t * fxh = (void*)client->buf;
	struct flex_header_t  * flh = (void*)fxh->data;

	fxh->msg_type = SUBSCRIBE;
	fxh->dup = 0;
	fxh->qos = 1;
	fxh->retain = 0;
	fxh->len = 2 * sizeof(struct fixed_header_t) + \
		strlen(topic) + 1;

	flh->flags = 0;
	flh->len = 1;

	flh = (void*)flh->data;
	flh->flags = 0;
	flh->len = strlen(topic);

	strcpy((void*)flh->data, topic);

	client->send_receive(client->buf, fxh->len + \
		sizeof(struct fixed_header_t));

	return client->buf[0] == (SUBACK << 4) ? \
		MQTT_SUCCESS : MQTT_AUTH_ERROR;
}

enum mqtt_err_t mqtt_check(struct mqtt_client_t *client,
	char *topic, char *msg)
{
	struct fixed_header_t * fxh = (void*)client->buf;

	if (client->send_receive(client->buf, 0) == 0)
		return MQTT_NO_MESSAGES;

	if (fxh->msg_type != PUBLISH)
		return MQTT_FORMAT_ERR;

	if (client->buf[3] > fxh->len)
		return MQTT_FORMAT_ERR;

	if (memcmp(topic, &client->buf[4], client->buf[3]))
		return MQTT_NO_MESSAGES_FOR_THIS_TOPIC;

	uint8_t message_len = fxh->len - client->buf[3] - \
		sizeof(struct fixed_header_t);
	memcpy(msg, &client->buf[ client->buf[3] + \
		sizeof(struct fixed_header_t) + \
			sizeof(struct flex_header_t) ],\
				message_len);
	msg[message_len] = 0;

	return MQTT_SUCCESS;
}

enum mqtt_err_t mqtt_get(struct mqtt_client_t *client,
	char *topic, char *message)
{
	struct fixed_header_t * fxh = (void*)client->buf;

	if (client->send_receive(client->buf, 0) == 0)
		return MQTT_NO_MESSAGES;

	if (fxh->msg_type != PUBLISH)
		return MQTT_FORMAT_ERR;

	if (client->buf[3] > fxh->len)
		return MQTT_FORMAT_ERR;

	memcpy(topic, &client->buf[4], client->buf[3]);
	topic[client->buf[3]] = 0;

	uint8_t message_len = fxh->len - client->buf[3] - \
		sizeof(struct fixed_header_t);
	memcpy(message, &client->buf[ client->buf[3] + \
		sizeof(struct fixed_header_t) + \
			sizeof(struct flex_header_t) ], \
				message_len);
	message[message_len] = 0;

	return MQTT_SUCCESS;
}
