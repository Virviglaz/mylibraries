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
}mqtt_msg_type_t;

typedef struct
{
	mqtt_msg_type_t msg_type : 4;
	uint8_t flags : 4;
	uint8_t len;
}mqtt_fixed_header_t;

#endif
