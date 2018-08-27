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

/* Local driver */
ethernet_t * ethernet;
uint16_t id = 0;

/* Local functions */
static void eth_send(eth_frame_t *frame, uint16_t len);
static void eth_read(eth_frame_t *frame, uint16_t len);
static uint8_t arp_read(eth_frame_t *frame, uint16_t len);
static void arp_send(eth_frame_t *frame);
static uint8_t ip_send(eth_frame_t *frame, uint16_t len);
static uint8_t ip_read(eth_frame_t *frame, uint16_t len);
static uint8_t icmp_read(eth_frame_t *frame, uint16_t len);
static uint16_t crc16(uint16_t *buf, uint16_t len);
static uint16_t tcp_crc16 (ip_pkt_t* ip_pkt);
static udp_packet_t * udp_receive (uint8_t * buf, uint16_t len);
static char * tcp_receive (uint8_t * buf, uint16_t len);
static uint32_t swap32 (uint32_t num);

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

static uint8_t ip_send(eth_frame_t *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	//fullfill header of IP package
	ip_pkt->len = be16toword(len);
	ip_pkt->fl_frg_of = 0;
	ip_pkt->ttl = 128;
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
		len = be16toword(ip_pkt->len) - sizeof(ip_pkt_t);
		switch (ip_pkt->prt)
		{
			case IP_ICMP:	icmp_read(frame, len); break;
			case IP_TCP:	if (ethernet->tcp_handler) ethernet->tcp_handler(tcp_receive(ip_pkt->data, ip_pkt->len)); break;
			case IP_UDP:	if (ethernet->udp_handler) ethernet->udp_handler(udp_receive(ip_pkt->data, ip_pkt->len)); break;
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
		ip_send(frame, len + sizeof(ip_pkt_t));
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
	memcpy(frame->addr_dest, dest_address, 6);	
	memcpy(frame->addr_src, ethernet->mac_address, 6);	
	frame->type = ETH_IP;
	
	/* ip */
	ip_pkt->verlen = 0x45;
	ip_pkt->ts = 0x00;
	ip_pkt->len = be16toword(sizeof(icmp_pkt) + sizeof(ip_pkt_t) + payload_len + 4);
	ip_pkt->id = ++id;
	ip_pkt->fl_frg_of = 0x0000;
	ip_pkt->ttl = 128;
	ip_pkt->prt = IP_ICMP;
	ip_pkt->cs = 0;
	memcpy(ip_pkt->ipaddr_src, ethernet->ipaddr, 4);
	memcpy(ip_pkt->ipaddr_dst, ip_address, 4);
	ip_pkt->cs = crc16((void*)ip_pkt, sizeof(ip_pkt_t));
	
	/* icmp */
	icmp_pkt->msg_tp = ICMP_REQ;
	icmp_pkt->msg_cd = 0x00;
	icmp_pkt->cs = 0;
	icmp_pkt->id = id;
	icmp_pkt->num = id;
	memcpy(icmp_pkt->data, (void*)payload, payload_len);
	icmp_pkt->cs = crc16((void*)icmp_pkt, sizeof(icmp_pkt_t) + payload_len);
	
	ethernet->packetSend((void*)frame, sizeof(icmp_pkt) + sizeof(ip_pkt_t) + sizeof(eth_frame_t) + payload_len + 4);
	
	while(counter++ < timeout)
		if (ethernet->packetReceive(ethernet->frame_buffer, ethernet->frame_buffer_size) > 0)
			if (icmp_pkt->msg_tp == ICMP_REPLY) return counter;
		
	return 0;
}

static udp_packet_t * udp_receive (uint8_t * buf, uint16_t len)
{
	udp_packet_t* res = (void*)buf;

	res->source_port = be16toword(res->source_port);
	res->dest_port = be16toword(res->dest_port);
	res->len = be16toword(res->len) - sizeof(udp_packet_t);
	res->data[res->len] = 0; //null terminate
	
	return res;
}

static char * tcp_receive (uint8_t * buf, uint16_t len)
{
	eth_frame_t *frame = (void*)ethernet->frame_buffer;		//ethernet frame
	ip_pkt_t* ip_pkt = (void*)frame->data;								//ip packet
	volatile tcp_pkt_t* tcp_pkt = (void*)ip_pkt->data;
	uint32_t seq;
	char * data;
		
	/* IP prepare */
	memcpy(ip_pkt->ipaddr_dst, ip_pkt->ipaddr_src, 4);
	memcpy(ip_pkt->ipaddr_src, ethernet->ipaddr, 4);	
	
	len = (tcp_pkt->header_len >> 4) * 4;
	data = (void*)tcp_pkt;
	data += len;
	
	ip_pkt->len = be16toword(len + sizeof(ip_pkt_t));
	ip_pkt->prt = IP_TCP;
	ip_pkt->cs = 0;
	ip_pkt->cs = crc16((void*)ip_pkt, sizeof(ip_pkt_t));
	
	uint16_t port = tcp_pkt->dest_port;
	tcp_pkt->dest_port = tcp_pkt->source_port;
	tcp_pkt->source_port = port;
	
	seq = tcp_pkt->seq_num;
	tcp_pkt->seq_num = tcp_pkt->ack_num;
	tcp_pkt->ack_num = seq;
	
	len += sizeof(ip_pkt_t);
	
	if (tcp_pkt->flags & TCP_FIN) //FIN & ACK (CLOSE CONNECTION)
	{
		tcp_pkt->ack_num = swap32(swap32(tcp_pkt->ack_num) + 1);
		tcp_pkt->flags = TCP_FIN | TCP_ACK;
		tcp_pkt->crc16 = tcp_crc16(ip_pkt);
		
		eth_send((void*)frame, len);
		return 0;
	}
	
	if (tcp_pkt->flags & TCP_SYN) //SYN & ACK (HANDSHAKE)
	{
		tcp_pkt->ack_num = swap32(swap32(tcp_pkt->ack_num) + 1);
		tcp_pkt->flags = TCP_SYN | TCP_ACK;
		tcp_pkt->crc16 = tcp_crc16(ip_pkt);

		eth_send((void*)frame, len);
		return 0;
	}
	
	return tcp_pkt->flags & TCP_PSH ? data : 0;
}

void udp_send (udp_packet_t * udp_packet)
{
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	uint16_t port, len = udp_packet->len + sizeof(ip_pkt_t) + sizeof(udp_packet_t);
	
	ip_pkt->prt = IP_UDP;
	ip_pkt->verlen = 0x45;
	
	port = udp_packet->source_port;
	udp_packet->source_port = udp_packet->dest_port;
	udp_packet->dest_port = port;
	
	udp_packet->source_port = be16toword(udp_packet->source_port);
	udp_packet->dest_port = be16toword(udp_packet->dest_port);
	udp_packet->len = be16toword(udp_packet->len + sizeof(udp_packet_t));
	udp_packet->crc16 = 0;

	if (ip_pkt->data != (void*)udp_packet)
		memcpy(ip_pkt->data, (void*)udp_packet, udp_packet->len + sizeof(udp_packet_t));
	
	ip_send(frame, len);
}

void tcp_send (char * data)
{
	eth_frame_t * frame = (void*)ethernet->frame_buffer;
	ip_pkt_t *ip_pkt = (void*)frame->data;
	volatile tcp_pkt_t* tcp_pkt = (void*)ip_pkt->data;
	char * buf = (void*)tcp_pkt;
	uint16_t data_len = strlen(data);
	uint8_t tcp_header_len = (tcp_pkt->header_len >> 4) * 4;
	
	buf += tcp_header_len;

	ip_pkt->len = be16toword(data_len + tcp_header_len + sizeof(ip_pkt_t));
	ip_pkt->prt = IP_TCP;
	ip_pkt->cs = 0;
	ip_pkt->cs = crc16((void*)ip_pkt, sizeof(ip_pkt_t));
	
	if (data != buf)
		strcpy(buf, data);
	
	tcp_pkt->ack_num = swap32(swap32(tcp_pkt->ack_num) + data_len);	
	tcp_pkt->crc16 = tcp_crc16(ip_pkt);
	
	eth_send((void*)frame, sizeof(ip_pkt_t) + tcp_header_len + data_len);
}

static uint16_t crc16(uint16_t *buf, uint16_t len)
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
	tcp_pkt_t* tcp_pkt = (void*)ip_pkt->data;
	
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

	/* TCP header */
	ptr = (void*)ip_pkt->data; 
	tcp_pkt->crc16 = 0;

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
