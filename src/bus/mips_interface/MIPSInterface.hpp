#pragma once

#include <cstdint>
#include "MIPSInterrupt.hpp"
#include "../../cpu/CPU.hpp"

class MIPSInterface {
public:
    bool upperMode;
    // number of bytes (minus 1) to write in repeat mode
    uint8_t repeatCount;
    bool repeatMode;
    bool ebus;

    MIPSInterrupt mipsInterrupt;
    MIPSInterrupt mipsMask;
    // TODO: add dp interrupt support

    void write(uint32_t value);
    void setMask(uint32_t value);
};