#pragma once

#ifndef STORAGE
#define STORAGE

#include <stdlib.h>
#include <stdint.h>

void storage_serial_words(uint32_t serial[static  4]);
void storage_serial_ascii(uint8_t  serial[static 33]);
void storage_serial_utf16(uint16_t serial[static 32]);

typedef struct {
	uint32_t ser[4]; // idn
	uint8_t  ip4[4];
	uint8_t  eth[6];
	uint8_t  xlc;
} storage_t;

void storage_init(void);

void storage_ethmac_ascii(uint8_t  address[static 13]);
void storage_ethmac_utf16(uint16_t address[static 12]);

const uint8_t* storage_router_mac(void);
const uint8_t* storage_router_ip4(void);
const uint8_t* storage_client_mac(void);
const uint8_t* storage_client_ip4(void);
extern const uint8_t storage_subnet_fnk[4];
extern const uint8_t storage_subnet_msk[4];
extern const uint8_t storage_subnet_ip4[4];
extern const uint8_t storage_subnet_brd[4];
uint8_t storage_xlc(void);

#define STORE __attribute__((section(".store")))
extern uint8_t storage_array[256];
uint8_t storage_flash(void * destin, void * source, size_t length);

#endif // STORAGE
