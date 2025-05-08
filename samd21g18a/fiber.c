#include "fiber.h"

#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "sam.h"
#include "console.h"

extern int main(void);

fiber_t fiber = {.funcs={[0]=&main}};

int8_t fiber_create(void *routine, uint8_t loop)
{
	extern uint32_t __stack; int8_t nxt = -1;
	for (uint8_t n=0; n<FIBER; n++) {if (!fiber.funcs[n]) nxt = n;}
	if (nxt < 0) return -1; // pick last free slot as nxt
	fiber.tasks[nxt][SP] = (int) &__stack - 1024 - nxt*1024;
	fiber.tasks[nxt][LR] = (int) (loop ? &fiber_loop : &fiber_once);
	fiber.funcs[nxt] = routine;
	return nxt;
}

__attribute__((naked,noreturn))
void fiber_once(void)
{
	fiber.funcs[fiber.now]();
	fiber.funcs[fiber.now] = 0;
	fiber_yield();
}

__attribute__((naked,noreturn))
void fiber_loop(void)
{
	while (1) {
		fiber.funcs[fiber.now]();
		fiber_yield();
	}
}

void fiber_yield(void)
{
	uint8_t now = fiber.now, nxt = now;
	for (unsigned n = 0; n < FIBER; n++) {
		nxt = (nxt+1) % FIBER;
		if (fiber.funcs[nxt] == NULL) continue;
		if ((nxt == now) || setjmp((long long int*)fiber.tasks[now])) return;
		else { fiber.now = nxt; longjmp((long long int*)fiber.tasks[nxt],1); }
	}
}

void fiber_exit(void)
{
	fiber.funcs[fiber.now] = 0;
	fiber_yield();
}

void fiber_change(void *routine, uint8_t loop)
{
	fiber.funcs[fiber.now] = 0;
	fiber_create(routine, loop);
	fiber_yield();
}

void fiber_print(void)
{
	for (uint8_t n = 0; n < FIBER; n++)
		printf("%d %05X %08X\r\n", n, (unsigned int) fiber.funcs[n], (unsigned int) fiber.tasks[n][SP]);
}
CONSOLE_RUN(fiber, fiber_print)

void fiber_watch(void)
{
	if (!REG_WDT_STATUS) REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
}

void fiber_stats(stats_t* store, uint32_t v)
{
	unsigned int count, value;
	if (
		__builtin_uadd_overflow(store->count, 1, &count) ||
		__builtin_uadd_overflow(store->value, v, &value)
	)  {
//		printf("%f\r\n", (float) store->value / (float) store->count);
		store->count = 0;
		store->value = 0;
	} else {
		store->count = count;
		store->value = value;
	}
	if (count == 500000) {
		printf("%d\r\n", (store->value*10000)/ store->count);
	}
}

void fiber_idle_init()
{
	// Send Event on Pending interrupt
	SCB->SCR |= SCB_SCR_SEVONPEND_Msk;
}

void fiber_idle_wfi()
{
	asm volatile ("cpsid i \n dsb 0xF \n wfi");
//	REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
	if (!REG_WDT_STATUS) REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
	REG_WDT_INTFLAG = 1;
	NVIC_ClearPendingIRQ(WDT_IRQn);
//	NVIC->ICPR[0] = 1u << WDT_IRQn;
	asm volatile ("cpsie i");
}

void fiber_idle_inhibit() { asm volatile ("sev"); }

void fiber_idle_wfe()
{
	asm volatile (
//	"cpsid	i	\n" // disable ISRs
	"dsb	0xF	\n" // complete all explicit memory accesses
	"wfe		\n" // Wait For Event
///	"wfi		\n" // Wait For Interrupt
///	"isb	0xF	\n" // refetch instruction pipeline
//	"cpsie	i	\n" // enable ISRs
	::: "memory");
}

/*  Written in 2019 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted.

This is xoshiro128++ 1.0, one of our 32-bit all-purpose, rock-solid
generators. It has excellent speed, a state size (128 bits) that is
large enough for mild parallelism, and it passes all tests we are aware of.

THE STATE MUST BE SEEDED SO THAT IT IS NOT EVERYWHERE ZERO */

uint32_t xoshiro[4]; // state entropy pool

inline uint32_t xoshiro_rotl (uint32_t x, int k) {
	return (x << k) | (x >> (32 - k));
}

inline void xoshiro_mix (void)
{
	uint32_t t  = xoshiro[1] << 9;
	xoshiro[2] ^= xoshiro[0];
	xoshiro[3] ^= xoshiro[1];
	xoshiro[1] ^= xoshiro[2];
	xoshiro[0] ^= xoshiro[3];
	xoshiro[2] ^= t;
	xoshiro[3] = xoshiro_rotl(xoshiro[3], 11);
}

void entropy_ensure_seed (void)
{
	if (xoshiro[0]==0 && xoshiro[1]==0 && xoshiro[2]==0 && xoshiro[3]==0) {
		// load serial number xor current ticks
		unsigned cycles = REG_TC4_COUNT32_COUNT;
		xoshiro[0] = *(unsigned*)0x0080A00Cu ^ cycles;
		xoshiro[1] = *(unsigned*)0x0080A040u ^ cycles;
		xoshiro[2] = *(unsigned*)0x0080A044u;
		xoshiro[3] = *(unsigned*)0x0080A048u;
	}
}

uint32_t xoshiro128plusplus (void)
{
	xoshiro_mix();
	entropy_ensure_seed();
	return xoshiro_rotl(xoshiro[0]+xoshiro[3], 7) + xoshiro[0];
}

uint32_t xoshiro128starstar (void)
{
	xoshiro_mix();
	entropy_ensure_seed();
	return xoshiro_rotl(xoshiro[1] * 5, 7) * 9;
}

// implementation for c library definition
int rand () { return xoshiro128plusplus(); }
clock_t clock (void) { return REG_TC4_COUNT32_COUNT; }

inline void system_timer_request ()
{
	while (REG_TC4_STATUS); // & TC_STATUS_SYNCBUSY); // write-sync
	*(RwReg8*)0x42003003u = (uint8_t) ((TC_READREQ_RCONT | TC_READREQ_RREQ)>>8);
}

void entropy_feed ()
{
	if (!(REG_TC4_INTFLAG & TC_INTFLAG_MC0)) return;
	unsigned read = REG_TC4_COUNT32_CC0;
	REG_TC4_INTFLAG = TC_INTFLAG_MC0;
	unsigned byte = read & 0xFF;
	unsigned mark = (read >> 8) & 0xF;
	((uint8_t*)xoshiro)[mark] ^= byte;
	system_timer_request();
}

void entropy_test (void)
{
	for (unsigned n = 0; n < 128000; n++) {
		printf("0x%08"PRIX32"," , xoshiro128plusplus());
	}	fflush(0);
}
CONSOLE_RUN(entropy, entropy_test)

/*
void fiber_test_sleep ()
{
	NVIC_EnableIRQ(TC4_IRQn);
	printf("0x%04X\r\n" , REG_TC4_READREQ);
	REG_TC4_INTENSET = TC_INTFLAG_MC1;
	REG_TC4_COUNT32_CC1 = REG_TC4_COUNT32_COUNT + 48000;
	system_timer_request();
}
CONSOLE_RUN(snafu, fiber_test_sleep)

void handler_TC4 ()
{
	REG_TC4_INTFLAG = TC_INTFLAG_MC1;
	REG_TC4_INTENCLR = TC_INTFLAG_MC1;
	system_timer_request();
	printf("TC4");
}
*/
