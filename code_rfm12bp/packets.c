#include "packets.h"

#include <stdlib.h>
#include <stdio.h>

#include "samd21g18a/fiber.h"
#include "samd21g18a/console.h"

#define PACKETS_COUNT 8
#define PACKETS_ALLOC 6

static packet_t  packets_data[PACKETS_ALLOC] __attribute__((aligned(4)));
static packet_t* packets_ring[PACKETS_COUNT];
static uint_fast8_t packets_head;
static uint_fast8_t packets_tail;

void packets_init(void)
{
	packets_head = 0;
	packets_tail = 0;
	for (uint_fast8_t n = 0; n < PACKETS_ALLOC; n++)
		packets_put(packets_data[n].octet);
}

uint8_t* packets_get(void)
{
	uint_fast8_t head, tail; packet_t* packet;
	while (1) {
		head = packets_head;
		tail = packets_tail;
		if (head == tail) fiber_yield(); else break;
	}
	packet = packets_ring[tail];
	packets_ring[tail] = NULL;
	packets_tail = (tail+1) % PACKETS_COUNT;
	return packet->octet;
}

void packets_put(uint8_t* packet)
{
	uint_fast8_t head = packets_head;
	packets_ring[head] = (packet_t*) packet;
	packets_head = (head+1) % PACKETS_COUNT;
}

void packets_print(void)
{
	printf("%d %d\r\n", packets_head, packets_tail);
	for (uint8_t n = 0; n < PACKETS_COUNT; n++)
		printf("%p\r\n", packets_ring[n]);
}
CONSOLE_RUN(packets, packets_print)

const uint8_t packets_type[] = {
	IP_PN_INT,
	IP_PN_ESP,
	IP_PN_ICMP,
	IP_PN_TCP,
	IP_PN_UDP,
	IP_PN_AH,
//	IP_PN_AX25
};

uint8_t packets_type_ether(uint8_t type)
{
	return (type < sizeof(packets_type)) ? packets_type[type] : 0xFF;
}

uint8_t packets_type_radio(uint8_t type)
{
	for (uint8_t n = 0; n < sizeof(packets_type); n++) {
		if (packets_type[n] == type) return n;
	}
	return 0;
}
