#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "samd21g18a/sam.h"
#include "samd21g18a/fiber.h"
#include "samd21g18a/version.h"
#include "tinyusb/tusb.h"

#include "network.h"
#include "circuit.h"
#include "rfm12bp.h"
#include "conduit.h"
#include "do3rb.h"
#include "channel.h"
#include "packets.h"
#include "storage.h"

#include <malloc.h>

// forward declarations
static void console_loop(void);

int main(void)
{
	storage_init();
	tusb_init();
	circuit_init();
	conduit_init();
	packets_init();
	fiber_create(&console_loop,1);
	fiber_create(&rfm12bp_init,0);
	while (1)
	{
		tud_task();
		fiber_yield();
		fiber_watch();
		REG_PORT_OUTTGL0 = PORT_PA17;
	}
	return 0;
}

void tud_network_init_cb(void)
{
	static uint8_t done;
	if (done) return; else done = true;
	fiber_create(&network_loop,1);
	fiber_create(&channel_init,0);
}

//__attribute__((flatten))
void handler_USB(void)
{
	REG_PORT_OUTCLR0 = PORT_PA18;
	tud_int_handler(0);
	REG_PORT_OUTSET0 = PORT_PA18;
}

//__attribute__((noinline,section(".ramfunc")))
void coroutine(void)
{
	REG_PORT_OUTCLR0 = PORT_PA19;

	printf("eic 0x%04X\r\n", (uint16_t) REG_EIC_INTFLAG);
	REG_EIC_INTFLAG = 0xFFFF;

	printf("spi 0x%04X\r\n", REG_SERCOM2_SPI_INTFLAG);
	printf("pin 0x%04X\r\n", (uint16_t) REG_PORT_IN0);
	printf("rfm 0x%04X\r\n", circuit_asic_word(0));
	printf("pin 0x%04X\r\n", (uint16_t) REG_PORT_IN0);

	printf("eic 0x%04X\r\n", (uint16_t) REG_EIC_INTFLAG);
	REG_EIC_INTFLAG = 0xFFFF;

	REG_PORT_OUTSET0 = PORT_PA19;
}

void console_heading(void)
{
	const char header[] =
	"Wireless Network Transceiver RFM12BP\r\n"
	"created per Aspera ad Astra by DO3RB\r\n";
	usleep(10000);
	write(STDOUT_FILENO, header, sizeof(header)-1);
}

void acm_line_state_cb(bool dtr, bool rts)
{
	(void) dtr;
	(void) rts;
	if (dtr) fiber_create(&console_heading,0);
}

