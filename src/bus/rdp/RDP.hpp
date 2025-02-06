#pragma once

#include "DPCStatus.hpp"

class RDP {
public:
    DPCStatus status;

    uint32_t readRegisters(uint32_t offset);
};