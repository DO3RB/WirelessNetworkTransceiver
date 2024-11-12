#include "conduit.h"

#include <string.h>

#include "samd21g18a/sam.h"
#include "samd21g18a/fiber.h"
#include "circuit.h"

volatile DmacDescriptor conduit_base[2] __attribute__((aligned(16)));
volatile DmacDescriptor conduit_back[2] __attribute__((aligned(16)));

static volatile uint8_t conduit_data    [CONDUIT_BYTE];
static volatile uint8_t conduit_data_vdi[CONDUIT_FLAG];
inline uint_fast16_t conduit_mark_dma(void) { return CONDUIT_BYTE - conduit_back[1].BTCNT.reg; }
uint_fast16_t conduit_mark_cpu = 0;

//volatile uint32_t counter;

// Radio nIRQ on PIN_PA04 triggers the chain
void conduit_open (void) {
	REG_EIC_INTENSET = CIRCUIT_nIRQ;
	REG_EIC_EVCTRL |=  EIC_EVCTRL_EXTINTEO4;
	circuit_vdi_clear();
}
void conduit_close(void) {
	REG_EIC_INTENCLR = CIRCUIT_nIRQ;
	REG_EIC_EVCTRL &= ~EIC_EVCTRL_EXTINTEO4;
	conduit_vdi_clear();
	conduit_mark_cpu = conduit_mark_dma();
}

void conduit_init(void)
{
	// EventSystem enable clock
	REG_PM_APBCMASK |= PM_APBCMASK_EVSYS;
	REG_EVSYS_CTRL = EVSYS_CTRL_GCLKREQ; // needed for software trigger
	// EventSystem Channel0 Transmit
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_EVSYS_0 | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
	REG_EVSYS_USER = EVSYS_USER_CHANNEL(0+1) | EVSYS_USER_USER(EVSYS_ID_USER_DMAC_CH_0);
	REG_EVSYS_CHANNEL = EVSYS_CHANNEL_CHANNEL(0) | EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_4) |
		EVSYS_CHANNEL_PATH_RESYNCHRONIZED | EVSYS_CHANNEL_EDGSEL_RISING_EDGE;
	// EventSystem Channel1 Receive
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_EVSYS_1 | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
	REG_EVSYS_USER = EVSYS_USER_CHANNEL(1+1) | EVSYS_USER_USER(EVSYS_ID_USER_DMAC_CH_1);
	REG_EVSYS_CHANNEL = EVSYS_CHANNEL_CHANNEL(1) | EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_DMAC_CH_0) |
		EVSYS_CHANNEL_PATH_RESYNCHRONIZED | EVSYS_CHANNEL_EDGSEL_RISING_EDGE;

	// DMAC enable clocks
	REG_PM_AHBMASK  |= PM_AHBMASK_DMAC;
	REG_PM_APBBMASK |= PM_APBBMASK_DMAC;
	// set addresses of descriptor arrays
	REG_DMAC_BASEADDR = (uint32_t) conduit_base;
	REG_DMAC_WRBADDR  = (uint32_t) conduit_back;
	// enable arbiter and controller
	REG_DMAC_PRICTRL0 = DMAC_PRICTRL0_RRLVLEN0;
	REG_DMAC_CTRL = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xF);

	// Descriptor0 Transmit
	conduit_base[0].BTCTRL.reg = DMAC_BTCTRL_VALID | DMAC_BTCTRL_EVOSEL_BEAT | DMAC_BTCTRL_BLOCKACT_NOACT |
		DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_STEPSEL_SRC | DMAC_BTCTRL_STEPSIZE_X1;
	conduit_base[0].BTCNT.reg    = CONDUIT_BYTE;
	conduit_base[0].SRCADDR.reg  = (uint32_t) & conduit_data[CONDUIT_BYTE];
	conduit_base[0].DSTADDR.reg  = (uint32_t) & REG_SERCOM0_SPI_DATA;
	conduit_base[0].DESCADDR.reg = (uint32_t) & conduit_base[0];

	// Descriptor1 Receive
	conduit_base[1].BTCTRL.reg = DMAC_BTCTRL_VALID | DMAC_BTCTRL_EVOSEL_BEAT | DMAC_BTCTRL_BLOCKACT_NOACT |
		DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_STEPSEL_DST | DMAC_BTCTRL_STEPSIZE_X1;
	conduit_base[1].BTCNT.reg    = CONDUIT_BYTE;
	conduit_base[1].SRCADDR.reg  = (uint32_t) & REG_SERCOM0_SPI_DATA;
	conduit_base[1].DSTADDR.reg  = (uint32_t) & conduit_data[CONDUIT_BYTE];
	conduit_base[1].DESCADDR.reg = (uint32_t) & conduit_base[1];

	// Channel0 Transmit
	REG_DMAC_CHID = DMAC_CHID_ID(0);
	REG_DMAC_CHCTRLB = DMAC_CHCTRLB_EVACT_CTRIG | DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_EVIE | DMAC_CHCTRLB_EVOE |
		DMAC_CHCTRLB_LVL_LVL0 |	DMAC_CHCTRLB_TRIGSRC(SERCOM0_DMAC_ID_TX) | DMAC_CHCTRLB_CMD_NOACT;
	REG_DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE;
	// Channel1 Receive
	REG_DMAC_CHID = DMAC_CHID_ID(1);
	REG_DMAC_CHCTRLB = DMAC_CHCTRLB_EVACT_CTRIG | DMAC_CHCTRLB_TRIGACT_BEAT | DMAC_CHCTRLB_EVIE |
		DMAC_CHCTRLB_LVL_LVL0 |	DMAC_CHCTRLB_TRIGSRC(SERCOM0_DMAC_ID_RX) | DMAC_CHCTRLB_CMD_NOACT;
	REG_DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE;
}

