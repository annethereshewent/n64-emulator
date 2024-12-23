#pragma once

#include <cstdint>
#include "../../cpu/CPUInstruction.hpp"

class ICache {
public:
    bool valid;
    uint32_t tag;
    uint16_t index;
    std::array<uint32_t, 8> words;
    std::array<CPUInstruction, 8> instructions;

};