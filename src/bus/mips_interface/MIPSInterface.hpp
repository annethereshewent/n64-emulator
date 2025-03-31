#pragma once

#include <cstdint>
#include "MIPSInterrupt.hpp"
#include "../../cpu/CPU.hpp"

class MIPSInterface {
public:
    bool upperMode = false;
    // number of bytes (minus 1) to write in repeat mode
    uint8_t repeatCount = 0;
    bool repeatMode = false;
    bool ebus = false;

    uint32_t mipsVersion = 0x2020102;

    MIPSInterrupt mipsInterrupt;
    MIPSInterrupt mipsMask;

    void write(uint32_t value);
    void setMask(uint32_t value);
};