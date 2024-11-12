#pragma once

#ifndef CHANNEL
#define CHANNEL

#include "packets.h"

void channel_init(void);
void channel_task(void);
void channel_transmit(packet_t *packet);
void channel_transmit_yield(packet_t *packet);
int  channel_receive_yield(packet_t *packet);

#endif // CHANNEL
