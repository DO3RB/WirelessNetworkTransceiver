#pragma once

#include <stdint.h>

#ifndef CIRCUIT_H
#define CIRCUIT_H

#define CIRCUIT_PIN_FFIT PIN_PA02
#define CIRCUIT_PIN_nIRQ PIN_PA04
#define CIRCUIT_PIN_nRES PIN_PA06
#define CIRCUIT_PIN_VDI  PIN_PA07 // nINT before config, driven low during mcu startup (errata 1.13.2)

#define EIC_CONFIG_SENSE_NONE(pin) (0x0ul << 4*(pin))
#define EIC_CONFIG_SENSE_RISE(pin) (0x1ul << 4*(pin))
#define EIC_CONFIG_SENSE_FALL(pin) (0x2ul << 4*(pin))
#define EIC_CONFIG_SENSE_BOTH(pin) (0x3ul << 4*(pin))
#define EIC_CONFIG_SENSE_LVHI(pin) (0x4ul << 4*(pin))
#define EIC_CONFIG_SENSE_LVLO(pin) (0x5ul << 4*(pin))

#define CIRCUIT_TXEN PORT_PB08
#define CIRCUIT_RXEN PORT_PB09

#define CIRCUIT_FFIT PORT_PA02
#define CIRCUIT_nIRQ PORT_PA04
#define CIRCUIT_nRES PORT_PA06
#define CIRCUIT_VDI  PORT_PA07

//		Function	 Pin		  Mux C 2		Mux D 3		XIAO	MINI
#define CIRCUIT_nFFS PORT_PA05 //				SERCOM0PAD1	1		A3	(D2 PA14)
#define CIRCUIT_MOSI PORT_PA08 // SERCOM0PAD0	SERCOM2PAD0	4		D4
#define CIRCUIT_nSEL PORT_PA09 // SERCOM0PAD1	SERCOM2PAD1	5		D3
#define CIRCUIT_MISO PORT_PA10 // SERCOM0PAD2	SERCOM2PAD2	2		D1
#define CIRCUIT_MSCK PORT_PA11 // SERCOM0PAD3	SERCOM2PAD3	3		D0

#define CIRCUIT_SPI (SERCOM_SPI_CTRLA_DIPO(2) | SERCOM_SPI_CTRLA_DOPO(3))
//#define CIRCUIT_SPI (SERCOM_SPI_CTRLA_DIPO(0) | SERCOM_SPI_CTRLA_DOPO(3)) // loopback

#define CIRCUIT_EINT PORT_WRCONFIG_PMUX(0)
#define CIRCUIT_FIFO PORT_WRCONFIG_PMUX(2)
#define CIRCUIT_ASIC PORT_WRCONFIG_PMUX(3)

void circuit_init(void);

uint8_t  circuit_asic_byte(uint8_t byte);
uint16_t circuit_asic_word(uint16_t xmit);
uint8_t  circuit_fifo_byte_block(uint8_t byte);
uint8_t  circuit_fifo_byte_yield(uint8_t byte);

void circuit_eint_yield(uint32_t mask);

void circuit_pmux_asic(void);
void circuit_pmux_fifo(void);

uint_fast8_t circuit_vdi(void);
void circuit_vdi_clear(void);

#endif // CIRCUIT_H
