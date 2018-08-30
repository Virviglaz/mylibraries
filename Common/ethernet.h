#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>

typedef struct
{
  uint8_t addr_dest[6];
  uint8_t addr_src[6];
  uint16_t type;
  uint8_t data[];
}eth_frame_t; //14 bytes

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
}ip_pkt_t; //20 bytes

typedef struct
{
	uint8_t msg_tp;//тип севриса
	uint8_t msg_cd;//код сообщения
	uint16_t cs;//контрольная сумма заголовка
	uint16_t id;//идентификатор пакета
	uint16_t num;//номер пакета
	uint8_t data[];//данные
}icmp_pkt_t; //8 bytes

/* UDP package */
typedef struct
{
	uint16_t source_port;
	uint16_t dest_port;
	uint16_t len;
	uint16_t crc16;
	uint8_t data[];
}udp_pkt_t; //8 bytes

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
}tcp_pkt_t; //24 bytes

typedef struct
{
	uint32_t ipaddr_src;
	uint32_t ipaddr_dst;
	uint8_t prt;
	uint8_t zero;
	uint16_t len;
}tcp_pseudo_header_t; //12 bytes

typedef struct 
{
	uint16_t (* packetReceive) (uint8_t* buf, uint16_t buflen);
	void (* packetSend) (uint8_t* buf, uint16_t buflen);
	uint8_t ipaddr[4];
	uint8_t * mac_address;
	uint8_t * frame_buffer;
	uint16_t frame_buffer_size;
	void (* udp_handler) (char * buf, uint16_t len);
	void (* tcp_handler) (char * buf, uint16_t len);
	char * (* telnet_handler) (char* buf);
	uint16_t (* http_handler) (char * buf, uint16_t len);
}ethernet_t;

typedef struct
{
	uint16_t id;
	uint16_t flags;
	uint16_t qd_count;
	uint16_t ad_count;
	uint16_t ns_count;
	uint16_t ar_count;
	uint8_t dns_query[];
}dns_pkt_t;

/* Public fuctions prototypes */
ethernet_t * ethernet_Init (ethernet_t * this);
void net_poll(void);
uint8_t * get_mac (uint8_t * ip_address, uint32_t timeout);
uint32_t ping (uint8_t * ip_address, uint32_t timeout);
void udp_send (char * data, uint16_t len);
void tcp_send (char * data, uint16_t len);
uint8_t * dns (uint8_t * dns_ip, char * url, uint32_t timeout);
#endif
