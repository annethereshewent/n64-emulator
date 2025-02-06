#pragma once

#include "RDP.hpp"
#include <cstdint>
#include <iostream>

uint32_t RDP::readRegisters(uint32_t offset) {
    switch (offset) {
        case 3:
            return status.value;
            break;
        default:
            std::cout << "(RD::readRegisters)offset not implemented: " << std::dec << offset << "\n";
            exit(1);
            break;
    }
}