void conduit_trigger_evsys(void)
{
	*(uint16_t*)&REG_EVSYS_CHANNEL = EVSYS_CHANNEL_CHANNEL(0) | EVSYS_CHANNEL_SWEVT;
}
void conduit_trigger_dmach(void)
{
	REG_DMAC_SWTRIGCTRL = DMAC_SWTRIGCTRL_SWTRIG0;
}

void handler_EIC(void)
{
	uint_fast16_t mark = conduit_mark_dma();
	uint_fast16_t word = mark / 8;
	uint32_t mask = 1 << (mark % 8);
	uint32_t flag = circuit_vdi() << (mark % 8);
	conduit_data_vdi[word] = (conduit_data_vdi[word] & ~mask) | flag;
	// clear interrupt origin
	REG_EIC_INTFLAG = CIRCUIT_nIRQ;
//	counter++;
}

void conduit_vdi_clear(void) {
	memset((uint8_t*)conduit_data_vdi, 0, CONDUIT_FLAG);
}

uint_fast16_t conduit_vdi(void) {
	uint_fast16_t word = conduit_mark_cpu / 8;
	uint_fast16_t flag = conduit_mark_cpu % 8;
	uint32_t data = conduit_data_vdi[ word  ];
	uint32_t last = conduit_data_vdi[(word-1)%CONDUIT_FLAG];
	return (((data << 8) | last) >> flag) & 0x01FF;
}

void conduit_wait(void)
{
	while (conduit_mark_dma() == conduit_mark_cpu) fiber_yield();
}
uint8_t conduit_read(void)
{
	uint8_t byte = conduit_data[conduit_mark_cpu];
	conduit_mark_cpu = (conduit_mark_cpu+1) % CONDUIT_BYTE;
	return byte;
}
uint8_t conduit_read_yield(void)
{
	while (conduit_mark_dma() == conduit_mark_cpu) fiber_yield();
	uint8_t byte = conduit_data[conduit_mark_cpu];
	conduit_mark_cpu = (conduit_mark_cpu+1) % CONDUIT_BYTE;
	return byte;
}
void conduit_write_yield(uint8_t byte)
{
	unsigned next_mark_cpu = (conduit_mark_cpu+1) % CONDUIT_BYTE;
	while (conduit_mark_dma() == next_mark_cpu) fiber_yield();
	conduit_data[conduit_mark_cpu] = byte;
	conduit_mark_cpu = next_mark_cpu;
}
int conduit_empty(void) { return conduit_mark_dma() == conduit_mark_cpu; }
int conduit_count(void) { return (conduit_mark_dma() - conduit_mark_cpu) % CONDUIT_BYTE; }

#include "circuit.h"
#include "rfm12bp.h"
#include <stdio.h>

void conduit_test(void)
{
	conduit_open();
	rfm12bp_receive_on();
	circuit_pmux_fifo();
	for (uint32_t m=0; m<256; m++) {
		printf("%02X ", conduit_read_yield());
	/*
		unsigned int A = REG_DMAC_ACTIVE;
		unsigned int B = conduit_back[0].BTCNT.reg;
		unsigned int C = conduit_back[1].BTCNT.reg;
		printf("ACTIVE 0x%08X CNT0 0x%02X CNT1 0x%02X\r\n", A,B,C);
	*/
	}
	printf("\r\n");
	rfm12bp_receive_off();
	conduit_close();
}

/*
void conduit_test(void)
{
	counter = 0;
	uint32_t done = 0;
	circuit_asic_byte(0);
	conduit_open();
	rfm12bp_receive_on();
	circuit_pmux_fifo();
	for (uint32_t m=0; m<CONDUIT_BYTE; m++) {
		if (done == counter) fiber_yield();
		done = counter;
		printf("DMA %d %d CNT %d\r\n",
			CONDUIT_BYTE - conduit_back[0].BTCNT.reg,
			CONDUIT_BYTE - conduit_back[1].BTCNT.reg, // conduit_mark_dma()
			(unsigned int) counter
		);
	}
	printf("\r\n");
	rfm12bp_receive_off();
	conduit_close();
}
*/
