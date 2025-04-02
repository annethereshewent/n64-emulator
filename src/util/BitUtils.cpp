#pragma once

#include <cstdint>

void setBit(uint32_t* value, int bit) {
    *value |= 1 << bit;
}

void clearBit(uint32_t* value, int bit) {
    *value &= ~(1 << bit);
}