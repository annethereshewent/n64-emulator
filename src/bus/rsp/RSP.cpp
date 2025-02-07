#pragma once

#include "RSP.hpp"
#include <iostream>
#include "RSPInstructions.cpp"

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

                Bus::writeWord(ramPtr, value);

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

                Bus::writeWord(&bus.rdram[dramAddress], value);

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

uint32_t RSP::readRegisters(uint32_t offset) {
    switch (offset) {
        case 2:
            return spReadLength.value;
            break;
        case 3:
            return spWriteLength.value;
            break;
        case 4:
            return status.value;
            break;
        case 5:
            return status.dmaFull;
            break;
        case 6:
            return status.dmaBusy;
            break;
        case 7: {
            uint32_t returnVal = semaphore;

            semaphore = 1;

            return returnVal;
            break;
        }
        default:
            std::cout << "(rsp read registers)not yet implemented: " << std::dec << offset << "\n";
            exit(1);
            break;
    }
}

void RSP::writeRegisters(uint32_t offset, uint32_t value) {
    switch (offset) {
        case 0:
            dmaMemAddress = value & 0x1ffc;
            break;
        case 1:
            dmaRamAddress = value & 0xfffffc;
            break;
        case 2:
            spReadLength.value = value;

            pushDma(Read);
            break;
        case 3:
            spWriteLength.value = value;

            pushDma(Write);
            break;
        case 4:
            updateStatus(value);
            break;
        case 5:
            status.dmaFull = value & 0b1;
            break;
        case 7:
            semaphore = 0;
            break;
        default:
            std::cout << "(rsp write registers)not yet implemented: " << std::dec << offset << "\n";
            exit(1);
            break;
    }
}

uint64_t RSP::runRsp() {
    cpuBroken = false;
    cycleCounter = 0;

    while (isRunning) {
        uint32_t instruction = memRead32(&imem[pc]);
        instructionType = Scalar;

        uint32_t command = instruction >> 26;

        previousPc = pc;

        pc = nextPc;

        nextPc += 4;

        bool previousDelaySlot = inDelaySlot;

        switch (command) {
            case 0:
                secondary[instruction & 0x3f](this, instruction);
                break;
            case 1:
                switch ((instruction >> 16) & 0x1f) {
                    case 0:
                        RSP::bltz(this, instruction);
                        break;
                    case 1:
                        RSP::bgez(this, instruction);
                        break;
                    case 16:
                        RSP::bltzal(this, instruction);
                        break;
                    case 17:
                        RSP::bgezal(this, instruction);
                        break;
                    default:
                        RSP::reserved(this, instruction);
                        break;
                }
                break;
            case 16:
                switch ((instruction >> 21) & 0x1f) {
                    case 0:
                        RSP::mfc0(this, instruction);
                        break;
                    case 4:
                        RSP::mtc0(this, instruction);
                        break;
                    default:
                        RSP::reserved(this, instruction);
                }
                break;
            case 18:
                switch ((instruction >> 21) & 0x1f) {
                    case 0:
                        RSP::mfc2(this, instruction);
                        break;
                    case 2:
                        RSP::cfc2(this, instruction);
                        break;
                    case 4:
                        RSP::mtc2(this, instruction);
                        break;
                    case 6:
                        RSP::ctc2(this, instruction);
                        break;
                    default:
                        if (((instruction >> 21) & 0x1f) > 15) {
                            instructionType = Vector;
                            vecInstructions[instruction & 0xf](this, instruction);
                        } else {
                            RSP::reserved(this, instruction);
                        }
                        break;
                }
                break;
            case 50: {
                uint32_t op = (instruction >> 11) & 0x1f;

                if (op < lwc2.size()) {
                    lwc2[op](this, instruction);
                } else {
                    RSP::reserved(this, instruction);
                }
                break;
            }
            case 58: {
                uint32_t op = (instruction >> 11) & 0x1f;

                if (op < swc2.size()) {
                    swc2[op](this, instruction);
                } else {
                    RSP::reserved(this, instruction);
                }
                break;
            }
            default:
                instructions[command](this, instruction);
                break;
        }

        if (previousDelaySlot && inDelaySlot) {
            inDelaySlot = false;
            if (cpuBroken) {
                break;
            }
        } else if (cpuBroken) {
            break;
        }

        pc &= 0xffc;
        nextPc &= 0xffc;

        if (instructionType == lastInstructionType) {
            cycleCounter++;
            pipelineFull = false;
        } else {
            lastInstructionType = instructionType;

            if (pipelineFull) {
                cycleCounter++;
                pipelineFull = false;
            } else {
                pipelineFull = true;
            }
        }
    }

    return (uint64_t)((double)cycleCounter * 1.5);
}

void RSP::memWrite32(uint32_t address, uint32_t value) {
    Bus::writeWord(&dmem[address & 0xfff], value);
}


void RSP::memWrite16(uint32_t address, uint16_t value) {
    Bus::writeHalf(&dmem[address & 0xfff], value);
}

uint32_t RSP::memRead32(uint8_t* ptr) {
    return std::byteswap(*(uint32_t*)&ptr[0]);
}

uint16_t RSP::memRead16(uint32_t address) {
    return std::byteswap(*(uint16_t*)&dmem[address]);
}

