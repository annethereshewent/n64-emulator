#pragma once

#include <cstdint>
#include "../../cpu/CPUInstruction.hpp"

class DCache {
public:
    bool valid;
    bool dirty;
    uint32_t tag;
    uint16_t index;
    std::array<uint32_t, 4> words;
};
