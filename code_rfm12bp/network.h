#pragma once

#ifndef NETWORK
#define NETWORK

#include <stdint.h>
#include "packets.h"

void printp(uint8_t *buf, uint16_t len);
uint8_t network_contains(uint8_t address[static NET_IP4_LEN]);
void network_loop(void);
uint16_t network_translate_ether(packet_t * packet);
uint16_t network_checksum(void *packet, uint16_t len);
uint8_t* dhcp_option(uint8_t *options, uint8_t type);

#endif // NETWORK
