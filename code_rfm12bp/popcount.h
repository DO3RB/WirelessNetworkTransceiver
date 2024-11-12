#pragma once

#include <stdint.h>

#ifndef POPCOUNT_H
#define POPCOUNT_H

uint_fast8_t popcount(uint32_t v);
uint_fast8_t popcount12 (uint32_t n);
uint_fast8_t popcount24 (uint32_t n);

#endif // POPCOUNT_H
