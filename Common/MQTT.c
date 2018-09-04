#include "MQTT.h"
#include <string.h>
#include <stdio.h>

uint8_t mqtt_connect (char * buf, char * clientId, char * username, char * password)
{
	const uint8_t mqtt_init_str[] = { 'M', 'Q', 'T', 'T', 0x04, 0xC2, 0x00, 0x3C };
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)buf;
	static mqtt_flex_header_t  * mqtt_flex_header;
	char * ptr = (void*)mqtt_fixed_header->data;

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
	ptr += sizeof(mqtt_init_str);
	
	/* ClientId */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(clientId);
	ptr += mqtt_flex_header->len;
	
	/* Username */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(username);
	ptr += mqtt_flex_header->len;
	
	/* Password */
	mqtt_flex_header = (void*)ptr;
	mqtt_flex_header->flags = 0;
	mqtt_flex_header->len = strlen(password);
	ptr += mqtt_flex_header->len;
	
	mqtt_fixed_header->len = ptr - buf;
	return mqtt_fixed_header->len;
}

uint8_t mqtt_sendMessage (char * buf, char * topic, char * message)
{
	mqtt_fixed_header_t * mqtt_fixed_header = (void*)buf;
	mqtt_flex_header_t  * mqtt_flex_header = (void*)mqtt_fixed_header->data;
	char * ptr = (void*)mqtt_flex_header->data;
	
	mqtt_fixed_header->msg_type = PUBLISH;
	mqtt_fixed_header->dup = 0;
	mqtt_fixed_header->qos = 0;
	mqtt_fixed_header->retain = 0;
	mqtt_fixed_header->len = sizeof(mqtt_fixed_header_t) + strlen(topic) + strlen(message);

	mqtt_flex_header->len = strlen(topic);
	mqtt_flex_header->flags = 0;	

	strcpy(ptr, topic);
	strcpy(ptr + strlen(topic), message);
	
	return mqtt_fixed_header->len;
}

