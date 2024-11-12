#include "fiber.h"

#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>

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

#include "sam.h"
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
