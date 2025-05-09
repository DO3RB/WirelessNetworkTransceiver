#include "circuit.h"

#include "samd21g18a/sam.h"
#include "samd21g18a/fiber.h"

void circuit_init(void)
{
	// configure pins
	REG_PORT_OUTCLR1 = CIRCUIT_TXEN | CIRCUIT_RXEN;
	REG_PORT_DIRSET1 = CIRCUIT_TXEN | CIRCUIT_RXEN;
	REG_PORT_WRCONFIG1 = CIRCUIT_TXEN | CIRCUIT_RXEN |
		PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_DRVSTR;

	REG_PORT_WRCONFIG0 = CIRCUIT_nIRQ | CIRCUIT_nRES | CIRCUIT_FFIT | CIRCUIT_VDI |
		PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_INEN | PORT_WRCONFIG_PULLEN |
		PORT_WRCONFIG_WRPMUX | CIRCUIT_EINT;
	REG_PORT_OUTSET0 = CIRCUIT_nIRQ | CIRCUIT_nRES; // pull high
	REG_PORT_CTRL0 = CIRCUIT_VDI; // always sample input

	REG_PORT_OUTSET0 = CIRCUIT_nSEL;
	REG_PORT_DIRSET0 = CIRCUIT_nSEL;

	REG_PORT_WRCONFIG0 = CIRCUIT_MISO |
		PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PMUXEN | PORT_WRCONFIG_INEN |
		PORT_WRCONFIG_WRPMUX | CIRCUIT_ASIC;

	REG_PORT_WRCONFIG0 = CIRCUIT_nFFS | CIRCUIT_MOSI | CIRCUIT_MSCK |
		PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_PMUXEN |
		PORT_WRCONFIG_WRPMUX | CIRCUIT_ASIC;

	// provide bus clock to SERCOMs
	REG_PM_APBCMASK |= PM_APBCMASK_SERCOM0 | PM_APBCMASK_SERCOM2;

	// configure generic clock 2 to divide 48MHz by 5
	REG_GCLK_GENDIV = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(5);
	REG_GCLK_GENCTRL = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_IDC;
	// provide generic clock 2 at 9.6MHz to SERCOM0
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_SERCOM0_CORE | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_CLKEN;
	// provide generic clock 0 at 48MHz to SERCOM2
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_SERCOM2_CORE | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;

	REG_SERCOM0_SPI_CTRLA = 0; // disable module to allow configuration
	REG_SERCOM0_SPI_CTRLA = SERCOM_SPI_CTRLA_MODE_SPI_MASTER | CIRCUIT_SPI;
	REG_SERCOM0_SPI_CTRLB = SERCOM_SPI_CTRLB_RXEN | SERCOM_SPI_CTRLB_MSSEN;
	REG_SERCOM0_SPI_BAUD = SERCOM_SPI_BAUD_BAUD(1); // 9.6MHz/4 = 2.4MHz
	REG_SERCOM0_SPI_CTRLA |= SERCOM_SPI_CTRLA_ENABLE;

	REG_SERCOM2_SPI_CTRLA = 0; // disable module to allow configuration
	REG_SERCOM2_SPI_CTRLA = SERCOM_SPI_CTRLA_MODE_SPI_MASTER | CIRCUIT_SPI;
	REG_SERCOM2_SPI_CTRLB = SERCOM_SPI_CTRLB_RXEN;
	REG_SERCOM2_SPI_BAUD = SERCOM_SPI_BAUD_BAUD(1); // 48MHz/4 = 12MHz
	REG_SERCOM2_SPI_CTRLA |= SERCOM_SPI_CTRLA_ENABLE;

	// provide bus clock to EIC
	REG_PM_APBAMASK |= PM_APBAMASK_EIC;
	// provide generic clock 2 at 9.6MHz to EIC
	REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_EIC | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_CLKEN;
	// configure pin sensing
	REG_EIC_CONFIG0 =
		EIC_CONFIG_SENSE_FALL(CIRCUIT_PIN_nIRQ) | EIC_CONFIG_SENSE_NONE(CIRCUIT_PIN_nRES) |
		EIC_CONFIG_SENSE_NONE(CIRCUIT_PIN_FFIT) | EIC_CONFIG_SENSE_FALL(CIRCUIT_PIN_VDI);
	// enable external interrupt controller
	REG_EIC_CTRL |= EIC_CTRL_ENABLE;
	// clear spurious flags (errata 1.9.1)
	REG_EIC_INTFLAG = 0xFFFF;
	NVIC_EnableIRQ(EIC_IRQn);
}

