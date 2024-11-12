#pragma once

#include <stdint.h>

#ifndef fiber_h
#define fiber_h

#define FIBER 8

enum fiber_tasks {V1=0,V2,V3,V4,V5,V6,SL,FP,SP,LR,RLEN};

typedef struct fiber {
   int   tasks[FIBER][RLEN]; // task control block
   int (*funcs[FIBER])(void);
  uint8_t now;
} fiber_t;

extern fiber_t fiber;

int8_t fiber_create(void *routine, uint8_t loop);
void fiber_once(void);
void fiber_loop(void);
void fiber_yield(void);
void fiber_print(void);
void fiber_watch(void);

typedef struct {
	unsigned int count;
	unsigned int value;
} stats_t;

void fiber_stats(stats_t* store, uint32_t value);

#endif // fiber_h
