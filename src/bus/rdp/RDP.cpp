#pragma once

#include "RDP.hpp"
#include <cstdint>
#include <iostream>

uint32_t RDP::readRegisters(uint32_t offset) {
    switch (offset) {
        case 0:
            // TODO
            return start;
            break;
        case 1:
            // todo: implement this register
            return end;
            break;
        case 2:
            // see above
            return 0;
            break;
        case 3:
            return status.value;
            break;
        default:
            std::cout << "(RDP::readRegisters)offset not implemented: " << std::dec << offset << "\n";
            exit(1);
            break;
    }
}

void RDP::writeRegisters(uint32_t offset, uint32_t value) {
    switch (offset) {
        case 0:
            if (!status.startPending) {
                start = value;
                status.startPending = 1;
            }
            break;
        case 1:
            end = value;
            if (status.startPending) {
                current = start;

                status.startPending = 0;
            }
            if (!status.freeze) {
                // TODO: start rdp
            } else {
                isFrozen = true;
            }
            break;
        case 3:
            status.value = value;
            break;
        default:
            std::cout << "(RDP::writeRegisters)offset not implemented: " << std::dec << offset << "\n";
            exit(1);
            break;
    }
}