void RSP::restartRsp() {
    if (cpuBroken) {
        status.broke = 1;
        status.halted = 1;

        if (status.intBreak) {
            bus.setInterrupt(SP_INTERRUPT_FLAG);
        }

        return;
    }
    if (cpuHalted) {
        status.halted = 1;
        return;
    }

    isRunning = true;
    startRsp();
}

uint32_t RSP::getVOffset(uint32_t instruction) {
    return (int8_t)(int32_t)(uint32_t)(((instruction & 0x7f) << 1) >> 1);
}

uint8_t RSP::getVElement(uint32_t instruction) {
    return (uint8_t)((instruction >> 7) & 0xf);
}

uint8_t RSP::getVt(uint32_t instruction) {
    return (uint8_t)((instruction >> 16) & 0x1f);
}
uint8_t RSP::getVs(uint32_t instruction) {
    return (uint8_t)((instruction >> 11) & 0x1f);
}
uint8_t RSP::getVte(uint32_t instruction) {
    return (uint8_t)((instruction >> 21) & 0xf);
}

uint8_t RSP::getVd(uint32_t instruction) {
    return (uint8_t)((instruction >> 6) & 0x1f);
}

uint8_t RSP::memRead8(uint32_t address) {
    return dmem[address & 0xfff];
}

void RSP::memWrite8(uint32_t address, uint8_t value) {
    dmem[address & 0xfff] = value;
}

uint8_t RSP::getVec8(uint8_t vt, uint8_t velement) {
    return vpr[vt][velement];
}

void RSP::setVec8(uint8_t vt, uint8_t velement, uint8_t value) {
    vpr[vt][velement] = value;
}

void RSP::setVec16UnalignedNoWrap(uint8_t vt, uint8_t velement, uint16_t value) {
    setVec8(vt, velement, (uint8_t)(value >> 8));

    if (velement < 15) {
        setVec8(vt, velement + 1, (uint8_t)value);
    }
}

int16_t RSP::getVec16(uint8_t vt, uint8_t element) {
    return std::byteswap(*(uint16_t*)&vpr[vt][element * 2]);
}

void RSP::setVec16(uint8_t vt, uint8_t element, uint16_t value) {
    Bus::writeHalf(&vpr[vt][element * 2], value);
}

void RSP::updateAccumulatorMid32(int element, int32_t result, bool accumulate) {
    int32_t v1 = result >= 0 ? 0 : -1;

    updateAccumulatorHiLo(element, v1, result, accumulate);
}

void RSP::updateAccumulatorHiLo(int element, int32_t v1, int32_t result, bool accumulate) {
    if (accumulate) {
        int32_t x1 = *(int32_t*)&vAcc[(element * 2 + 1) * 4];
        int32_t x0 = *(int32_t*)&vAcc[(element * 2) * 4];

        int32_t z0 = (int64_t)x0 + (int64_t)result;

        int64_t c = ((x0 & result) | ((x0 | result) & ~z0)) >> 31;
        int32_t z1 = x1 + v1 + c;

        writeAcc32((element * 2 + 1) * 4, (uint32_t)((z1 << 16) >> 16));
        writeAcc32((element * 2) * 4, (uint32_t)z0);
    } else {

        std::cout << "writing to accumulate " << std::hex << v1 << " and " << result << "\n";

        writeAcc32((element * 2 + 1) * 4, v1);
        writeAcc32((element * 2) * 4, result);


    }
}

void RSP::updateAccumulatorLow32(int element, uint32_t result, bool accumulate) {
    updateAccumulatorHiLo(element, 0, result, accumulate);
}

void RSP::writeAcc32(int offset, uint32_t value) {
    std::cout << "writing to accumulator 32 bit value " << std::hex << value << " at offset " << std::dec << offset << "\n";
    for (int i = 0; i < 4; i++) {
        int shift = 8 * i;
        vAcc[offset + i] = (uint8_t)(value >> shift);
    }
}

void RSP::setVecFromAccSignedMid(uint8_t vd) {
    for (int i = 0; i < 8; i++) {
        int32_t himid = *(int32_t*)&vAcc[((i * 4) + 1) * 2];

        std::cout << "himid = " << std::hex << himid << "\n";

        int16_t result;

        if (himid >= 0) {
            result = (himid & 0xffff8000) ? 0x7fff : himid;
        } else {
            result = (~himid & 0xffff8000) ? 0x8000 : himid;
        }

        setVec16(vd, i, (uint16_t)result);
    }
}

void RSP::setVecFromAccSignedLow(uint8_t vd) {
    for (int i = 0; i < 8; i++) {
        int32_t himid = *(int32_t*)&vAcc[((i * 4) + 1) * 2];
        int16_t lo = *(int16_t*)&vAcc[(i * 4) * 2];

        int16_t result;

        if (himid >= 0) {
            result = (himid & 0xffff8000) ? 0x7fff : lo;
        } else {
            result = (~himid & 0xffff8000) ? 0x8000 : lo;
        }

        setVec16(vd, i, (uint16_t)result);
    }
}

void RSP::setVecFromAccMid(uint8_t vd) {
    for (int i = 0; i < 8; i++) {
        int16_t mid = *(int16_t*)&vAcc[(i * 4 + 1) * 2];
        setVec16(vd, i, (uint16_t)mid);
    }
}