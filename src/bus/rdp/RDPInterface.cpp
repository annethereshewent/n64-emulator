#pragma once

#include "RDPInterface.hpp"
#include <cstdint>
#include <iostream>
#include "../../interface.cpp"

uint32_t RDPInterface::readRegisters(uint32_t offset) {
    switch (offset) {
        case 0:
            return start;
            break;
        case 1:
            return end;
            break;
        case 2:
            return current;
            break;
        case 3:
            return status.value;
            break;
        default:
            std::cout << "(RDPInterface::readRegisters)offset not implemented: " << std::dec << offset << "\n";
            throw std::runtime_error("");
            break;
    }
}

void RDPInterface::writeRegisters(uint32_t offset, uint32_t value) {
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
                uint64_t cycles = rdp_process_commands();

                status.gclk = 1;
                status.cmdBusy = 1;
                status.pipeBusy = 1;

                if (cycles > 0) {
                    bus.cpu.scheduler.addEvent(Event(RDPEvent, bus.cpu.cop0.count + cycles));
                }
            } else {
                isFrozen = true;
            }
            break;
        case 3:
            updateStatus(value);
            break;
        default:
            std::cout << "(RDPInterface::writeRegisters)offset not implemented: " << std::dec << offset << "\n";
            throw std::runtime_error("");
            break;
    }
}

void RDPInterface::updateStatus(uint32_t value) {
    if ((value & 0b1) == 1) {
        status.xbus = 0;
    }
    if (((value >> 1) & 0b1) == 1) {
        status.xbus = 1;
    }

    if (((value >> 2) & 0b1) == 1) {
        status.freeze = 0;
    }

    if (((value >> 3) & 0b1) == 1) {
        status.freeze = 1;
    }

    if (((value >> 4) & 0b1) == 1) {
        status.flush = 0;
    }

    if (((value >> 5) & 0b1) == 1) {
        status.flush = 1;
    }

    if (((value >> 6) & 0b1) == 1) {
        status.tmemBusy = 0;
    }

    if (((value >> 7) & 0b1) == 1) {
        status.pipeBusy = 0;
    }

    if (((value >> 8) & 0b1) == 1) {
        status.cmdBusy = 0;
    }

    if (((value >> 9) & 0b1) == 1) {
        clockCounter = 0;
    }


}