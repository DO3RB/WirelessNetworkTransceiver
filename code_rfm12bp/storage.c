#include "storage.h"

#include "stdio.h"
#include "string.h"

void storage_serial_words(uint32_t serial[static  4])
{
	for (uint_fast8_t n = 0; n < 4; n++) {
		serial[n] = *(uint32_t*)(0x0080a00cU+(0x30U*((n+3U)/4U))+(n*4U));
	}
}

void storage_serial_ascii(uint8_t  serial[static 33])
{
	unsigned int words[4];
	storage_serial_words((uint32_t*) words);
	sprintf((char*) serial, "%08X%08X%08X%08X",	words[0], words[1], words[2], words[3]);
}

void storage_serial_utf16(uint16_t serial[static 32])
{
	uint8_t ascii[33];
	storage_serial_ascii(ascii);
	for (uint8_t n = 0; n < 32; n++) serial[n] = ascii[n];
}

const storage_t storage_data[] = {
{{0x00000000,0x00000000,0x00000000,0x00000000}, {0x0A,0x00,0x49,0x10}, {0x00,0xA0,0xAD,0x00,0x73,0x00}, 20},
{{0x15501A0E,0x50544135,0x342E3120,0xFF0B332F}, {0x0A,0x00,0x49,0x01}, {0x00,0xA0,0xAD,0x00,0x73,0x01}, 21}, // +4.5 ppm
{{0xC3258D7D,0x50573539,0x382E3120,0xFF161319}, {0x0A,0x00,0x49,0x02}, {0x00,0xA0,0xAD,0x00,0x73,0x02}, 20}, // +6.8
{{0xF6A25BD5,0x51543051,0x46202020,0xFF15260E}, {0x0A,0x00,0x49,0x03}, {0x00,0xA0,0xAD,0x00,0x73,0x03}, 21}, // +1.0
{{0xB58AE024,0x51543051,0x46202020,0xFF0F2C1A}, {0x0A,0x00,0x49,0x04}, {0x00,0xA0,0xAD,0x00,0x73,0x04}, 20}, // +7.6
{{0x72C7B3A2,0x50573539,0x382E3120,0xFF160C3D}, {0x0A,0x00,0x49,0x05}, {0x00,0xA0,0xAD,0x00,0x73,0x05}, 20}, // +0.8
{{0xAAB83496,0x50544135,0x342E3120,0xFF0C0E3C}, {0x0A,0x00,0x49,0x06}, {0x00,0xA0,0xAD,0x00,0x73,0x06}, 20}, // +4.1
{{0x4F3D9573,0x50583833,0x312E3120,0xFF0D3419}, {0x0A,0x00,0x49,0x07}, {0x00,0xA0,0xAD,0x00,0x73,0x07}, 21}, // +4.3
{{0x87F5FE5F,0x50573539,0x382E3120,0xFF16090D}, {0x0A,0x00,0x49,0x08}, {0x00,0xA0,0xAD,0x00,0x73,0x08}, 21}, // +3.2
{{0x6E3BE83E,0x50553441,0x302E3120,0xFF151242}, {0x0A,0x00,0x49,0x09}, {0x00,0xA0,0xAD,0x00,0x73,0x09}, 20}, // ?
{{0xE52AEACD,0x50553441,0x302E3120,0xFF182922}, {0x0A,0x00,0x49,0x0A}, {0x00,0xA0,0xAD,0x00,0x73,0x0A}, 20}, // ?
{{0x00000000,0x00000000,0x00000000,0x00000000}, {0x0A,0x00,0x49,0x0E}, {0x00,0xA0,0xAD,0x00,0x73,0x0E}, 20},
};

static uint8_t storage_index;

void storage_init(void)
{
	uint32_t words[4];
	storage_serial_words(words);
	for (storage_index = 0; storage_index < sizeof(storage_data)/sizeof(storage_t)-1; storage_index++) {
		if (memcmp(storage_data[storage_index].ser, words, sizeof(words)) == 0) return;
	}
}

void storage_ethmac_ascii(uint8_t  address[static 13])
{
	uint8_t const * m = storage_data[storage_index].eth;
	sprintf((char*) address, "%02x%02x%02x%02x%02x%02x", m[0], m[1], m[2], m[3], m[4], m[5]);
}

