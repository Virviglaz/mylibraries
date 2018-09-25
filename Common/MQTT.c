#include "MQTT.h"
#include <string.h>
#include <stdio.h>

mqtt_client_t * client;

mqtt_client_t * mqtt_init (mqtt_client_t * ptr)
{
	if (ptr) client = ptr;
	
	return client;
}

mqtt_err_t mqtt_connect (void)
{
	if (client == NULL) return MQTT_NOT_INIT;
	const uint8_t mqtt_init_str[] = { 'M', 'Q', 'T', 'T', 0x04, 0xC2, 0x00, 0x3C };
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)client->buf;
	mqtt_flex_header_t  * mqtt_flex_header;
	uint8_t * ptr = (void*)mqtt_fixed_header->data;

	/* Fixed header */
	mqtt_fixed_header->msg_type = CONNECT;
	mqtt_fixed_header->dup = 0;
	mqtt_fixed_header->qos = 0;
	mqtt_fixed_header->retain = 0;

	/* Init string */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = 4;
	memcpy(mqtt_flex_header->data, mqtt_init_str, sizeof(mqtt_init_str));
	ptr += sizeof(mqtt_init_str) + sizeof(mqtt_flex_header_t);
	
	/* ClientId */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(client->clientId);
	strcpy(mqtt_flex_header->data, client->clientId);
	ptr += mqtt_flex_header->len + sizeof(mqtt_flex_header_t);
	
	/* Username */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(client->username);
	strcpy(mqtt_flex_header->data, client->username);
	ptr += mqtt_flex_header->len + sizeof(mqtt_flex_header_t);
	
	/* Password */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(client->password);
	strcpy(mqtt_flex_header->data, client->password);
	ptr += mqtt_flex_header->len;
	
	/* Update message len in header */
	mqtt_fixed_header->len = ptr - client->buf;
	
	/* Connect to server and try auth */
	client->sendReceive(client->buf, mqtt_fixed_header->len + sizeof(mqtt_fixed_header_t));
	
	/* Return result */
	return client->buf[0] == (CONNACK << 4) ? MQTT_SUCCESS : MQTT_AUTH_ERROR;
}

mqtt_err_t mqtt_ping (void)
{
	const char ping_str[2] = { PINGREQ << 4, 0x00 };
	if (client == NULL) return MQTT_NOT_INIT;
	memcpy((void*)client->buf, ping_str, sizeof(ping_str));
	
	client->sendReceive(client->buf, sizeof(ping_str));
	
	return client->buf[0] == (PINGRESP << 4) ? MQTT_SUCCESS : MQTT_PING_FAIL;
}

mqtt_err_t sendMessageToServerMessage (char * topic, char * message)
{
	if (client == NULL) return MQTT_NOT_INIT;
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)client->buf;
	mqtt_flex_header_t  * mqtt_flex_header;
	
	mqtt_fixed_header->msg_type = PUBLISH;
	mqtt_fixed_header->dup = 0;
	mqtt_fixed_header->qos = 1;
	mqtt_fixed_header->retain = 0;
	mqtt_fixed_header->len = sizeof(mqtt_fixed_header_t) + strlen(topic) + strlen(message) + sizeof(uint16_t);

	/* Topic string */
	mqtt_flex_header = (void*)mqtt_fixed_header->data;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(topic);
	strcpy(mqtt_flex_header->data, topic);
	
	static uint16_t packet_id = 0;
	packet_id++;
	memcpy(mqtt_flex_header->data + mqtt_flex_header->len, &packet_id, sizeof(uint16_t));
	strcpy(mqtt_flex_header->data + mqtt_flex_header->len + sizeof(uint16_t), message);
	client->sendReceive(client->buf, mqtt_fixed_header->len + sizeof(mqtt_fixed_header_t));
	
	/* Return result */
	return client->buf[0] == (PUBACK << 4) ? MQTT_SUCCESS : MQTT_AUTH_ERROR;	
}

mqtt_err_t mqtt_subscribe (char * topic)
{
	if (client == NULL) return MQTT_NOT_INIT;
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)client->buf;
	mqtt_flex_header_t  * mqtt_flex_header = (void*)mqtt_fixed_header->data;
	
	mqtt_fixed_header->msg_type = SUBSCRIBE;
	mqtt_fixed_header->dup = 0;
	mqtt_fixed_header->qos = 1;
	mqtt_fixed_header->retain = 0;
	mqtt_fixed_header->len = 2 * sizeof(mqtt_fixed_header_t) + strlen(topic) + 1;
	
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = 1;
	
	mqtt_flex_header = (void*)mqtt_flex_header->data;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(topic);

	strcpy((void*)mqtt_flex_header->data, topic);
	
	client->sendReceive(client->buf, mqtt_fixed_header->len + sizeof(mqtt_fixed_header_t));
	
	return client->buf[0] == (SUBACK << 4) ? MQTT_SUCCESS : MQTT_AUTH_ERROR;	
}

mqtt_err_t mqtt_checkMessages (char * topic, char * messageOut)
{
	if (client->sendReceive(client->buf, 0) == 0) return MQTT_NO_MESSAGES;
	
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)client->buf;

	if (mqtt_fixed_header->msg_type != PUBLISH) return MQTT_FORMAT_ERR;
	
	if (client->buf[3] > mqtt_fixed_header->len) return MQTT_FORMAT_ERR;
	
	if (memcmp(topic, &client->buf[4], client->buf[3])) return MQTT_NO_MESSAGES_FOR_THIS_TOPIC;
		
	uint8_t message_len = mqtt_fixed_header->len - client->buf[3] - sizeof(mqtt_fixed_header_t);
	memcpy(messageOut, &client->buf[ client->buf[3] + sizeof(mqtt_fixed_header_t) + sizeof(mqtt_flex_header_t) ], message_len);
	messageOut[message_len] = 0;
	
	return MQTT_SUCCESS;
}

mqtt_err_t mqtt_getMessages (char * topicOut, char * messageOut)
{
	if (client->sendReceive(client->buf, 0) == 0) return MQTT_NO_MESSAGES;
	
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)client->buf;

	if (mqtt_fixed_header->msg_type != PUBLISH) return MQTT_FORMAT_ERR;
	
	if (client->buf[3] > mqtt_fixed_header->len) return MQTT_FORMAT_ERR;

	memcpy(topicOut, &client->buf[4], client->buf[3]);
	topicOut[client->buf[3]] = 0;
		
	uint8_t message_len = mqtt_fixed_header->len - client->buf[3] - sizeof(mqtt_fixed_header_t);
	memcpy(messageOut, &client->buf[ client->buf[3] + sizeof(mqtt_fixed_header_t) + sizeof(mqtt_flex_header_t) ], message_len);
	messageOut[message_len] = 0;
	
	return MQTT_SUCCESS;
}
