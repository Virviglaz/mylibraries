#include "ethernet.h"
#include <string.h>
//#include <stdio.h>

#define be16toword(a) ((((a) >> 8) & 0xff) | (((a) << 8) & 0xff00))
#define ETH_ARP be16toword(0x0806)
#define ETH_IP be16toword(0x0800)
#define ARP_ETH	be16toword(0x0001)
#define ARP_IP	be16toword(0x0800)
#define ARP_REQUEST	be16toword(1)
#define ARP_REPLY	be16toword(2)
#define IP_ICMP			1
#define IP_TCP			6
#define IP_UDP			17
#define ICMP_REQ		8
#define ICMP_REPLY	0
#define TCP_FIN			1
#define TCP_SYN			2
#define TCP_PSH			8
#define TCP_ACK			16
#define TELNET			23
#define HTTP				80

/* Local driver */
ethernet_t * ethernet;
uint16_t id = 0;

/* Local functions */
static void eth_send(eth_frame_t *frame, uint16_t len);
static void eth_read(eth_frame_t *frame, uint16_t len);
static uint8_t arp_read(eth_frame_t *frame, uint16_t len);
static void arp_send(eth_frame_t *frame);
static uint8_t ip_send(eth_frame_t *frame, uint16_t len, uint8_t prt);
static uint8_t ip_read(eth_frame_t *frame, uint16_t len);
static uint8_t icmp_read(eth_frame_t *frame, uint16_t len);
static uint16_t crc16(uint16_t *buf, uint16_t len);
static uint16_t tcp_crc16 (ip_pkt_t* ip_pkt);
static void udp_receive (uint8_t * buf, uint16_t len);
static void tcp_receive (uint8_t * buf, uint16_t len);
static uint32_t swap32 (uint32_t num);
static uint8_t dns_query_prep (char * dst, char * path);
static uint8_t * dns_query_parse (uint8_t * data);

/* Public fuctions */
ethernet_t * ethernet_Init (ethernet_t * this)
{
	if (this) ethernet = this;
	
	return ethernet;
}

void net_poll(void)
{
	uint16_t len;
	eth_frame_t *frame = (void*)ethernet->frame_buffer;
	
	while((len = ethernet->packetReceive(ethernet->frame_buffer, ethernet->frame_buffer_size)) > 0)
		eth_read(frame, len);
}

static void eth_read(eth_frame_t *frame, uint16_t len)
{
	if(len > sizeof(eth_frame_t))
	{
		if(frame->type == ETH_ARP)
			if(arp_read(frame, len - sizeof(eth_frame_t)))
				arp_send(frame);			
		
		if(frame->type == ETH_IP)	
			ip_read(frame, len - sizeof(ip_pkt_t));
	}
}

static void eth_send(eth_frame_t *frame, uint16_t len)
{
	memcpy(frame->addr_dest, frame->addr_src, 6);
	memcpy(frame->addr_src, ethernet->mac_address, 6);
	ethernet->packetSend((void*)frame, len + sizeof(eth_frame_t));
}

static uint8_t arp_read(eth_frame_t *frame, uint16_t len)
{
	uint8_t res = 0;
	arp_pkt_t *msg = (void*)frame->data;
	if(len > sizeof(arp_pkt_t))
		if((msg->net_tp == ARP_ETH) && (msg->proto_tp == ARP_IP))
			if((msg->op == ARP_REQUEST) && (!memcmp(msg->ipaddr_dst, ethernet->ipaddr, 4)))
				res = 1;
	return res;
}

static void arp_send(eth_frame_t *frame)
{
	arp_pkt_t *msg=(void*)frame->data;
	msg->op = ARP_REPLY;
	memcpy(msg->macaddr_dst, msg->macaddr_src, 6);
	memcpy(msg->macaddr_src, ethernet->mac_address, 6);
	memcpy(msg->ipaddr_dst, msg->ipaddr_src, 4);
	memcpy(msg->ipaddr_src, ethernet->ipaddr, 4);
	eth_send(frame, sizeof(arp_pkt_t));
}

static uint8_t ip_send(eth_frame_t *frame, uint16_t len, uint8_t prt)
{
	uint8_t res = 0;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	//fullfill header of IP package
	ip_pkt->verlen = 0x45;
	ip_pkt->ts = 0;
	ip_pkt->len = be16toword(len);
	ip_pkt->fl_frg_of = 0;
	ip_pkt->ttl = 128;
	ip_pkt->prt = prt;
	ip_pkt->cs = 0;
	memcpy(ip_pkt->ipaddr_dst, ip_pkt->ipaddr_src, 4);
	memcpy(ip_pkt->ipaddr_src, ethernet->ipaddr, 4);
	ip_pkt->cs = crc16((void*)ip_pkt, sizeof(ip_pkt_t));
	//send frame
	eth_send(frame, len);
	return res;
}

