#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>

typedef struct
{
  uint8_t addr_dest[6];
  uint8_t addr_src[6];
  uint16_t type;
  uint8_t data[];
}eth_frame_t;

typedef struct
{
  uint16_t net_tp;
  uint16_t proto_tp;
  uint8_t macaddr_len;
  uint8_t ipaddr_len;
  uint16_t op;
  uint8_t macaddr_src[6];
  uint8_t ipaddr_src[4];
  uint8_t macaddr_dst[6];
  uint8_t ipaddr_dst[4];
}arp_pkt_t;

typedef struct
{
	uint8_t verlen;//версия протокола и длина заголовка
	uint8_t ts;//тип севриса
	uint16_t len;//длина
	uint16_t id;//идентификатор пакета
	uint16_t fl_frg_of;//флаги и смещение фрагмента
	uint8_t ttl;//время жизни
	uint8_t prt;//тип протокола
	uint16_t cs;//контрольная сумма заголовка
	uint8_t ipaddr_src[4];//IP-адрес отправителя
	uint8_t ipaddr_dst[4];//IP-адрес получателя
	uint8_t data[];//данные
}ip_pkt_t;

typedef struct
{
	uint8_t msg_tp;//тип севриса
	uint8_t msg_cd;//код сообщения
	uint16_t cs;//контрольная сумма заголовка
	uint16_t id;//идентификатор пакета
	uint16_t num;//номер пакета
	uint8_t data[];//данные
}icmp_pkt_t;

/* UDP package */
typedef struct
{
	uint16_t source_port;
	uint16_t dest_port;
	uint16_t len;
	uint16_t crc16;
	uint8_t data[];
}udp_paket_t;

/* TCP package */
typedef struct
{
	uint16_t source_port;
	uint16_t dest_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint8_t header_len;
	uint8_t flags;
	uint16_t window_size;
	uint16_t crc16;
	uint16_t priority;
	uint32_t options;
	uint8_t data[];
}tcp_packet_t;


typedef struct 
{
	uint16_t (* packetReceive) (uint8_t* buf, uint16_t buflen);
	void (* packetSend) (uint8_t* buf, uint16_t buflen);
	uint8_t ipaddr[4];
	uint8_t * mac_address;
	uint8_t * frame_buffer;
	uint16_t frame_buffer_size;
	void (* udp_handler) (udp_paket_t * udp_packet);
	//void (* tcp_handler) (uint8_t* buf, uint16_t buflen);	
}ethernet_t;

/* Public fuctions prototypes */
ethernet_t * ethernet_Init (ethernet_t * this);
void net_poll(void);
uint8_t * get_mac (uint8_t * ip_address, uint32_t timeout);
uint32_t ping (uint8_t * ip_address, uint32_t timeout);
void udp_send (udp_paket_t * udp_packet);
#endif
