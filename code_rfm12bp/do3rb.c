#include "do3rb.h"

#include "popcount.h"

bool do3rb_synchron_detect(uint32_t dword)
{
	// candidate differs from pattern in less than five bits
	return popcount24(dword ^ DO3RB_SYNCHRON) < 5;
}

static unsigned MUL(unsigned LH, unsigned RH)
{	asm ("mul %0, %1" : "+l" (LH) : "l" (RH) : "cc"); return LH; }

// rows equal syndromes of single-bit errors in data
const uint16_t do3rb_parity[12] = {
	0b101010101011,
	0b010101010111,
	0b001111100110,
	0b011111001001,
	0b111110010010,
	0b111100100101,
	0b111001001110,
	0b110010011101,
	0b100100111110,
	0b001001111101,
	0b010011111010,
	0b100111110001,
};

// Generator matrix permuted by DO3RB 2024
uint_fast32_t do3rb_golay_encode(uint_fast16_t word)
{
	uint_fast32_t code = 0;
	code ^= 0b101100010101000010100000 * (word & 1); word >>= 1;
	code ^= 0b010100001011000101010000 * (word & 1); word >>= 1;
	code ^= 0b001100001110001010010000 * (word & 1); word >>= 1;
	code ^= 0b011000001111000000100010 * (word & 1); word >>= 1;
	code ^= 0b111100001100000001000100 * (word & 1); word >>= 1;
	code ^= 0b111000101001000010010000 * (word & 1); word >>= 1;
	code ^= 0b111100000010010000110000 * (word & 1); word >>= 1;
	code ^= 0b110000000101000001111000 * (word & 1); word >>= 1;
	code ^= 0b100100001000100011110000 * (word & 1); word >>= 1;
	code ^= 0b001001000011000011110000 * (word & 1); word >>= 1;
	code ^= 0b010100000110000011100001 * (word & 1); word >>= 1;
	code ^= 0b100010001111000011000000 * (word & 1);
	code ^= 0b011011100110111001101110;
	return code;
}

const uint8_t do3rb_reverse_index[24] = {
// D00 P00 D01 P01 D02 P02 D03 P03 D04 P04 D05 P05
    16, 12,  8, 20,  9,  4,  1,  5,  2,  6, 17,  7,
// D06 P06 D07 P07 D08 P08 D09 P09 D10 P10 D11 P11
    10, 13,  3, 14, 11, 15, 18, 21,  0, 22, 19, 23,
};

// dodecahedral Golaycode is self-dual
const uint32_t do3rb_reverse[12] = {
	0b00001010101010110000000000000001,
	0b00000101010101110000000000000010,
	0b00000011111001100000000000000100,
	0b00000111110010010000000000001000,
	0b00001111100100100000000000010000,
	0b00001111001001010000000000100000,
	0b00001110010011100000000001000000,
	0b00001100100111010000000010000000,
	0b00001001001111100000000100000000,
	0b00000010011111010000001000000000,
	0b00000100111110100000010000000000,
	0b00001001111100010000100000000000,
};

uint_fast32_t do3rb_golay_decode(uint_fast32_t code)
{
	uint_fast32_t P1D0 = 0, D1P0 = 0; code ^= 0x6E6E6E;
	#pragma GCC unroll 12
	for (unsigned n = 0; n < 12; n++) {
	//	unshuffle codeword, split parity and data, calculate inverses
		P1D0 ^= MUL(code>>do3rb_reverse_index[2*n+0]&1,do3rb_reverse[n]);
		D1P0 ^= MUL(code>>do3rb_reverse_index[2*n+1]&1,do3rb_reverse[n]);
	}
	uint_fast16_t P0 = D1P0 & 0xFFFF; uint_fast16_t D1 = D1P0 >> 16;
	uint_fast16_t D0 = P1D0 & 0xFFFF; uint_fast16_t P1 = P1D0 >> 16;
	// syndrome weight less than four means
	if (popcount12(P0 ^ P1) < 4) return D0; // all errors in parity
	if (popcount12(D0 ^ D1) < 4) return D1; // all errors in data
	// syndrome difference closer than four to coset leader
	#pragma GCC unroll 12
	for (unsigned n = 0; n < 12; n++) {
		// one error in data, one or two errors in parity
		// one error in parity, one or two errors in data
		if (popcount12(P0^P1^do3rb_parity[n]) < 4) return D0^(1<<n);
		if (popcount12(D0^D1^do3rb_parity[n]) < 4) return D1^do3rb_parity[n];
	}
	return D0; // more than three errors, good luck
}

#include <stdio.h>
#include "samd21g18a/fiber.h"

uint32_t do3rb_test_permute(uint32_t v)
{// https://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
	uint32_t t = v | (v - 1);
	return (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}
void do3rb_test_exhaustive(void)
{
	unsigned int count[5] = {0}, correct[5] = {0}, code, error;
	for (uint16_t n = 0; n <= 0xFFF; n += 1) {
		code = do3rb_golay_encode(n);
		count[0] += 1; if (n == (uint16_t) do3rb_golay_decode(code))       correct[0] += 1;
	for (error = 0x1; error < (1<<24); error = do3rb_test_permute(error)) {
		count[1] += 1; if (n == (uint16_t) do3rb_golay_decode(code^error)) correct[1] += 1; }
	for (error = 0x3; error < (1<<24); error = do3rb_test_permute(error)) {
		count[2] += 1; if (n == (uint16_t) do3rb_golay_decode(code^error)) correct[2] += 1; }
	for (error = 0x7; error < (1<<24); error = do3rb_test_permute(error)) {
		count[3] += 1; if (n == (uint16_t) do3rb_golay_decode(code^error)) correct[3] += 1; }
	for (error = 0xF; error < (1<<24); error = do3rb_test_permute(error)) {
		count[4] += 1; if (n == (uint16_t) do3rb_golay_decode(code^error)) correct[4] += 1; }
	fiber_yield();
	}
	printf("Errors in 0 bits recovered %3d%% %d\r\n", 100*correct[0]/count[0], count[0]);
	printf("Errors in 1 bit  recovered %3d%% %d\r\n", 100*correct[1]/count[1], count[1]);
	printf("Errors in 2 bits recovered %3d%% %d\r\n", 100*correct[2]/count[2], count[2]);
	printf("Errors in 3 bits recovered %3d%% %d\r\n", 100*correct[3]/count[3], count[3]);
	printf("Errors in 4 bits recovered %3d%% %d\r\n", 100*correct[4]/count[4], count[4]);
}