static uint8_t ip_read(eth_frame_t *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	if((ip_pkt->verlen == 0x45) && (!memcmp(ip_pkt->ipaddr_dst, ethernet->ipaddr, 4)))
	{
		len = be16toword(ip_pkt->len);
		frame->data[len] = 0;
		len -= sizeof(ip_pkt_t);
		switch (ip_pkt->prt)
		{
			case IP_ICMP:	icmp_read(frame, len); break;
			case IP_TCP:	if (ethernet->tcp_handler) tcp_receive(ip_pkt->data, ip_pkt->len); break;
			case IP_UDP:	if (ethernet->udp_handler) udp_receive(ip_pkt->data, ip_pkt->len); break;
		}
	}
	return res;
}

static uint8_t icmp_read(eth_frame_t *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	icmp_pkt_t *icmp_pkt = (void*)(ip_pkt->data);
	if (len < sizeof(icmp_pkt_t)) return 0;
	
	if (icmp_pkt->msg_tp == ICMP_REQ) //answer for incoming ping
	{
		icmp_pkt->msg_tp = ICMP_REPLY;
		icmp_pkt->cs = 0;
		icmp_pkt->cs = crc16((void*)icmp_pkt, len);
		ip_send(frame, len + sizeof(ip_pkt_t), IP_ICMP);
	}
	return res;
}

uint8_t * get_mac (uint8_t * ip_address, uint32_t timeout)
{
	uint32_t counter = 0;
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	
	arp_pkt_t * arp_pkt = (void*)frame->data;
	arp_pkt->net_tp = 0x0100;
	arp_pkt->proto_tp = 0x0008;
	arp_pkt->macaddr_len = 6;
	arp_pkt->ipaddr_len = 4;
	arp_pkt->op = 0x0100;
	memcpy(arp_pkt->macaddr_src, ethernet->mac_address, 6);
	memcpy(arp_pkt->ipaddr_src, ethernet->ipaddr, 4);
	memset(arp_pkt->macaddr_dst, 0x00, 6);
	memcpy(arp_pkt->ipaddr_dst, ip_address, 4);

	memcpy(frame->addr_src, ethernet->mac_address, 6);
	memset(frame->addr_dest, 0xFF, 6);
	frame->type = ETH_ARP;
	ethernet->packetSend((void*)frame, sizeof(arp_pkt_t) + sizeof(eth_frame_t));

	while(counter++ < timeout)
		if (ethernet->packetReceive(ethernet->frame_buffer, ethernet->frame_buffer_size) > 0)
			if (arp_pkt->op == 0x0200) return arp_pkt->macaddr_src;
	
	return 0;
}

uint32_t ping (uint8_t * ip_address, uint32_t timeout)
{
	const uint8_t payload[] = "Ping remote address.";
	uint8_t payload_len = strlen((void*)payload);
	uint32_t counter = 0;
	while(payload_len % 4) payload_len--;
	
	/* icmp -> ip -> ethernet */
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	icmp_pkt_t *icmp_pkt = (void*)ip_pkt->data;
	
	/* get phisical address */
	uint8_t * dest_address = get_mac(ip_address, timeout);
	if (dest_address == 0) return 0;
	
	/* prepare ethernet frame */
	frame->type = ETH_IP;
	
	memcpy(frame->addr_src, dest_address, 6);
	memcpy(ip_pkt->ipaddr_src, ip_address, 4);
	
	/* icmp */
	icmp_pkt->msg_tp = ICMP_REQ;
	icmp_pkt->msg_cd = 0x00;
	icmp_pkt->cs = 0;
	icmp_pkt->id = ++id;
	icmp_pkt->num = id;
	memcpy(icmp_pkt->data, (void*)payload, payload_len);
	icmp_pkt->cs = crc16((void*)icmp_pkt, sizeof(icmp_pkt_t) + payload_len);
	
	ip_send(frame, sizeof(icmp_pkt) + sizeof(ip_pkt_t) + payload_len + 4, IP_ICMP);
	
	while(counter++ < timeout)
		if (ethernet->packetReceive(ethernet->frame_buffer, ethernet->frame_buffer_size) > 0)
			if (icmp_pkt->msg_tp == ICMP_REPLY) return counter;
		
	return 0;
}

static void udp_receive (uint8_t * buf, uint16_t len)
{
	udp_pkt_t * udp_pkt = (void*)buf;
	
	len = be16toword(udp_pkt->len) - sizeof(udp_pkt_t);
	udp_pkt->data[len] = 0; //null terminate
	
	if (ethernet->udp_handler) ethernet->udp_handler((void*)udp_pkt->data, len);
}

