#include "RDPInterface.hpp"
#include <cstdint>
#include <iostream>
#include "../../config.hpp"
#ifdef USING_SDL3
    #include "../../parallel-rdp-files/interface.cpp"
#endif
#include "../Bus.hpp"
#include "../../cpu/CPU.hpp"
#include "../../util/BitUtils.cpp"

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
            clearBit(&status, DPCDmaBusy);
            return status;
            break;
        case 4:
            return 0xffffff;
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
            if (((status >> StartPending) & 0b1) == 0) {
                start = value;
                setBit(&status, StartPending);
            }
            break;
        case 1:
            end = value;
            if (((status >> StartPending) & 0b1) == 1) {
                current = start;

                clearBit(&status, StartPending);
            }
            if (((status >> Freeze) & 0b1) == 0) {
                #ifdef USING_SDL3
                    uint64_t cycles = rdp_process_commands();
                    pipeBusy = 0xffffff;

                    setBit(&status, Gclk);
                    setBit(&status, CmdBusy);
                    setBit(&status, PipeBusy);

                    if (cycles > 0) {
                        bus.cpu.scheduler.addEvent(Event(RDPEvent, bus.cpu.cop0.count + cycles));
                    }
                #endif
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
        clearBit(&status, Xbus);
    }
    if (((value >> 1) & 0b1) == 1) {
        setBit(&status, Xbus);
    }

    if (((value >> 2) & 0b1) == 1) {
        clearBit(&status, Freeze);
    }

    if (((value >> 3) & 0b1) == 1) {
        setBit(&status, Freeze);
    }

    if (((value >> 4) & 0b1) == 1) {
        clearBit(&status, Flush);
    }

    if (((value >> 5) & 0b1) == 1) {
        setBit(&status, Flush);
    }

    if (((value >> 6) & 0b1) == 1) {
        clearBit(&status, TmemBusy);
    }

    if (((value >> 7) & 0b1) == 1) {
        clearBit(&status, PipeBusy);
        pipeBusy = 0;
    }

    if (((value >> 8) & 0b1) == 1) {
        clearBit(&status, CmdBusy);
    }

    if (((value >> 9) & 0b1) == 1) {
        clockCounter = 0;
    }
}