#include "network.h"

#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "packets.h"
#include "channel.h"
#include "storage.h"

#include "samd21g18a/tinyusb_ecm.h"

void printp(uint8_t *buf, uint16_t len)
{
	for (uint16_t n = 0; n < len; n++) {
		write(STDOUT_FILENO, &"0123456789ABCDEF"[(buf[n]&0xF0)>>4] ,1);
		write(STDOUT_FILENO, &"0123456789ABCDEF"[ buf[n]&0x0F]     ,1);
	}	write(STDOUT_FILENO, &"\r\n", 2);
}

const uint8_t network_router_lts[4] = {0x00,0x00,0x1C,0x20};

uint8_t network_contains(uint8_t address[static NET_IP4_LEN])
{
	for (uint8_t n = 0; n < NET_IP4_LEN; n++) {
		if (storage_subnet_ip4[n] != (address[n] & storage_subnet_msk[n]))
			return 0;
	}
	return 1;
}

uint8_t network_routable(uint8_t address[static NET_IP4_LEN])
{
	for (uint8_t n = 0; n < NET_IP4_LEN; n++) {
		if (storage_subnet_ip4[n] != (address[n] & storage_subnet_fnk[n]))
			return 0;
	}
	return 1;
}

void network_loop(void)
{
	uint16_t recv_len = 0;
	uint16_t xmit_len = 0;
	packet_t* recv = (packet_t*) network_receive_yield(&recv_len);
	packet_t* xmit = (packet_t*) packets_get();
//	printp (recv->octet, recv_len);
	switch (recv->eth.typ) {
	case ETH_P_ARP:
		if (recv->arp.oper == ARP_OP_RQS && network_contains(recv->arp.tpa) && recv->arp.tpa[3] != storage_client_ip4()[3]) {
			memcpy(xmit->eth.dst, recv->eth.src, NET_ETH_LEN);
			memcpy(xmit->eth.src, storage_router_mac(), NET_ETH_LEN);
			xmit->eth.typ = ETH_P_ARP;
			xmit->arp.htyp = ARP_HT_ETH;
			xmit->arp.ptyp = ARP_PT_IP4;
			xmit->arp.hlen = NET_ETH_LEN;
			xmit->arp.plen = NET_IP4_LEN;
			xmit->arp.oper = ARP_OP_RSP;
			memcpy(xmit->arp.sha, storage_router_mac(), NET_ETH_LEN); // your mac on your ip?
			memcpy(xmit->arp.spa, recv->arp.tpa, NET_IP4_LEN);
			memcpy(xmit->arp.tha, recv->arp.sha, NET_ETH_LEN);
			memcpy(xmit->arp.tpa, recv->arp.spa, NET_IP4_LEN);
			xmit_len = sizeof(FrameETH_t) + sizeof(FrameARP_t);
//			printp(xmit->octet, xmit_len);
			network_transmit_yield(xmit->octet, xmit_len); xmit = NULL;
		}	break;
	case ETH_P_IP:
		//	recv->ip4.ihl equals five
		if (recv->ip4.typ == IP_PN_ICMP) {
			if (recv->icmp.type != ICMP_ECHO_RQS && recv->icmp.type != ICMP_ECHO_RPL) break; // discard
			if (recv->icmp.type == ICMP_ECHO_RQS && recv->ip4.dst[3] == storage_router_ip4()[3]) {
			memcpy(xmit->icmp.data, recv->icmp.data, xmit_len = recv_len - (recv->icmp.data - recv->octet));
			xmit->icmp.type = ICMP_ECHO_RPL;
			xmit->icmp.code = 0;
			xmit->icmp.csum = 0;
			xmit->icmp.iden = recv->icmp.iden;
			xmit->icmp.seqn = recv->icmp.seqn;
			xmit->icmp.csum = network_checksum(&xmit->icmp, xmit_len += sizeof(FrameICMP_t));
			xmit->ip4.vrs = 4;
			xmit->ip4.ihl = 5;
			xmit->ip4.tos = 0;
			xmit->ip4.len = __builtin_bswap16(xmit_len += sizeof(FrameIP4_t));
			xmit->ip4.idn = 0;
			xmit->ip4.frg = IP_NOFRAG;
			xmit->ip4.ttl = recv->ip4.ttl - 1;
			xmit->ip4.typ = IP_PN_ICMP;
			xmit->ip4.chk = 0;
			memcpy(xmit->ip4.src, recv->ip4.dst, NET_IP4_LEN);
			memcpy(xmit->ip4.dst, recv->ip4.src, NET_IP4_LEN);
			xmit->ip4.chk = network_checksum(&xmit->ip4, sizeof(FrameIP4_t));
			memcpy(xmit->eth.dst, recv->eth.src, NET_ETH_LEN);
			memcpy(xmit->eth.src, recv->eth.dst, NET_ETH_LEN);
			xmit->eth.typ = ETH_P_IP;
			xmit_len += sizeof(FrameETH_t);
		//	printp(xmit->octet, xmit_len);
			network_transmit_yield(xmit->octet, xmit_len); xmit = NULL;
			break;
		}}
		if (recv->ip4.typ == IP_PN_UDP &&
			recv->udp.dst == PORT_DHCP_SRV &&
			recv->dhcp.oper == DHCP_RQS) {
		//	printp(recv->octet, recv_len);
			uint8_t type = *dhcp_option(recv->dhcp.opts, DHCP_TYPE);
			if ((type != DHCP_DISCOVER) && (type != DHCP_REQUEST)) break;
		//	printp(recv->dhcp.opts, recv_len-(recv->dhcp.opts-recv->octet));
			uint8_t * opts = xmit->dhcp.opts;
			*opts++ = DHCP_TYPE;		*opts++ = 1; *opts++ = type == DHCP_DISCOVER ? DHCP_OFFER : DHCP_ACK;
			*opts++ = DHCP_SUBNETMASK;	*opts++ = 4; memcpy(opts, storage_subnet_msk  , 4); opts += 4;
		//	*opts++ = DHCP_DNSERVER;	*opts++ = 4; memcpy(opts, storage_router_ip4(), 4); opts += 4;
			*opts++ = DHCP_LEASETIME;	*opts++ = 4; memcpy(opts, network_router_lts  , 4); opts += 4;
			*opts++ = DHCP_SERVERID;	*opts++ = 4; memcpy(opts, storage_router_ip4(), 4); opts += 4;
		//	*opts++ = DHCP_MTU;			*opts++ = 2; *opts++= (uint8_t) (CFG_TUD_NET_MTU >> 8); *opts++= (uint8_t) CFG_TUD_NET_MTU;
		//	*opts++ = DHCP_BROADCAST;	*opts++ = 4; memcpy(opts, storage_subnet_brd  , 4); opts += 4;
			*opts++ = DHCP_END;
			xmit->dhcp.oper = DHCP_RPL;
			xmit->dhcp.htyp = DHCP_HTYPE;
			xmit->dhcp.hlen = NET_ETH_LEN;
			xmit->dhcp.hops = 0;
			xmit->dhcp.xidn = recv->dhcp.xidn;
			xmit->dhcp.secs = 0;
			xmit->dhcp.flag = 0;
			memset(xmit->dhcp.ciad, 0, NET_IP4_LEN);
			memcpy(xmit->dhcp.yiad, storage_client_ip4(), NET_IP4_LEN);
			memset(xmit->dhcp.siad, 0, NET_IP4_LEN);
			memset(xmit->dhcp.giad, 0, NET_IP4_LEN);
			memcpy(xmit->dhcp.chad, recv->dhcp.chad, 16); // NET_ETH_LEN
			memset(xmit->dhcp.lgcy, 0, 192);
			xmit->dhcp.magi = DHCP_MAGIC;
			// set udp pseudo header, use ip fields ttl protocol checksum
			xmit->udp.src = PORT_DHCP_SRV;
			xmit->udp.dst = PORT_DHCP_CLT;
			xmit->udp.len = __builtin_bswap16(xmit_len = opts - xmit->dhcp.opts + sizeof(FrameDHCP_t) + sizeof(FrameUDP_t));
			xmit->udp.chk = 0; // calc network_checksum(&xmit->ip4.ttl)
			xmit->ip4.vrs = 4;
			xmit->ip4.ihl = 5;
			xmit->ip4.tos = 0;
			xmit->ip4.len = __builtin_bswap16(xmit_len += sizeof(FrameIP4_t));
			xmit->ip4.idn = 0;
			xmit->ip4.frg = 0;
			xmit->ip4.ttl = 8;
			xmit->ip4.typ = IP_PN_UDP;
			xmit->ip4.chk = 0;
			memcpy(xmit->ip4.src, storage_router_ip4(), NET_IP4_LEN);
			memcpy(xmit->ip4.dst, recv->ip4.src, NET_IP4_LEN);
			xmit->ip4.chk = network_checksum(&xmit->ip4, sizeof(FrameIP4_t));
			if (recv->dhcp.flag & DHCP_BROAD)
					memset(xmit->eth.dst, 0xFF, NET_ETH_LEN);
			else	memcpy(xmit->eth.dst, recv->eth.src, NET_ETH_LEN);
			memcpy(xmit->eth.src, storage_router_mac(), NET_ETH_LEN);
			xmit->eth.typ = ETH_P_IP;
			xmit_len += sizeof(FrameETH_t);
			network_transmit_yield(xmit->octet, xmit_len); xmit = NULL;
			break;
		}
		if (network_routable(recv->ip4.dst)) { // route broadcast .31 to .0 or .15
			packet_frame_t * frame = &recv->radio.frame;
			if (recv->ip4.typ == IP_PN_ESP && recv->esp.spi == 0x01000000) {
				recv->ip4.typ =  IP_PN_INT;
				frame = (packet_frame_t*) (((uint32_t)&recv->radio.frame)+8);
				recv_len -= 8;
			}
			frame->dst = recv->ip4.dst[3];
			frame->src = recv->ip4.src[3];
			frame->typ = packets_type_radio(recv->ip4.typ);
			frame->len = recv_len - sizeof(FrameETH_t) - sizeof(FrameIP4_t);
			channel_transmit_yield(recv);
			recv = NULL;
			break;
		}
	}
	if (recv) packets_put(recv->octet);
	if (xmit) packets_put(xmit->octet);
}