__attribute__((noinline,noclone))
uint8_t circuit_asic_byte(uint8_t byte)
{
	REG_PORT_WRCONFIG0 = CIRCUIT_MISO | CIRCUIT_MOSI | CIRCUIT_MSCK | PORT_WRCONFIG_WRPMUX | CIRCUIT_ASIC;
	REG_PORT_OUTCLR0 = CIRCUIT_nSEL;
	REG_SERCOM2_SPI_DATA = byte;
	while (!(REG_SERCOM2_SPI_INTFLAG & SERCOM_SPI_INTFLAG_RXC));
	byte = REG_SERCOM2_SPI_DATA;
	REG_PORT_OUTSET0 = CIRCUIT_nSEL;
	REG_SERCOM2_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
	return byte;
}

__attribute__((noinline,noclone))
uint16_t circuit_asic_word(uint16_t xmit)
{
	uint16_t recv;
	REG_PORT_WRCONFIG0 = CIRCUIT_MISO | CIRCUIT_MOSI | CIRCUIT_MSCK | PORT_WRCONFIG_WRPMUX | CIRCUIT_ASIC;
	REG_PORT_OUTCLR0 = CIRCUIT_nSEL;
	REG_SERCOM2_SPI_DATA = xmit >> 8;
	while (!(REG_SERCOM2_SPI_INTFLAG & SERCOM_SPI_INTFLAG_RXC));
	recv = REG_SERCOM2_SPI_DATA << 8;
	REG_SERCOM2_SPI_DATA = xmit;
	while (!(REG_SERCOM2_SPI_INTFLAG & SERCOM_SPI_INTFLAG_RXC));
	recv |= REG_SERCOM2_SPI_DATA;
	REG_PORT_OUTSET0 = CIRCUIT_nSEL;
	REG_SERCOM2_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
	return recv;
}

__attribute__((noinline,noclone))
uint8_t circuit_fifo_byte_block(uint8_t byte)
{
	REG_PORT_WRCONFIG0 = CIRCUIT_MISO | CIRCUIT_MOSI | CIRCUIT_MSCK | PORT_WRCONFIG_WRPMUX | CIRCUIT_FIFO;
	REG_SERCOM0_SPI_DATA = byte;
	while (!(REG_SERCOM0_SPI_INTFLAG & SERCOM_SPI_INTFLAG_RXC));
	byte = REG_SERCOM0_SPI_DATA;
	REG_SERCOM0_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
	return byte;
}

__attribute__((noinline,noclone))
uint8_t circuit_fifo_byte_yield(uint8_t byte)
{
	REG_PORT_WRCONFIG0 = CIRCUIT_MISO | CIRCUIT_MOSI | CIRCUIT_MSCK | PORT_WRCONFIG_WRPMUX | CIRCUIT_FIFO;
	REG_SERCOM0_SPI_DATA = byte;
	while (!(REG_SERCOM0_SPI_INTFLAG & SERCOM_SPI_INTFLAG_RXC)) fiber_yield();
	byte = REG_SERCOM0_SPI_DATA;
	REG_SERCOM0_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
	return byte;
}

void circuit_eint_yield(uint32_t mask)
{
	while ((REG_EIC_INTFLAG & mask) == 0) fiber_yield();
	REG_EIC_INTFLAG = mask;
}

void circuit_pmux_asic(void) {
	REG_PORT_WRCONFIG0 = CIRCUIT_MISO | CIRCUIT_MOSI | CIRCUIT_MSCK | PORT_WRCONFIG_WRPMUX | CIRCUIT_ASIC;
}
void circuit_pmux_fifo(void) {
	REG_PORT_WRCONFIG0 = CIRCUIT_MISO | CIRCUIT_MOSI | CIRCUIT_MSCK | PORT_WRCONFIG_WRPMUX | CIRCUIT_FIFO;
}

uint_fast8_t circuit_vdi(void) {
	uint_fast8_t vdi = (CIRCUIT_VDI & (REG_PORT_IN0 & ~REG_EIC_INTFLAG)) >> CIRCUIT_PIN_VDI;
	REG_EIC_INTFLAG = CIRCUIT_VDI;
	return vdi;
}
void circuit_vdi_clear(void) { REG_EIC_INTFLAG = CIRCUIT_VDI; }