static void tcp_receive (uint8_t * buf, uint16_t len)
{
	eth_frame_t * frame = (void*)ethernet->frame_buffer;		//ethernet frame
	ip_pkt_t * ip_pkt = (void*)frame->data;								//ip packet
	volatile tcp_pkt_t * tcp_pkt = (void*)ip_pkt->data;
	uint32_t seq;
	char * data;
		
	/* DATA SHIFT */
	len = (tcp_pkt->header_len >> 4) * 4; //TCP header len
	data = (void*)tcp_pkt;
	data += len;
	
	/* PORT */
	uint16_t port = tcp_pkt->dest_port;
	tcp_pkt->dest_port = tcp_pkt->source_port;
	tcp_pkt->source_port = port;
	
	/* ACK & SEQ */
	seq = tcp_pkt->seq_num;
	tcp_pkt->seq_num = tcp_pkt->ack_num;
	tcp_pkt->ack_num = seq;
	
	if ((tcp_pkt->flags & TCP_PSH) == 0) // HANDSHAKE & CLOSE CONNECTION
	{
		if (tcp_pkt->flags != TCP_ACK)
			tcp_pkt->ack_num = swap32(swap32(tcp_pkt->ack_num) + 1);
		if (tcp_pkt->flags & TCP_SYN) tcp_pkt->flags |= TCP_ACK;
		tcp_pkt->crc16 = 0;
		tcp_pkt->crc16 = tcp_crc16(ip_pkt);
		
		ip_send(frame, len + sizeof(ip_pkt_t), IP_TCP);
		return;		
	}
	
	len = be16toword(ip_pkt->len) - (tcp_pkt->header_len >> 4) * 4 - sizeof(ip_pkt_t);

	/* Handlers should be here */
	switch(be16toword(tcp_pkt->source_port))
	{
		case TELNET: if (ethernet->telnet_handler) tcp_send(ethernet->telnet_handler(data), 0); return;
		case HTTP: if (ethernet->http_handler) tcp_send(data, ethernet->http_handler(data, len)); return;
	}

	if (ethernet->tcp_handler) ethernet->tcp_handler(data, len); //undefined protocol
}

void udp_send (char * data, uint16_t len)
{
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	ip_pkt_t * ip_pkt = (void*)frame->data;
	udp_pkt_t * udp_pkt = (void*)ip_pkt->data;
	uint16_t port;
	if (len == 0) len = strlen(data);
	while(len % 2) len++;
	data[len] = 0;

	/* UDP */
	port = udp_pkt->source_port;
	udp_pkt->source_port = udp_pkt->dest_port;
	udp_pkt->dest_port = port;

	udp_pkt->len = be16toword(len + sizeof(udp_pkt_t));
	udp_pkt->crc16 = 0;
	//udp_pkt->crc16 = be16toword(tcp_crc16(ip_pkt));

	if (udp_pkt->data != (void*)data)
		memcpy(udp_pkt->data, (void*)data, len);
	
	ip_send(frame, len + sizeof(udp_pkt_t) + sizeof(ip_pkt_t), IP_UDP);
}

void tcp_send (char * data, uint16_t len)
{
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	ip_pkt_t * ip_pkt = (void*)frame->data;
	tcp_pkt_t* tcp_pkt = (void*)ip_pkt->data;
	char * buf = (void*)tcp_pkt;
	uint8_t tcp_header_len = (tcp_pkt->header_len >> 4) * 4;
	
	if (len == 0) len = strlen(data);
	buf += tcp_header_len;
	if (data != buf)
		memcpy(buf, data, len);
	while(len % 2) len++;
	data[len] = 0;
	
	ip_pkt->len = be16toword(len + tcp_header_len + sizeof(ip_pkt_t));
	
	if (be16toword(tcp_pkt->source_port) != TELNET)
		tcp_pkt->ack_num = swap32(swap32(tcp_pkt->ack_num) + len);
	tcp_pkt->crc16 = 0;
	tcp_pkt->crc16 = tcp_crc16(ip_pkt);
	
	ip_send(frame, sizeof(ip_pkt_t) + tcp_header_len + len, IP_TCP);
}

