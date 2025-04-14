#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifndef DO3RB_H
#define DO3RB_H

// Preamble
#define DO3RB_PREAMBLE 0x333555u
#define DO3RB_PREAMBLE_1ST 0x33u
#define DO3RB_PREAMBLE_2ND 0x35u
#define DO3RB_PREAMBLE_3RD 0x55u

// Frame Synchronization
#define DO3RB_SYNCHRON 0x190A7Du
#define DO3RB_SYNCHRON_1ST 0x19u
#define DO3RB_SYNCHRON_2ND 0x0Au
#define DO3RB_SYNCHRON_3RD 0x7Du

/*
DO3RBv2 leng outer run 07, inner run 11, concatenated 14
sums of runs 0 0 0 0 0 0 0 0 0 0 0 0 0 24 32 56 144 448 576 1408 2944 6144 12800 26624
leng at indx 07 11 11 10 11 10 09 08 08 11 11 11 11 10 11 08 08 09 10 11 11 10 11 07

01010101 0x55 000110010000101001111101 0x190A7D 011011100110111001101110 0x6E6E6E

crosscorrelation with leading preamble minimal weight difference 11
11 13 11 13 11 13 11 13 11 12 12 12 11 10 12 12 12 13 11 10 11 12 11 12 12 12 13 13 12 12 12 24
*/

bool do3rb_synchron_detect(uint32_t dword);

uint_fast32_t do3rb_golay_encode(uint_fast16_t word);
uint_fast16_t do3rb_golay_decode(uint_fast32_t code);

unsigned do3rb_test_exhaustive(void);

#endif // DO3RB_H
