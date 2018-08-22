#include "ethernet.h"
#include <string.h>

#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
#define ETH_ARP be16toword(0x0806)
#define ETH_IP be16toword(0x0800)
#define ARP_ETH	be16toword(0x0001)
#define ARP_IP	be16toword(0x0800)
#define ARP_REQUEST	be16toword(1)
#define ARP_REPLY	be16toword(2)
#define IP_ICMP	1
#define IP_TCP	6
#define IP_UDP	17
#define ICMP_REQ	8
#define ICMP_REPLY	0

/* Local driver */
ethernet_t * ethernet;

/* Local functions */
void eth_send(frame_ptr *frame, uint16_t len);
void eth_read(frame_ptr *frame, uint16_t len);
uint8_t arp_read(frame_ptr *frame, uint16_t len);
void arp_send(frame_ptr *frame);
uint8_t ip_send(frame_ptr *frame, uint16_t len);
uint8_t ip_read(frame_ptr *frame, uint16_t len);
uint8_t icmp_read(frame_ptr *frame, uint16_t len);
uint16_t checksum(uint8_t *ptr, uint16_t len);

/* Public fuctions */
ethernet_t * ethernet_Init (ethernet_t * this)
{
	if (this) ethernet = this;
	
	return ethernet;
}

void net_poll(void)
{
	uint16_t len;
	frame_ptr *frame = (void*)ethernet->frame_buffer;
	
	while((len = ethernet->packetReceive(ethernet->frame_buffer, ethernet->frame_buffer_size)) > 0)
		eth_read(frame, len);
}

void eth_read(frame_ptr *frame, uint16_t len)
{
	if(len > sizeof(frame_ptr))
	{
		if(frame->type == ETH_ARP)
			if(arp_read(frame, len - sizeof(frame_ptr)))
				arp_send(frame);			
		
		if(frame->type == ETH_IP)	
			ip_read(frame, len - sizeof(ip_pkt_ptr));
	}
}


void eth_send(frame_ptr *frame, uint16_t len)
{
	memcpy(frame->addr_dest, frame->addr_src, 6);
	memcpy(frame->addr_src, ethernet->mac_address, 6);
	ethernet->packetSend((void*)frame, len + sizeof(frame_ptr));
}

uint8_t arp_read(frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	arp_msg_ptr *msg = (void*)(frame->data);
	if(len > sizeof(arp_msg_ptr))
		if((msg->net_tp == ARP_ETH) && (msg->proto_tp == ARP_IP))
			if((msg->op == ARP_REQUEST) && (!memcmp(msg->ipaddr_dst, ethernet->ipaddr, 4)))
				res = 1;
	return res;
}

void arp_send(frame_ptr *frame)
{
	arp_msg_ptr *msg=(void*)(frame->data);
	msg->op = ARP_REPLY;
	memcpy(msg->macaddr_dst, msg->macaddr_src, 6);
	memcpy(msg->macaddr_src, ethernet->mac_address, 6);
	memcpy(msg->ipaddr_dst, msg->ipaddr_src, 4);
	memcpy(msg->ipaddr_src, ethernet->ipaddr, 4);
	eth_send(frame, sizeof(arp_msg_ptr));
}

uint8_t ip_send(frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_ptr *ip_pkt = (void*)(frame->data);
	//fullfill header of IP package
	ip_pkt->len = be16toword(len);
	ip_pkt->fl_frg_of = 0;
	ip_pkt->ttl = 128;
	ip_pkt->cs = 0;
	memcpy(ip_pkt->ipaddr_dst, ip_pkt->ipaddr_src, 4);
	memcpy(ip_pkt->ipaddr_src, ethernet->ipaddr, 4);
	ip_pkt->cs = checksum((void*)ip_pkt, sizeof(ip_pkt_ptr));
	//send frame
	eth_send(frame, len);
	return res;
}

uint8_t ip_read(frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_ptr *ip_pkt = (void*)(frame->data);
	if((ip_pkt->verlen == 0x45) && (!memcmp(ip_pkt->ipaddr_dst, ethernet->ipaddr, 4)))
	{
		len = be16toword(ip_pkt->len) - sizeof(ip_pkt_ptr);
		if(ip_pkt->prt == IP_ICMP)
			icmp_read(frame, len);
		else if(ip_pkt->prt == IP_TCP)
		{
		}
		else if(ip_pkt->prt == IP_UDP)
		{
		}
	}
	return res;
}

uint8_t icmp_read(frame_ptr *frame, uint16_t len)
{
	uint8_t res = 0;
	ip_pkt_ptr *ip_pkt = (void*)(frame->data);
	icmp_pkt_ptr *icmp_pkt = (void*)(ip_pkt->data);
	if((len >= sizeof(icmp_pkt_ptr)) && (icmp_pkt->msg_tp == ICMP_REQ))
	{
		icmp_pkt->msg_tp = ICMP_REPLY;
		icmp_pkt->cs = 0;
		icmp_pkt->cs = checksum((void*)icmp_pkt, len);
		ip_send(frame, len + sizeof(ip_pkt_ptr));
	}
	return res;
}

uint16_t checksum(uint8_t *ptr, uint16_t len)
{
	uint32_t sum = 0;
	while(len > 0)
	{
		sum += (uint16_t) (((uint32_t)*ptr << 8) | *(ptr+1));
		ptr += 2;
		len -= 2;
	}
	if(len) sum += ((uint32_t)*ptr) << 8;
	while(sum >> 16) sum = (uint16_t)sum + (sum >> 16);
	return ~be16toword((uint16_t)sum);
}

