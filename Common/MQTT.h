#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>

typedef enum
{
	RESERVED,		//Зарезервирован
	CONNECT,		//Запрос	клиента	на	подключение	к	серверу
	CONNACK,		//Подтверждение	успешного	подключения
	PUBLISH,		//Публикация	сообщения
	PUBACK,			//Подтверждение	публикации
	PUBREC,			//Публикация	получена
	PUBREL,			//Разрешение	на	удаление	сообщения
	PUBCOMP,		//Публикация	завершена
	SUBSCRIBE,	//Запрос	на	подписку
	SUBACK,			//Запрос	на	подписку	принят
	UNSUBSCRIBE,//Запрос	на	отписку
	UNSUBACK,		//Запрос	на	отписку	принят
	PINGREQ,		//PING	запрос
	PINGRESP,		//PING	ответ
	DISCONNECT,	//Сообщение	об	отключении	от	сервера
}mqtt_sendMessage_type_t;

typedef struct
{
	uint8_t retain : 1;
	uint8_t qos : 2;
	uint8_t dup : 1;
	uint8_t msg_type : 4;
	uint8_t len;
	uint8_t data[];
}mqtt_fixed_header_t;

typedef struct
{
	uint8_t flags;
	uint8_t len;
	char data[];
}mqtt_flex_header_t;

uint8_t mqtt_connect (char * buf, char * clientId, char * username, char * password);
uint8_t mqtt_sendMessage (char * buf, char * topic, char * message);
#endif