static void console_loop(void)
{
	static uint8_t pos;
	static char buf[80];
	uint8_t inc, n, enter = false;

	inc = read(STDIN_FILENO, &buf[pos], sizeof(buf)-pos);

	if (pos+inc >= sizeof(buf)-1) {
		enter = true;
		n = sizeof(buf);
	}
	else for (n = pos; n < pos+inc; n++) {
	  if (buf[n] == '\r') {enter = true; break;}
	}
	write(STDOUT_FILENO, &buf[pos], n-pos);
	pos = n; if (!enter) return;
	write(STDOUT_FILENO, "\r\n", 2);

	if (strncmp(buf, "load", 4) == 0) exit( 1);
	if (strncmp(buf, "halt", 4) == 0) exit( 0);
	if (strncmp(buf, "boot", 4) == 0) exit(-1);

	if (strncmp(buf, "fiber", 5) == 0) fiber_print();
	if (strncmp(buf, "version", 7) == 0) { write(STDOUT_FILENO, VERSION, strlen(VERSION)); }

	if (strncmp(buf, "hang", 4) == 0) { while (1) {} }
	if (strncmp(buf, "fault", 5) == 0) { asm volatile ("udf #0"); }

	if (strncmp(buf, "id", 2) == 0) {
		printf("0x%X Revision %c\r\n", (unsigned) DSU->DID.reg, 'A'+DSU->DID.bit.REVISION);
	}

	if (strncmp(buf, "reset", 5) == 0) {
		circuit_asic_word(0xFE00);
	}
	if (strncmp(buf, "asic", 4) == 0) {
		printf("rfm 0x%02X\r\n", circuit_asic_byte(0));
	}
	if (strncmp(buf, "fifo", 4) == 0) {
		circuit_fifo_byte_yield(0xAA);
	}
	if (strncmp(buf, "mem", 3) == 0) {
		uint8_t* blocks[8];
		memset(blocks, 0, sizeof(blocks));
		for (unsigned m=0; m<8; m++) {
			blocks[m] = (uint8_t*) malloc(4096);
			printf("%p\r\n", blocks[m]);
			if (blocks[m] == NULL) break;
		}
		for (unsigned m=0; m<8; m++) if (blocks[m]) free(blocks[m]);
	}
	if (strncmp(buf, "state", 5) == 0) {
		coroutine();
	//	fiber_create(&coroutine,0);
	}
	if (strncmp(buf, "start", 5) == 0) {
		fiber_create(&rfm12bp_init,0);
	}
	if (strncmp(buf, "transmit", 8) == 0) {
		fiber_create(&rfm12bp_transmit,0);
	}
	if (strncmp(buf, "conduit", 7) == 0) {
		conduit_test();
	}
	if (strncmp(buf, "do3rb", 5) == 0) {
		uint32_t time = REG_RTC_MODE0_COUNT;
		do3rb_test_exhaustive();
		time = REG_RTC_MODE0_COUNT - time;
		int s = time % 60; time /= 60;
		int m = time % 60; time /= 60;
		int h = time % 24; time /= 24;
		printf("%02d:%02d:%02d\r\n", h, m, s);
	}
	if (strncmp(buf, "packets", 7) == 0) {
		packets_print();
	}
	if (strncmp(buf, "xlc ", 4) == 0) {
		unsigned int xlc = 0;
		circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX);
		sscanf(&buf[4], "%d", &xlc);
		circuit_asic_word(RFM_CFG_CMD | RFM_CFG_EL | RFM_CFG_EF | RFM_CFG_433 | RFM_CFG_X(xlc));
		printf("%d\r\n", xlc);
		for (uint8_t m=0; m<FIBER; m++) {if (fiber.funcs[m] == (int(*)(void)) &channel_task) fiber.funcs[m] = 0;}
		circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX);
	}
	if (strncmp(buf, "cycles", 6) == 0) {
		volatile uint32_t code = 0;
		unsigned int time = SysTick->VAL;
		if (do3rb_synchron_detect(code)) return;	// 41
	//	code = do3rb_synchron_detect(code);			// 46
		time = (time - SysTick->VAL) & SysTick_VAL_CURRENT_Msk;
		printf("%d\r\n", time);
	}
	if (strncmp(buf, "popcount", 8) == 0) {
		unsigned int errors = 0;
		#include "popcount.h"
		for (uint32_t m = 0; m < 1<<24; m++) {
			if (popcount24(m) != popcount(m)) errors += 1;
			if (m % 1024) fiber_yield();
		}
		printf("%d errors\r\n", errors);
	}
	if (strncmp(buf, "readi", 5) == 0) {
		int test;
		scanf("%d", &test);
		printf("%d\r\n", test);
	}
	if (strncmp(buf, "reads", 5) == 0) {
		scanf("%s", buf);
		printf("%s\r\n", buf);
	}
	if (strncmp(buf, "writes", 6) == 0) {
		for (uint_fast8_t m = 0; m < sizeof(buf); m++) {buf[m] = '0'+m;}
		for (uint_fast8_t m = 0; m < sizeof(buf); m++) {
			write(STDOUT_FILENO, buf, m);
			write(STDOUT_FILENO, "\r\n", 2);
		}
	}
	if (strncmp(buf, "clock", 5) == 0) {
		printf (
			"%02d:%02d:%02d\r\n",
			RTC->MODE2.CLOCK.bit.HOUR,
			RTC->MODE2.CLOCK.bit.MINUTE,
			RTC->MODE2.CLOCK.bit.SECOND
		);
	}
	if (strncmp(buf, "header", 6) == 0) {
		fiber_create(&console_heading,0);
	}
	if (strncmp(buf, "tick", 4) == 0) {
		usleep(1000000);
		for (uint_fast8_t m = 0; m < 16; m++) {
			printf("tock\r\n");
			usleep(1000000);
		}
	}
	if (strncmp(buf, "store", 5) == 0) {
		uint8_t* mem = malloc(256);
		if (mem) {
		memset(mem, 0x00, 256);
		scanf("%s", mem);
		storage_flash(storage_array, mem, sizeof(storage_array));
		free(mem);
	}}
	if (strncmp(buf, "recall", 6) == 0) {
	//	recall of unwritten store hardfaults
	//	flash probably default 0xFF no string null found
		printf("%s\r\n", storage_array);
	}

	pos = 0;
}
