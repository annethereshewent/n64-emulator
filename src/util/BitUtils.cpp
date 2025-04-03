#pragma once

#include <cstdint>

void setBit(uint32_t* value, int bit) {
    *value |= 1 << bit;
}

void clearBit(uint32_t* value, int bit) {
    *value &= ~(1 << bit);
}

uint8_t getBit(uint32_t value, int bit) {
    return (value >> bit) & 0b1;
}