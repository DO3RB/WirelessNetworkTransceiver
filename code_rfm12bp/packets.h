#pragma once

#ifndef PACKETS
#define PACKETS

#include <stdint.h>

#include "tinyusb_ecm.h"

#define NET_ETH_LEN 6
#define NET_IP4_LEN 4

#define ETH_P_IP    0x0008 // Internet Protocol packet
#define ETH_P_ARP   0x0608 // Address Resolution packet
#define ETH_P_RARP  0x3580 // Reverse Addr Res packet
#define ETH_P_AX25  0x0200 // Dummy protocol id for AX.25

// https://en.wikipedia.org/wiki/Ethernet_frame
typedef struct FrameETH {
    uint8_t  dst[NET_ETH_LEN];
    uint8_t  src[NET_ETH_LEN];
    uint16_t typ;
} __attribute__((packed)) FrameETH_t;

#define ARP_HT_ETH 0x0100
#define ARP_PT_IP4 0x0008
#define ARP_OP_RQS 0x0100
#define ARP_OP_RSP 0x0200

// https://datatracker.ietf.org/doc/html/rfc826
typedef struct FrameARP {
    uint16_t htyp; // hardware type
    uint16_t ptyp; // protocol type
    uint8_t  hlen; // hardware length
    uint8_t  plen; // protocol length
    uint16_t oper; // operation
    uint8_t  sha[NET_ETH_LEN]; // sender hardware address
    uint8_t  spa[NET_IP4_LEN]; // sender protocol address
    uint8_t  tha[NET_ETH_LEN]; // target hardware address
    uint8_t  tpa[NET_IP4_LEN]; // target protocol address
} __attribute__((packed)) FrameARP_t;

#define IP_PN_ICMP 0x01
#define IP_PN_TCP  0x06
#define IP_PN_UDP  0x11
#define IP_PN_ESP  0x32
#define IP_PN_AH   0x33
#define IP_PN_INT  0x3D
#define IP_PN_AX25 0x5D
#define IP_NOFRAG  0x0040

// https://datatracker.ietf.org/doc/html/rfc791#section-3.1
typedef struct FrameIP4 {
    uint8_t  ihl:4; // internet header length
    uint8_t  vrs:4; // version
    uint8_t  tos;   // type of service dscp:6 ecn:2
    uint16_t len;   // total length
    uint16_t idn;   // fragment identification
    uint16_t frg;   // fragment flags:3 offset:13
    uint8_t  ttl;   // time to live
    uint8_t  typ;   // protocol
    uint16_t chk;   // header checksum
    uint8_t  src[NET_IP4_LEN];
    uint8_t  dst[NET_IP4_LEN];
} __attribute__((packed)) FrameIP4_t;

#define ICMP_ECHO_RPL 0x00
#define ICMP_ECHO_RQS 0x08

// https://datatracker.ietf.org/doc/html/rfc792
typedef struct FrameICMP {
    uint8_t  type;
    uint8_t  code;
    uint16_t csum;
    union {
        uint32_t rest;
        struct {uint16_t iden; uint16_t seqn;};
    };
    uint8_t  data[];
} __attribute__((packed)) FrameICMP_t;

#define PORT_DHCP_SRV 0x4300 // 67
#define PORT_DHCP_CLT 0x4400 // 68

// https://datatracker.ietf.org/doc/html/rfc768
typedef struct FrameUDP {
    uint16_t src;   // souce port
    uint16_t dst;   // destination port
    uint16_t len;   // length
    uint16_t chk;   // pseudo header checksum
} __attribute__((packed)) FrameUDP_t;

#define DHCP_RQS 1
#define DHCP_RPL 2
#define DHCP_HTYPE 0x01
#define DHCP_MAGIC 0x63538263
#define DHCP_BROAD 0x0080

// https://datatracker.ietf.org/doc/html/rfc2131
typedef struct FrameDHCP {
    uint8_t  oper;      // packet opcode type
    uint8_t  htyp;      // hardware addr type
    uint8_t  hlen;      // hardware addr length
    uint8_t  hops;      // gateway hops
    uint32_t xidn;      // transaction ID
    uint16_t secs;      // seconds since boot began
    uint16_t flag;
    uint8_t  ciad[4];   // client IP address
    uint8_t  yiad[4];   // 'your' IP address
    uint8_t  siad[4];   // server IP address
    uint8_t  giad[4];   // gateway IP address
    uint8_t  chad[16];  // client hardware address
    uint8_t  lgcy[192]; // legacy sname and file
    uint32_t magi;      // magic number
    uint8_t  opts[];
} __attribute__((packed)) FrameDHCP_t;

// https://datatracker.ietf.org/doc/html/rfc2132
enum DHCP_OPTS {
    DHCP_PAD = 0,
    DHCP_SUBNETMASK = 1,
    DHCP_DNSERVER = 6,
    DHCP_MTU = 26,
    DHCP_BROADCAST = 28,
    DHCP_LEASETIME = 51,
    DHCP_TYPE = 53,
    DHCP_SERVERID = 54,
    DHCP_END = 255
};

// https://datatracker.ietf.org/doc/html/rfc2132
enum DHCP_MESG {
    DHCP_DISCOVER = 1,
    DHCP_OFFER,
    DHCP_REQUEST,
    DHCP_DECLINE,
    DHCP_ACK,
    DHCP_NAK,
    DHCP_RELEASE,
    DHCP_INFORM
};

// https://datatracker.ietf.org/doc/html/rfc4303
typedef struct FrameESP {
    uint32_t spi; // security parameters index
    uint32_t sqn; // sequence number
} __attribute__((packed)) FrameESP_t;

typedef struct {
    uint8_t  typ:4;
    uint8_t  src:4;
    uint8_t  dst:4;
    uint16_t len:12;
} __attribute__((packed,scalar_storage_order("big-endian"))) packet_frame_t;

typedef struct {
    uint16_t one:12;
    uint16_t two:12;
} __attribute__((packed,scalar_storage_order("big-endian"))) packet_block_t;

#define PACKET_RADIO_LEN (CFG_TUD_NET_MTU-sizeof(FrameETH_t)-sizeof(FrameIP4_t))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wscalar-storage-order"

typedef union {
	packet_frame_t frame;
	packet_block_t block;
} packet_radio_t;

//	arrange following into tree of structs and unions
//	{ETH,ARP} {ETH,IP4,OCTET} {ETH,IP4,ICMP} {ETH,IP4,UDP,DHCP} {EMPTY,RADIO,BLOCKS}
typedef union {
	struct { // ETHER
		FrameETH_t eth;
		union {
		FrameARP_t arp;
		struct {
		FrameIP4_t ip4;
		union {
			FrameESP_t  esp;
			FrameICMP_t icmp;
			struct {
			FrameUDP_t  udp;
			FrameDHCP_t dhcp;
			} __attribute__((packed));
		};} __attribute__((packed));
	};} __attribute__((packed));
	struct { // RADIO
		uint8_t empty[sizeof(FrameETH_t)+sizeof(FrameIP4_t)-sizeof(packet_frame_t)];
		union {
		packet_frame_t frame;
		packet_block_t block[PACKET_RADIO_LEN/sizeof(packet_block_t)+1];
		};
	} __attribute__((packed)) radio;
	uint8_t octet[CFG_TUD_NET_MTU];
} packet_t;

#pragma GCC diagnostic pop

void packets_init(void);
uint8_t* packets_get(void);
void packets_put(uint8_t* packet);
void packets_print(void);

uint8_t packets_type_ether(uint8_t type);
uint8_t packets_type_radio(uint8_t type);

#endif // PACKETS
