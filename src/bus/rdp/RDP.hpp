#pragma once

#include "DPCStatus.hpp"

class RDP {
public:
    DPCStatus status;

    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t current = 0;
    bool isFrozen = false;
    bool frameFinished = false;

    uint32_t readRegisters(uint32_t offset);
    void writeRegisters(uint32_t offset, uint32_t value);
};