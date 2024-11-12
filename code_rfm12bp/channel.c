#include "channel.h"

#include "rfm12bp.h"
#include "circuit.h"
#include "conduit.h"
#include "do3rb.h"
#include "network.h"
#include "storage.h"

#include "samd21g18a/fiber.h"

#include <stdio.h>

static uint32_t Window;
static   int8_t Offset;
static void channel_receive_clear(void) { Window = 0; Offset = 7; }
static packet_t* Transmit;

void channel_init(void)
{
	rfm12bp_init();
	fiber_create(&channel_task,0);
}

void channel_task(void)
{
	int_fast32_t backoff = 0;
	packet_t* packet = (packet_t*) packets_get();
	// receiver start
	channel_receive_clear();
	rfm12bp_receive_on();
	circuit_pmux_fifo();
	conduit_open();
	while (1) {
		while (!conduit_empty()) {
			if (channel_receive_yield(packet)) {
				network_transmit_yield(packet->octet, network_translate_ether(packet));
				packet = (packet_t*) packets_get();
			}
			if (backoff > 0) backoff -= 1;
		}
		if (Transmit && !(backoff>0)) {
			// receiver stop
			conduit_close();
			rfm12bp_receive_off();
			// transmission
			channel_transmit(Transmit);
			circuit_asic_byte(RFM_SRD_CMD);
			packets_put(Transmit->octet);
			Transmit = NULL;
			// receiver restart
			channel_receive_clear();
			rfm12bp_receive_on();
			circuit_pmux_fifo();
			conduit_open();
			backoff = 128;
		}
		fiber_yield();
	}
	packets_put(packet->octet);
	conduit_close();
	rfm12bp_receive_off();
}

void channel_transmit(packet_t* packet)
{
	circuit_fifo_byte_yield(DO3RB_PREAMBLE_1ST);
	circuit_fifo_byte_yield(DO3RB_PREAMBLE_2ND);
	rfm12bp_transmit_on();
	circuit_pmux_fifo();
	conduit_open();
	conduit_write_yield(DO3RB_PREAMBLE_3RD);
	conduit_write_yield(DO3RB_PREAMBLE_3RD);
	conduit_write_yield(DO3RB_PREAMBLE_3RD);
	conduit_write_yield(DO3RB_SYNCHRON_1ST);
	conduit_write_yield(DO3RB_SYNCHRON_2ND);
	conduit_write_yield(DO3RB_SYNCHRON_3RD);
	if (packet->ip4.typ == IP_PN_INT) packet = (packet_t*) (((uint32_t)packet)+8);
	uint_fast16_t one = packet->radio.block[0].one;
	uint_fast16_t two = packet->radio.block[0].two;
	uint_fast32_t chk = do3rb_golay_encode(one ^ two);
	conduit_write_yield(chk>>16);
	conduit_write_yield(chk>>8);
	conduit_write_yield(chk);
//	packet->octet[packet->radio.frame.len] = 0x00;
	int_fast32_t code, m=0, n=packet->radio.frame.len+sizeof(packet->radio.frame);
	while (1) {
		code = do3rb_golay_encode(packet->radio.block[m].one);
		conduit_write_yield(code>>16);
		conduit_write_yield(code>>8);
		conduit_write_yield(code);
		if ((n -= 1) <= 0) break;
		code = do3rb_golay_encode(packet->radio.block[m].two);
		conduit_write_yield(code>>16);
		conduit_write_yield(code>>8);
		conduit_write_yield(code);
		if ((n -= 2) <= 0) break;
		m += 1;
	}
	conduit_write_yield(0x00); // flush radio's
	conduit_write_yield(0x00); // double buffer
	while (!conduit_empty()) fiber_yield(); // push one more, wait almost empty
	conduit_close();
	rfm12bp_transmit_off();
}

void channel_transmit_yield(packet_t* packet)
{
	while (Transmit != NULL) fiber_yield();
	Transmit = packet;
}

static int channel_receive_code_yield(void)
{
	#pragma GCC unroll 3
	for (uint8_t n = 0; n < 3; n++) {
		Window = (Window << 8) | conduit_read_yield();
	}
	return conduit_vdi() < 128+32;
}

int channel_receive_yield(packet_t* packet) // stateful
{
	Window = (Window << 8) | conduit_read();

	do if (__builtin_expect(do3rb_synchron_detect(Window >> Offset), false))
	{ goto header; } while (Offset --> 0); Offset = 7; return 0;

	uint_fast16_t mark_cpu;
	packet_radio_t header;
	header:
	mark_cpu = conduit_mark_cpu;
	channel_receive_code_yield();
	uint32_t header0 = do3rb_golay_decode(Window >> Offset);
	channel_receive_code_yield();
	uint32_t header1 = do3rb_golay_decode(Window >> Offset);
	channel_receive_code_yield();
	uint32_t header2 = do3rb_golay_decode(Window >> Offset);

	header.block.one = header1;
	header.block.two = header2;

	// check header
	uint_fast8_t src = header.frame.src;
	uint_fast8_t dst = header.frame.dst;
	uint_fast8_t ip4 = storage_client_ip4()[3];
	if (
		header0 != (header1 ^ header2) ||
		packets_type_ether(header.frame.typ) == 0xFF ||
		PACKET_RADIO_LEN < header.frame.len ||
		src == dst || src == ip4
	) {
		if (Offset == 0) {
			conduit_mark_cpu = (mark_cpu - 2) % CONDUIT_BYTE;
			Offset = 7;
		} else {
			conduit_mark_cpu = (mark_cpu - 3) % CONDUIT_BYTE;
			Offset -= 1;
		}
		Window = (Window << 8) | conduit_read();
		Window = (Window << 8) | conduit_read();
		return 0;
	}

	if (packets_type_ether(header.frame.typ) == IP_PN_INT) {
		packet->ip4.typ = IP_PN_INT;
		packet = (packet_t*) (((uint32_t)packet)+8);
	}

	packet->radio.block[0] = header.block;

	// payload
	uint_fast16_t m=1, n=0;
	while (1) {
		channel_receive_code_yield();
		packet->radio.block[m].one = do3rb_golay_decode(Window >> Offset);
		if ((n += 1) >= packet->radio.frame.len) break;
		channel_receive_code_yield();
		packet->radio.block[m].two = do3rb_golay_decode(Window >> Offset);
		if ((n += 2) >= packet->radio.frame.len) break;
		m += 1;
	}
	return 1;
}
