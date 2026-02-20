#pragma once

#include <stdint.h>

#ifndef FIBER_H
#define FIBER_H

#define FIBER 8

enum fiber_tasks {V1=0,V2,V3,V4,V5,V6,SL,FP,SP,LR,RLEN};

typedef int jmp_buf[RLEN];
int setjmp [[gnu::returns_twice]] (jmp_buf env);
void longjmp [[noreturn]] (jmp_buf env, int val);

typedef struct fiber {
   jmp_buf tasks[FIBER]; // task control block
   int   (*funcs[FIBER])(void);
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

void fiber_idle_init();
void fiber_idle_inhibit();
void fiber_idle_wfi();
void fiber_idle_wfe();

uint32_t xoshiro128plusplus (void);
uint32_t xoshiro128starstar (void);
void entropy_feed ();

#endif // FIBER_H