uint16_t network_translate_ether(packet_t * packet)
{
	packet_frame_t frame;
	uint16_t length;
	if (packet->ip4.typ == IP_PN_INT) {
		frame = ((packet_t*)(((uint32_t)packet)+8))->radio.frame;
		length = frame.len + 8;
		packet->ip4.typ = IP_PN_ESP;
		packet->esp.spi = 0x01000000; // frame copied else
		packet->esp.sqn = 0x01000000; // overwritten here
	} else {
		frame = packet->radio.frame;
		length = frame.len;
		packet->ip4.typ = packets_type_ether(frame.typ);
	}
	packet->ip4.vrs = 4;
	packet->ip4.ihl = 5;
	packet->ip4.tos = 0;
	packet->ip4.len = __builtin_bswap16(length += sizeof(FrameIP4_t));
	packet->ip4.idn = 0;
	packet->ip4.frg = 0;
	packet->ip4.ttl = 8;
//	packet->ip4.typ = packets_type_ether(frame->typ);
	packet->ip4.chk = 0;
	packet->ip4.src[3] = frame.src;
	packet->ip4.src[2] = storage_subnet_ip4[2];
	packet->ip4.src[1] = storage_subnet_ip4[1];
	packet->ip4.src[0] = storage_subnet_ip4[0];
	packet->ip4.dst[3] = frame.dst;
	packet->ip4.dst[2] = storage_subnet_ip4[2];
	packet->ip4.dst[1] = storage_subnet_ip4[1];
	packet->ip4.dst[0] = storage_subnet_ip4[0];
	packet->ip4.chk = network_checksum(&packet->ip4, sizeof(FrameIP4_t));
	memcpy(packet->eth.dst, storage_client_mac(), NET_ETH_LEN);
	memcpy(packet->eth.src, storage_router_mac(), NET_ETH_LEN);
	packet->eth.typ = ETH_P_IP;
	return length + sizeof(FrameETH_t);
}

// https://datatracker.ietf.org/doc/html/rfc1071
uint16_t network_checksum(void *packet, uint16_t len)
{
	uint16_t* data = (uint16_t*) packet;
	uint32_t sum = 0;
	while (len > 1) { sum += *data++; len -= 2;	}
	if (len > 0) sum += *(uint8_t*)data;
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return ~sum;
}

uint8_t* dhcp_option(uint8_t *options, uint8_t type)
{
	do {
		if (*options == type) return options + 2;
		options += *(options+1) + 2;
	} while (*options != DHCP_END);
	return NULL;
}