uint8_t * dns (uint8_t * dns_ip, char * url, uint32_t timeout)
{
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	ip_pkt_t * ip_pkt = (void*)frame->data;
	udp_pkt_t * udp_pkt = (void*)ip_pkt->data;
	dns_pkt_t * dns_pkt = (void*)udp_pkt->data;
	uint8_t * addr_src;
	char * dns_query = (void*)dns_pkt->dns_query;
	char * prt = dns_query;
	static uint16_t id = 0xAAAA;
	uint32_t counter = 0;
	if (url == 0) return 0;
	uint8_t url_len = strlen(url);
	if (url_len == 0 || strchr(url, '.') == 0) return 0;
	
	/* PHY */
	addr_src = get_mac(dns_ip, timeout);
	frame->type = ETH_IP;
	if (addr_src == 0) return 0;
	memcpy(frame->addr_src, addr_src, 6);
	
	/* IP */
	memcpy(ip_pkt->ipaddr_src, dns_ip, 4);
	ip_pkt->id = ++id;
	
	/* DNS */
	memset(dns_pkt, 0, sizeof(dns_pkt_t));
	dns_pkt->id = id;
	dns_pkt->flags = 0x0001; //standart query
	dns_pkt->qd_count = 0x0100;

	/* DNS QUERY */
	dns_query += dns_query_prep(dns_query, url);
	*dns_query++ = 0;
	*dns_query++ = 0; //QT TYPE
	*dns_query++ = 1;
	*dns_query++ = 0;	//QT CLASS
	*dns_query++ = 1;
	
	/* UDP */
	udp_pkt->source_port = be16toword(53);
	udp_pkt->dest_port = be16toword(53);
	udp_pkt->crc16 = 0;
	udp_pkt->len = sizeof(dns_pkt_t) + sizeof(udp_pkt_t) + strlen(url) + (dns_query - prt) ;
		
	udp_send((void*)udp_pkt->data, udp_pkt->len);
	
	while(counter++ < timeout)
		if (ethernet->packetReceive(ethernet->frame_buffer, ethernet->frame_buffer_size) > 0)
			if (id == dns_pkt->id)
				return dns_query_parse((void*)dns_pkt);
				
	return 0;
}

static uint16_t crc16(uint16_t * buf, uint16_t len)
{
    uint32_t sum = 0;
    while (len > 1)
    {
        sum += *buf++;
        len -= sizeof(uint16_t);
    }

    if (len)
        sum += *(uint8_t*)buf;
      
    sum = (sum >> 16) + (sum & 0xffff);
    sum += sum >> 16;

    return ~sum;
}

static uint16_t tcp_crc16 (ip_pkt_t* ip_pkt)
{
	uint32_t sum = 0;
	uint16_t i;
	tcp_pseudo_header_t tcp_pseudo_header;
	uint16_t * ptr = (void*)&tcp_pseudo_header;
	
	/* pseudo prepare */
	tcp_pseudo_header.ipaddr_dst = ip_pkt->ipaddr_dst[2] << 24 | ip_pkt->ipaddr_dst[3] << 16 | ip_pkt->ipaddr_dst[0] << 8 | ip_pkt->ipaddr_dst[1];
	tcp_pseudo_header.ipaddr_src = ip_pkt->ipaddr_src[2] << 24 | ip_pkt->ipaddr_src[3] << 16 | ip_pkt->ipaddr_src[0] << 8 | ip_pkt->ipaddr_src[1];
	tcp_pseudo_header.zero = 0;
	tcp_pseudo_header.len = be16toword(ip_pkt->len) - sizeof(ip_pkt_t);
	tcp_pseudo_header.prt = ip_pkt->prt;

	/* pseudo crc calculation */
	for (i = 0; i < sizeof(tcp_pseudo_header_t); i += sizeof(uint16_t))
	{
		uint16_t value = *ptr++;
		sum += be16toword(value);
	}

	ptr = (void*)ip_pkt->data; 

	for (i = 0; i < tcp_pseudo_header.len; i += sizeof(uint16_t))	
		sum += *ptr++;
	
	sum = (sum >> 16) + (sum & 0xffff);
	sum += sum >> 16;

	return ~sum;
}	

static uint32_t swap32 (uint32_t num)
{
 return ((num>>24)&0xff) | // move byte 3 to byte 0
                    ((num<<8)&0xff0000) | // move byte 1 to byte 2
                    ((num>>8)&0xff00) | // move byte 2 to byte 1
                    ((num<<24)&0xff000000); // byte 0 to byte 3
}

static uint8_t dns_query_prep (char * dst, char * path)
{
		const char delim[2] = ".";
    char *token = strtok(path, delim), *ptr = dst;
    
    while(token)
    {
        *dst++ = strlen(token);
        strcpy(dst, token);
        dst += strlen(token);
        token = strtok(NULL, delim);
    }

    return dst - ptr;
}

static uint8_t * dns_query_parse (uint8_t * data)
{
    dns_pkt_t * dns_pkt = (void*)data;
    uint8_t * ptr = dns_pkt->dns_query;
    
    if (be16toword(dns_pkt->qd_count) != 1) return 0; //only one question supported
    
    while(*ptr++){} // search for end of question
    ptr += 2 + 14; //CLASS = 0x0001, NAME 2, TYPE 2, CLASS 2, TIME 4, LEN 2
    
    return ptr;
}
