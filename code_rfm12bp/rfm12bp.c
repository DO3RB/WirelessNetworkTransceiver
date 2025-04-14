#include "rfm12bp.h"

#include <stdio.h>

#include "samd21g18a/sam.h"
#include "samd21g18a/console.h"

#include "circuit.h"
#include "storage.h"

void rfm12bp_init(void)
{
	// ensure power-on reset
	while ((circuit_asic_byte(RFM_SRD_CMD) & (RFM_SRD_POR>>8)) == 0) {
		REG_EIC_INTFLAG = CIRCUIT_nIRQ; // circuit_eint_clear
		circuit_asic_word(RFM_RST_CMD);
		circuit_eint_yield(CIRCUIT_nIRQ);
	}
	REG_EIC_INTFLAG = 0xFFFF;
	// configure
	circuit_asic_word(RFM_CFG_CMD | RFM_CFG_EL | RFM_CFG_EF | RFM_CFG_433 | RFM_CFG_X(storage_xlc()));
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC); // RFM_PWR_EB
	circuit_asic_word(RFM_FRQ_CMD | RFM_FRQ_F(434000000));
	circuit_asic_word(RFM_DRT_CMD | RFM_DRT_R(115200));
	circuit_asic_word(RFM_RCV_CMD | RFM_RCV_CRL | RFM_RCV_VDI | RFM_RCV_BW270 | RFM_RCV_SQL(2));
	circuit_asic_word(RFM_DFI_CMD | RFM_DFI_AL | RFM_DFI_F(5));
	circuit_asic_word(RFM_FRM_CMD | RFM_FRM_F(8) | RFM_FRM_AL | RFM_FRM_FF);
	circuit_asic_word(RFM_AFC_CMD | RFM_AFC_EN | RFM_AFC_OE | RFM_AFC_RL0 | RFM_AFC_A1);
	circuit_asic_word(RFM_TXC_CMD | RFM_TXC_M(150));
	circuit_asic_word(RFM_DIV_CMD | RFM_DIV_ONE);
}
CONSOLE_RUN(start, rfm12bp_init)

void rfm12bp_reset(void)
{
	circuit_asic_word(0xFE00);
}
CONSOLE_RUN(reset, rfm12bp_reset)

void rfm12bp_transmit(void)
{
	REG_PORT_OUTSET1 = CIRCUIT_TXEN;
	circuit_fifo_byte_yield(0x33); // 00110011
	circuit_fifo_byte_yield(0x35); // 00110101
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC | RFM_PWR_ET);
	circuit_eint_yield(CIRCUIT_nIRQ); // CIRCUIT_PIN_FFIT only active in receive
	circuit_fifo_byte_yield(0x55); // 01010101
	for (uint8_t n = 0; n < 10; n++) {
		circuit_eint_yield(CIRCUIT_nIRQ);
//		printf("pin %04X ", (uint16_t) REG_PORT_IN0);
		circuit_fifo_byte_yield(0xAA);
//		printf("rfm %02X\r\n", circuit_asic_byte(0)); 
	}
	circuit_eint_yield(CIRCUIT_nIRQ);
	circuit_fifo_byte_yield(0x00);
	circuit_eint_yield(CIRCUIT_nIRQ);
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC);
	REG_PORT_OUTCLR1 = CIRCUIT_TXEN;
	circuit_fifo_byte_yield(0x00);
}
CONSOLE_RUN(transmit, rfm12bp_transmit)

void rfm12bp_transmit_on(void)
{
	REG_PORT_OUTSET1 = CIRCUIT_TXEN;
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC | RFM_PWR_ET);
}
void rfm12bp_transmit_off(void)
{
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC);
	REG_PORT_OUTCLR1 = CIRCUIT_TXEN;
}

#include "do3rb.h"

void rfm12bp_receive(void)
{
	REG_PORT_OUTSET1 = CIRCUIT_RXEN;
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC | RFM_PWR_ER);
	uint32_t word = 0;
	for (uint8_t n = 0; n < 10; n++) {
		circuit_eint_yield(CIRCUIT_nIRQ);
		word = (word << 8) | circuit_fifo_byte_yield(0x00);
		if (do3rb_synchron_detect(word)) printf("s"); else printf(".");
	}
	printf("\r\n");
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC);
	REG_PORT_OUTCLR1 = CIRCUIT_RXEN;
}

void rfm12bp_receive_on(void)
{
	REG_PORT_OUTSET1 = CIRCUIT_RXEN;
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC | RFM_PWR_ER);
}
void rfm12bp_receive_off(void)
{
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX | RFM_PWR_DC);
	REG_PORT_OUTCLR1 = CIRCUIT_RXEN;
}

#include "samd21g18a/fiber.h"
#include "channel.h"

void rfm12bp_calibrate(char *value)
{
	unsigned int xlc = 0;
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX);
	sscanf(value, "%d", &xlc);
	circuit_asic_word(RFM_CFG_CMD | RFM_CFG_EL | RFM_CFG_EF | RFM_CFG_433 | RFM_CFG_X(xlc));
	printf("%d\r\n", xlc);
	for (uint8_t m=0; m<FIBER; m++) {if (fiber.funcs[m] == (int(*)(void)) &channel_task) fiber.funcs[m] = 0;}
	circuit_asic_word(RFM_PWR_CMD | RFM_PWR_EBB | RFM_PWR_ES | RFM_PWR_EX);
}
CONSOLE_ARG(xlc, rfm12bp_calibrate)
