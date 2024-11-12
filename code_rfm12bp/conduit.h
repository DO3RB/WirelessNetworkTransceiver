#pragma once

#include <stdint.h>

#ifndef CONDUIT_H
#define CONDUIT_H

#define CONDUIT_BYTE 1024
#define CONDUIT_FLAG (((CONDUIT_BYTE)+7)/8)

void conduit_open(void);
void conduit_close(void);

void conduit_init(void);

void conduit_wait(void);
uint8_t conduit_read(void);
uint8_t conduit_read_yield(void);
void conduit_write_yield(uint8_t byte);
int conduit_empty(void);
int conduit_count(void);
uint_fast16_t conduit_vdi(void);
void conduit_vdi_clear(void);

void conduit_test(void);
uint_fast16_t conduit_mark_dma(void);
extern uint_fast16_t conduit_mark_cpu;

#endif // CONDUIT_H