void storage_ethmac_utf16(uint16_t address[static 12])
{
	uint8_t ascii[13];
	storage_ethmac_ascii(ascii);
	for (uint8_t n = 0; n < 12; n++) address[n] = ascii[n];
}

const uint8_t* storage_router_mac(void) { return storage_data[0].eth; }
const uint8_t* storage_router_ip4(void) { return storage_data[0].ip4; }
const uint8_t* storage_client_mac(void) { return storage_data[storage_index].eth; }
const uint8_t* storage_client_ip4(void) { return storage_data[storage_index].ip4; }
const uint8_t  storage_subnet_fnk[4] = {0xFF,0xFF,0xFF,0xF0};
const uint8_t  storage_subnet_msk[4] = {0xFF,0xFF,0xFF,0xE0};
const uint8_t  storage_subnet_ip4[4] = {0x0A,0x00,0x49,0x00};
const uint8_t  storage_subnet_brd[4] = {0x0A,0x00,0x49,0x0F};

uint8_t storage_xlc(void)
{
	return storage_data[storage_index].xlc;
}


uint8_t storage_array STORE [256];

#include "samd21g18a/sam.h"
#include <malloc.h>

uint8_t storage_flash [[gnu::noinline,gnu::noclone]] (void * destin, void * source, size_t length) // nvmcpy
{
	const ssize_t row_size = NVMCTRL_PAGE_SIZE * NVMCTRL_ROW_PAGES;
	uint8_t *dst = destin; uint8_t *src = source; ssize_t len = length;
	uint8_t *row_head = (uint8_t*) ((intptr_t) dst & ~(row_size-1));
	uint8_t *buf = malloc(row_size); if (!buf) return 0;
	while (len > 0) {
		ssize_t row_offs = dst - row_head;
		// copy row content into buffer
		memcpy(buf, row_head, row_size);
		// overwrite changes in buffer
		memcpy(buf+row_offs, src, len < row_size ? len : row_size);
		// erase row of four pages
		REG_NVMCTRL_ADDR = (intptr_t) row_head / 2;
		REG_NVMCTRL_CTRLA = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
		while (!NVMCTRL->INTFLAG.bit.READY);
		// write modified buffer into flash row
		volatile uint32_t *row_addr = (uint32_t*) row_head;
		volatile uint32_t *buf_addr = (uint32_t*) buf;
		while ((uint8_t*) row_addr < row_head + row_size) {
			// clear page buffer
			REG_NVMCTRL_CTRLA = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
			while (!NVMCTRL->INTFLAG.bit.READY);
			// write temp buffer into page buffer
			for (unsigned n = 0; n < NVMCTRL_PAGE_SIZE/sizeof(uint32_t); n++)
				*row_addr++ = *buf_addr++; // access as words (or shorts)
			// write page buffer into flash
			REG_NVMCTRL_CTRLA = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
			while (!NVMCTRL->INTFLAG.bit.READY);
		}
		dst = (row_head += row_size);
		src += row_size - row_offs;
		len -= row_size - row_offs;
	}
	free(buf);
	return 1;
}

void storage_constructor_delist ( void(*function)(void) )
{
	extern void (*__init_array []) (void);
	extern size_t __init_count [];

	for (size_t n = 0; n < (size_t) __init_count; n++) {
		if (__init_array[n] == function) {
			storage_flash(&__init_array[n], &(uint32_t[]){0}, sizeof(void*));
		}
	}
}

#include "samd21g18a/console.h"

void storage_test_store(void)
{
	uint8_t* mem = malloc(256);
	if (!mem) return;
	memset(mem, 0x00, 256);
	scanf("%[^\r]255s", mem); getchar();
	storage_flash(storage_array, mem, sizeof(storage_array));
	free(mem);
	printf("\r\n");
}
CONSOLE_RUN(store, storage_test_store)

void storage_test_recall(void)
{
	// recall of unwritten store hardfaults
	// flash probably default 0xFF no string null found
	printf("%s\r\n", storage_array);
}
CONSOLE_RUN(recall, storage_test_recall)
