#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "samd21g18a/sam.h"
#include "samd21g18a/fiber.h"
#include "samd21g18a/console.h"
#include "tinyusb/tusb.h"

#include "network.h"
#include "circuit.h"
#include "rfm12bp.h"
#include "conduit.h"
#include "channel.h"
#include "packets.h"
#include "storage.h"

int main(void)
{
	storage_init();
	tusb_rhport_init(TUD_OPT_RHPORT,
		& (tusb_rhport_init_t const) {
		.role = TUSB_ROLE_DEVICE,
		.speed = TUSB_SPEED_FULL
	});
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
		entropy_feed();
		REG_PORT_OUTTGL0 = PORT_PA17;
	//	fiber_idle_wfi();
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
	tusb_int_handler(0, true);
	REG_PORT_OUTSET0 = PORT_PA18;
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
	(void) rts;
	if (dtr) fiber_create(&console_heading,0);
}
CONSOLE_DAT(header, acm_line_state_cb, true)

//__attribute__((noinline,long_call,section(".ramfunc")))
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
CONSOLE_RUN(state, coroutine)
