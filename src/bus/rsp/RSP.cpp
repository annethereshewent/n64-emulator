#pragma once

#include "RSP.hpp"
#include <iostream>
#include "../Bus.hpp"
#include <bit>

void RSP::handleDma(SPDma dma) {
    std::cout << "doing an RSP dma....\n";
    uint32_t length = dma.length.length + 1;
    uint32_t count = dma.length.count + 1;
    uint32_t skip = dma.length.skip;

    uint32_t memAddress = dma.memAddress;
    uint32_t dramAddress = dma.dramAddress;

    bool isImem = (memAddress & 0x1000) != 0;

    if (isImem) {
        memAddress -= 0x1000;
    }

    if (dma.direction == Read) {
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < length; j += 4) {
                uint32_t value = std::byteswap(*(uint32_t*)&bus.rdram[dramAddress]);

                uint8_t* ramPtr = isImem ? &imem[memAddress] : &dmem[memAddress];

                bus.writeWord(ramPtr, value);

                memAddress += 4;
                dramAddress += 4;
            }
            dramAddress += skip;
        }
    } else {
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < length; j += 4) {
                uint8_t* ramPtr = isImem ? &imem[memAddress] : &dmem[memAddress];

                uint32_t value = std::byteswap(*(uint32_t*)ramPtr);

                bus.writeWord(&bus.rdram[dramAddress], value);

                memAddress += 4;
                dramAddress += 4;
            }
            dramAddress += skip;
        }
    }

    dmaMemAddress = memAddress;
    dmaRamAddress = dramAddress;
    spReadLength.value = 0xff8;
    spWriteLength.value = 0xff8;

    bus.cpu.scheduler.addEvent(
        Event(
            RspDmaPop,
            bus.cpu.cop0.count + bus.calculateRdRamCycles(count * length) + 9
        )
    );
}

void RSP::pushDma(DmaDirection direction) {
    if (status.dmaFull) {
        std::cout << "dma fifo is already full\n";
        return;
    }
    SPDma entry;

    entry.length = direction == Read ? spReadLength : spWriteLength;
    entry.direction = direction;
    entry.dramAddress = dmaRamAddress;
    entry.memAddress = dmaMemAddress;

    if (status.dmaBusy) {
        fifo[1] = entry;

        status.dmaFull = 1;
    } else {
        fifo[0] = entry;

        status.dmaBusy = 1;
    }

    handleDma(fifo[0]);
}

void RSP::updateStatus(uint32_t value) {
    bool previousHalt = status.halted;

    if ((value & 0b1) == 1 && ((value >> 1) & 0b1) == 0) {
        status.halted = 0;
    }
    if ((value & 0b1) == 0 & ((value >> 1) & 0b1) == 1) {
        status.halted = 1;
        // todo: remove from scheduler "run rsp" event
    }
    if (((value >> 2) & 0b1) == 1) {
        status.broke = 0;
    }
    if (((value >> 3) & 0b1) == 1 && ((value >> 4) & 0b1) == 0) {
        bus.clearInterrupt(SP_INTERRUPT_FLAG);
    }
    if (((value >> 3) & 0b1) == 0 && ((value >> 4) & 0b1) == 1) {
        std::cout << "setting SP interrupt flag\n";
        bus.setInterrupt(SP_INTERRUPT_FLAG);
    }
    if (((value >> 5) & 0b1) == 1 && ((value >> 6) & 0b1) == 0) {
        status.singleStep = 0;
    }
    if (((value >> 5) & 0b1) == 0 && ((value >> 6) & 0b1) == 1) {
        status.singleStep = 1;
    }
    if (((value >> 7) & 0b1) == 1 && ((value >> 8) & 0b1) == 0) {
        status.intBreak = 0;
    }
    if (((value >> 7) & 0b1) == 0 && ((value >> 8) & 0b1) == 1) {
        status.intBreak = 1;
    }
    if (((value >> 9) & 0b1) == 1 && ((value >> 10) & 0b1) == 0) {
        status.flag0 = 0;
    }
    if (((value >> 9) & 0b1) == 0 && ((value >> 10) & 0b1) == 1) {
        status.flag0 = 1;
    }
    if (((value >> 11) & 0b1) == 1 && ((value >> 12) & 0b1) == 0) {
        status.flag1 = 0;
    }
    if (((value >> 11) & 0b1) == 0 && ((value >> 12) & 0b1) == 1) {
        status.flag1 = 1;
    }
    if (((value >> 13) & 0b1) == 1 && ((value >> 14) & 0b1) == 0) {
        status.flag2 = 0;
    }
    if (((value >> 13) & 0b1) == 0 && ((value >> 14) & 0b1) == 1) {
        status.flag2 = 1;
    }
    if (((value >> 15) & 0b1) == 1 && ((value >> 16) & 0b1) == 0) {
        status.flag3 = 0;
    }
    if (((value >> 15) & 0b1) == 0 && ((value >> 16) & 0b1) == 1) {
        status.flag3 = 1;
    }
    if (((value >> 17) & 0b1) == 1 && ((value >> 18) & 0b1) == 0) {
        status.flag4 = 0;
    }
    if (((value >> 17) & 0b1) == 0 && ((value >> 18) & 0b1) == 1) {
        status.flag4 = 1;
    }
    if (((value >> 19) & 0b1) == 1 && ((value >> 20) & 0b1) == 0) {
        status.flag5 = 0;
    }
    if (((value >> 19) & 0b1) == 0 && ((value >> 20) & 0b1) == 1) {
        status.flag5 = 1;
    }
    if (((value >> 21) & 0b1) == 1 && ((value >> 22) & 0b1) == 0) {
        status.flag6 = 0;
    }
    if (((value >> 21) & 0b1) == 0 && ((value >> 22) & 0b1) == 1) {
        status.flag6 = 1;
    }
    if (((value >> 23) & 0b1) == 1 && ((value >> 24) & 0b1) == 0) {
        status.flag7 = 0;
    }
    if (((value >> 23) & 0b1) == 0 && ((value >> 24) & 0b1) == 1) {
        status.flag7 = 1;
    }

    if (!status.halted && previousHalt) {
        std::cout << "starting RSP CPU\n";
        cpuHalted = false;
        isRunning = true;
        cpuBroken = false;

        startRsp();
    }
}

void RSP::popDma() {
    if (status.dmaFull) {
        fifo[0] = fifo[1];

        status.dmaFull = 0;

        handleDma(fifo[0]);
    } else {
        status.dmaBusy = 0;
    }
}

void RSP::startRsp() {

    uint64_t cycles = runRsp();

    bus.cpu.scheduler.addEvent(Event(RunRspPc, bus.cpu.cop0.count + cycles));
}

uint64_t RSP::runRsp() {
    cpuBroken = false;
    cycleCounter = 0;

    while (isRunning) {
        uint32_t instruction = imemRead32(pc);

        previousPc = pc;

        pc = nextPc;

        nextPc += 4;

        break;
    }

    std::cout << "TODO: RSP CPU\n";
    exit(1);

    return cycleCounter;
}

uint32_t RSP::imemRead32(uint32_t address) {
    return std::byteswap(*(uint32_t*)&imem[address]);

    std::cout << "invalid address given to imem: " << std::hex << address << "\n";
    exit(1);
}