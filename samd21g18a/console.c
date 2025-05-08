#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include "console.h"

#include "sam.h"
#include "version.h"

#include <malloc.h>

CONSOLE_DAT(load, exit,  1)
CONSOLE_DAT(halt, exit,  0)
CONSOLE_DAT(boot, exit, -1)

void console_version(void) { write(STDOUT_FILENO, VERSION, strlen(VERSION)); }
CONSOLE_RUN(version, console_version)

void console_hang(void) { while (1) asm volatile ("nop"); }
CONSOLE_RUN(hang, console_hang)

void console_fault(void) { asm volatile ("udf #0"); }
CONSOLE_RUN(fault, console_fault)

void console_id(void) { printf("0x%"PRIX32" Revision %c\r\n", DSU->DID.reg, 'A'+DSU->DID.bit.REVISION); }
CONSOLE_RUN(id, console_id)

void console_mem(void) {
	uint8_t* blocks[8] = {0};
//	memset(blocks, 0, sizeof(blocks));
//	asm ("":::"memory");
	for (unsigned m=0; m<8; m++) {
		blocks[m] = malloc(4096);
		printf("%p\r\n", blocks[m]);
		if (blocks[m] == NULL) break;
	}
	for (unsigned m=0; m<8; m++) if (blocks[m]) free(blocks[m]);
}
CONSOLE_RUN(mem, console_mem)

void console_clock(void)
{
	unsigned time = REG_RTC_MODE0_COUNT;
	printf (
		"%02d:%02d:%02d\r\n",
		time / 3600 % 24,
		time / 60   % 60,
		time % 60
	);
}
CONSOLE_RUN(clock, console_clock)

void console_tick(void)
{
	usleep(1000000);
	for (uint_fast8_t m = 0; m < 16; m++) {
		printf("tock\r\n");
		usleep(1000000);
	}
}
CONSOLE_RUN(tick, console_tick)

void console_writes(char* buf)
{
	for (uint_fast8_t m = 0; m < 64; m++) {buf[m] = '0'+m;}
	for (uint_fast8_t m = 0; m < 64; m++) {
		write(STDOUT_FILENO, buf, m);
		write(STDOUT_FILENO, "\r\n", 2);
	}
}
CONSOLE_ARG(writes, console_writes)

void console_readi(void)
{
	int test;
	if (scanf("%d", &test)) printf("%d\r\n", test);
	getchar(); // unmatched multicharacter input remains in stdin
}
CONSOLE_RUN(readi, console_readi)

void console_reads(char* buf)
{
//	scanf("%[^\r]79s\r", buf);
//	scanf(" %[^\r]79s", buf);
	scanf("%[^\r]79s", buf); getchar();
	printf("%s\r\n", buf);
}
CONSOLE_ARG(reads, console_reads)

void console_gets(char* buf)
{
	fgets(buf, 80, stdin);
	printf("%s\r\n", buf);
}
CONSOLE_ARG(gets, console_gets)

void console_loop(void)
{
	static uint8_t pos;
	static char buf[80];
	unsigned inc, n, enter = false;

	inc = read(STDIN_FILENO, &buf[pos], sizeof(buf)-pos);

	if (pos+inc >= sizeof(buf)-1) {
		enter = true;
		n = sizeof(buf);
	}
	else for (n = pos; n < pos+inc; n++) {
	  if (buf[n] == '\r') {enter = true; break;}
	}
	write(STDOUT_FILENO, &buf[pos], n-pos);
	pos = n; if (!enter) return; // buf[n] = '\0';
	write(STDOUT_FILENO, "\r\n", 2);

	extern console_t __console_start;
	extern console_t __console_end;

	for (console_t* command = &__console_start; command < &__console_end; command++) {
		// scanning along constant string
		char *store = command->str, *input = buf;
		while (*store != '\0' && *store++ == *input++);
		if (*store != '\0') continue;
		if (*input != '\r' && *input != ' ') continue;
		// behaviour defined for aapcs R0-R3
		switch (command->typ) {
			case RUN: ((void(*)(void))command->ptr)(); break;
			case ARG: ((void(*)(char*))command->ptr)(++input); break;
			case DAT: ((void(*)(unsigned))command->ptr)(command->dat); break;
			default: break;
		}
	}

	pos = 0;
}
