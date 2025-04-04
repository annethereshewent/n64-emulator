#include "RDPInterface.hpp"
#include <cstdint>
#include <iostream>
#include <bit>
#include <vector>
#include <array>
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
                #else
                    uint64_t cycles = processRdpCommands();
                #endif

                pipeBusy = 0xffffff;

                setBit(&status, Gclk);
                setBit(&status, CmdBusy);
                setBit(&status, PipeBusy);

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

#ifndef USING_SDL3
    uint64_t RDPInterface::processRdpCommands() {
        std::println("processing RDP commands");
        uint32_t cycles = 0;
        int length = end - current;

        if (length <= 0) {
            return cycles;
        }

        uint32_t uLength = (uint32_t)length  >> 3;

        if ((cmdPtr + uLength) & ~(0x0003FFFF >> 3)) {
            return cycles;
        }

        uint32_t offset = current;

        if (getBit(status, Xbus)) {
            do {
                offset &= 0xff8;

                cmdBuffer[2 * cmdPtr] = std::byteswap(*(uint32_t*)&bus.rsp.dmem[offset]);
                cmdBuffer[2 * cmdPtr + 1] = std::byteswap(*(uint32_t*)&bus.rsp.dmem[offset + 4]);
                offset += sizeof(uint64_t);
                cmdPtr++;
            } while (--length > 0);
        } else {
            do {
                offset &= 0xFFFFF8;

                cmdBuffer[2 * cmdPtr] = std::byteswap(*(uint32_t*)&bus.rdram[offset]);
                cmdBuffer[2 * cmdPtr + 1] = std::byteswap(*(uint32_t*)&bus.rdram[offset + 4]);
                offset += sizeof(uint64_t);
                cmdPtr++;
            } while (--length > 0);
        }

        while (currCmd - cmdPtr < 0)
        {
            uint32_t word1 = cmdBuffer[2 * currCmd];
            uint32_t word2 = cmdBuffer[2 * currCmd + 1];

            uint32_t command = (word1 >> 24) & 63;

            int cmdLength = cmdLengths[command];

            if (cmdPtr - currCmd - cmdLength < 0)
            {
                start = current = end;

                if (enqueuedWords.size() > 0) {
                    cmdsReady = true;
                }

                return cycles;
            }

            if (command >= 8) {
                enqueueCommandWords(cmdLength * 2, &cmdBuffer[2 * currCmd]);
            }

            if (RdpOp(command) == RdpOp::SetScissor)
            {
                uint32_t upperLeftX = ((word1 >> 12) & 0xFFF) >> 2;
                uint32_t upperLeftY = (word1 & 0xFFF) >> 2;
                uint32_t lowerRightX = ((word2 >> 12) & 0xFFF) >> 2;
                uint32_t lowerRightY = (word2 & 0xFFF) >> 2;
                region = (lowerRightX - upperLeftX) * (lowerRightY - upperLeftY);
            }
            if (RdpOp(command) == RdpOp::SyncFull)
            {
                cycles = region;

                if (cycles == 0) {
                    cycles = 5000;
                }
            }

            currCmd += cmdLength;
        }

        cmdPtr = 0;
        currCmd = 0;
        current = end;

        if (enqueuedWords.size() > 0) {
            cmdsReady = true;
        }

        return cycles;
    }

    void RDPInterface::enqueueCommandWords(int numWords, uint32_t* words) {
        std::vector<uint32_t> result = {};
        for (int i = 0; i < numWords; i++) {
            result.push_back(words[i]);
        }

        enqueuedWords.push_back(result);
    }

    void RDPInterface::clearCommands() {
        enqueuedWords.clear();
    }
#endif