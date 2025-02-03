#pragma once

#include "Bus.hpp"
#include <iostream>
#include <bit>
#include "pif/PIF.cpp"
#include "../cpu/CPU.hpp"

uint32_t SP_INTERRUPT_FLAG = 1;
uint32_t SI_INTERRUPT_FLAG = 1 << 1;
uint32_t AI_INTERRUPT_FLAG = 1 << 2;
uint32_t VI_INTERRUPT_FLAG = 1 << 3;
uint32_t PI_INTERRUPT_FLAG = 1 << 4;

uint8_t Bus::memRead8(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);
    bool cached = (address & 0x20000000) == 0;

    if (cached) {
        uint32_t value = readDataCache(actualAddress & ~0x3);

        switch (actualAddress & 0x3) {
            case 0:
                return (uint8_t)(value >> 24);
                break;
            case 1:
                return (uint8_t)(value >> 16);
                break;
            case 2:
                return (uint8_t)(value >> 8);
                break;
            case 3:
                return (uint8_t)value;
                break;
        }
    }

    switch (actualAddress) {
        default:
            if (actualAddress <= 0x03EFFFFF) {
                return rdram[actualAddress];
            }
            if (actualAddress >= 0x10000000 && actualAddress <= 0x1FBFFFFF) {
                uint64_t offset = actualAddress - 0x10000000;

                return cartridge[offset];
            }
            std::cout << "(memRead8) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

uint16_t Bus::memRead16(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);
    bool cached = (address & 0x20000000) == 0;

    if (cached) {
        uint32_t value = readDataCache(actualAddress & ~0x3);

        if ((actualAddress & 0x3) == 0) {
            return (uint16_t)(value >> 16);
        }

        return (uint16_t)value;
    }

    switch (actualAddress) {
        default:
            if (actualAddress <= 0x03EFFFFF) {
                return std::byteswap(*(uint16_t*)&rdram[actualAddress]);
            }
            std::cout << "(memRead16) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

void Bus::memWrite8(uint64_t address, uint8_t value) {
    uint64_t actualAddress = Bus::translateAddress(address);

    bool cached = (address & 0x20000000) == 0;

    if (cached) {
        switch (actualAddress & 0x3) {
            case 0:
                writeDataCache(actualAddress & ~0x3, (uint32_t)value << 24, 0xff << 24);
                break;
            case 1:
                writeDataCache(actualAddress & ~0x3, (uint32_t)value << 16, 0xff << 16);
                break;
            case 2:
                writeDataCache(actualAddress & ~0x3, (uint32_t)value << 8, 0xff << 8);
                break;
            case 3:
                writeDataCache(actualAddress & ~0x3, (uint32_t)value, 0xff);
                break;
        }

        return;
    }

    switch (actualAddress) {
        default:
            if (actualAddress <= 0x03EFFFFF) {
                rdram[actualAddress] = value;

                return;
            }

            std::cout << "(memWrite8) unsupported address given: " << std::hex << address << "\n";
            exit(1);
            break;
    }
}

void Bus::memWrite16(uint64_t address, uint16_t value) {
    uint64_t actualAddress = Bus::translateAddress(address);

    bool cached = (address & 0x20000000) == 0;

    if (cached) {
        if ((actualAddress & 0x3) == 0) {
            uint32_t returnValue = (uint32_t)value << 16;
            writeDataCache(actualAddress & ~0x3, (uint32_t)value << 16, 0xffff0000);
        } else {
            writeDataCache(actualAddress & ~0x3, (uint32_t)value, 0x0000ffff);
        }

        return;
    }

    switch (actualAddress) {
        default:
            if (actualAddress <= 0x03EFFFFF) {
                writeHalf(&rdram[actualAddress], value);

                return;
            }

            std::cout << "(memWrite16) unsupported address given: " << std::hex << address << "\n";
            exit(1);
            break;
    }
}

void Bus::memWrite64(uint64_t address, uint64_t value) {
    uint64_t actualAddress = Bus::translateAddress(address);

    bool cached = (address & 0x20000000) == 0;

    if (cached) {
        writeDataCache(actualAddress + 4, (uint32_t)value);
        writeDataCache(actualAddress, (uint32_t)(value >> 32));
        return;
    }

    if (actualAddress <= 0x03EFFFFF) {
        writeDoubleWord(&rdram[actualAddress], value);
        return;
    }
}

uint64_t Bus::memRead64(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);
    bool cached = (address & 0x20000000) == 0;

    if (cached) {
        return (uint64_t)readDataCache(address) << 32 | (uint64_t)readDataCache(address + 4);
    }

    if (actualAddress <= 0x03EFFFFF) {
        return std::byteswap(*(uint64_t*)&rdram[actualAddress]);
    }

    std::cout << "(memRead64) unsupported address given: " << std::hex << address << "\n";
    exit(1);

}
uint32_t Bus::memRead32(uint64_t address, bool ignoreCache) {
    uint64_t actualAddress = Bus::translateAddress(address);

    bool cached = (address & 0x20000000) == 0;

    if (cached && !ignoreCache) {
        return readDataCache(actualAddress);
    }

    switch (actualAddress) {
        case 0x4040010:
            return rsp.status.value;
            break;
        case 0x4040018:
            return rsp.status.dmaBusy;
            break;
        case 0x4080000:
            return rsp.pc;
            break;
        case 0x410000c:
            return rdp.status.value;
            break;
        case 0x4300008:
            return mips.mipsInterrupt.value;
            break;
        case 0x430000c:
            return mips.mipsMask.value;
            break;
        case 0x4400010:
            // TODO: implement V_CURRENT
            return videoInterface.vcurrent.value;
            break;
        case 0x4500004:
            // TODO: actually return the current audio length
            return audioInterface.audioLength;
            break;
        case 0x450000c:
            // TODO: implement this audio register
            return 0;
            break;
        case 0x4600010:
            return peripheralInterface.piStatus.value;
            break;
        case 0x470000C:
            // just return 0x14 to skip the initialization process
            // TODO: actually implement rdInterface related stuff
            return 0x14;
            // return rdInterface.select.value;
            break;
        case 0x4800000:
            return serialInterface.dramAddress;
            break;
        case 0x4800018:
            return serialInterface.status.value;
            break;
        default:
            if (actualAddress <= 0x03EFFFFF) {
                return std::byteswap(*(uint32_t*)&rdram[actualAddress]);
            }
            if (actualAddress >= 0x08000000 && actualAddress <= 0x0FFFFFFF) {
                // cartridge sram
                // TODO: implement saves
                return 0xff;
            }
            if (actualAddress >= 0x4000000 && actualAddress <= 0x4000FFF) {
                uint64_t offset = actualAddress - 0x4000000;

                return std::byteswap(*(uint32_t*)&rsp.dmem[offset]);
            }
            if (actualAddress >= 0x4001000 && actualAddress <= 0x4001FFF) {
                uint64_t offset = actualAddress - 0x4001000;

                return std::byteswap(*(uint32_t*)&rsp.imem[offset]);
            }
            if (actualAddress >= 0x10000000 && actualAddress <= 0x1FBFFFFF) {
                uint32_t offset = actualAddress - 0x10000000;

                if (offset < cartridge.size()) {
                    return std::byteswap(*(uint32_t*)&cartridge[offset]);
                }

                std::cout << "invalid offset given to cartridge: " << std::hex << offset << "\n";
                exit(1);
            }
            if (actualAddress >= 0x1FC00000 && actualAddress <= 0x1FC007BF) {

                uint64_t pifAddress = actualAddress - 0x1FC00000;

                return std::byteswap(*(uint32_t*)&PIF_BOOT_ROM[pifAddress]);
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                cpu->cop0.addCycles(3000);
                uint32_t offset = actualAddress - 0x1fc007c0;

                return std::byteswap(*(uint32_t*)&pif.ram[offset]);
            }

            std::cout << "(memRead32) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

void Bus::memWrite32(uint64_t address, uint32_t value, bool ignoreCache) {
    uint64_t actualAddress = Bus::translateAddress(address);

    bool cached = (address & 0x20000000) == 0;

    if (cached && !ignoreCache) {
        writeDataCache(actualAddress, value);
        return;
    }

    switch (actualAddress) {
        case 0x4040000:
            rsp.dmaMemAddress = value & 0x1ffc;
            break;
        case 0x4040004:
            rsp.dmaRamAddress = value & 0xfffffc;
            break;
        case 0x4040008:
            rsp.spReadLength.value = value;

            rsp.pushDma(Read);

            handleRspDma(rsp.fifo[0]);
            break;
        case 0x4040010:
            // rsp.status.value = value;
            updateRspStatus(value);
            break;
        case 0x4080000:
            rsp.pc = value & 0xffc;
            break;
        case 0x410000c:
            rdp.status.value = value;
            break;
        case 0x4300000:
            mips.write(value);
            // TODO: clear dp interrupt
            checkIrqs();
            cpu->checkIrqs();
            break;
        case 0x430000c:
            mips.setMask(value);
            checkIrqs();
            cpu->checkIrqs();
            break;
        case 0x4400000:
            videoInterface.ctrl.value = value & 0x1ffff;
            videoInterface.ctrl.unused = 0;
            break;
        case 0x4400004:
            videoInterface.origin = value & 0xffffff;
            break;
        case 0x4400008:
            videoInterface.width = value & 0xfff;
            break;
        case 0x440000c:
            videoInterface.vInterrupt = value & 0x3ff;
            break;
        case 0x4400010:
            clearInterrupt(VI_INTERRUPT_FLAG);
            break;
        case 0x4400014:
            videoInterface.viBurst = value & 0x3fffffff;
            break;
        case 0x4400018:
            if (videoInterface.vTotal != (value & 0x3ff)) {
                videoInterface.vTotal = value & 0x3ff;

                recalculateDelay();

                if (!videoInterface.interruptStarted) {
                    videoInterface.interruptStarted = true;

                    cpu->scheduler.addEvent(Event(VideoInterrupt, cpu->cop0.count + videoInterface.delay));
                }
            }
            break;
        case 0x440001c:
            if (videoInterface.hTotal != (value & 0xfff)) {
                videoInterface.hTotal = value & 0xfff;
                recalculateDelay();
            }
            break;
        case 0x4400020:
            videoInterface.hTotalLeap.value = value & 0xfffffff;
            videoInterface.hTotalLeap.unused = 0;
            break;
        case 0x4400024:
            videoInterface.hVideo = value & 0x3ff;
            break;
        case 0x4400028:
            videoInterface.vVideo.value = value & 0x3ffffff;
            videoInterface.vVideo.unused = 0;
            break;
        case 0x440002c:
            videoInterface.vBurst = value & 0x3ffffff;
            break;
        case 0x4400030:
            videoInterface.xScale.value = value & 0xfffffff;
            videoInterface.xScale.unused = 0;
            break;
        case 0x4400034:
            videoInterface.yScale.value = value & 0x3ffffff;
            videoInterface.yScale.unused = 0;
            break;
        case 0x4500000:
            audioInterface.dramAddress = value & 0xffffff;
            break;
        case 0x4500004:
            audioInterface.audioLength = value & 0x3ffff;
            break;
        case 0x4500008:
            audioInterface.dmaEnable = (value & 0b1) == 1;
            break;
        case 0x450000c:
            clearInterrupt(AI_INTERRUPT_FLAG);
            break;
        case 0x4500010:
            audioInterface.dacRate = value & 0x3fff;
            break;
        case 0x4500014:
            audioInterface.bitRate = value & 0xf;
            break;
        case 0x4600000:
            peripheralInterface.dramAddress = value & 0xfffffe;
            break;
        case 0x4600004:
            peripheralInterface.cartAddress = value & 0xfffffe;
            break;
        case 0x460000c:
            peripheralInterface.wrLen = value & 0xffffff;
            dmaWrite();
            setInterrupt(PI_INTERRUPT_FLAG);
            break;
        case 0x4600010:
            if ((value & 0b1) == 1) {
                // TODO: reset dma controller from doing transfer
                std::cout << "it should reset the dma controller from doing a transfer.\n";
            }
            if (((value >> 1) & 0b1) == 1) {
                clearInterrupt(PI_INTERRUPT_FLAG);
                peripheralInterface.piStatus.dmaCompleted = 0;
            }
            break;
        case 0x4600014:
            peripheralInterface.dom1Latch = value & 0xff;
            break;
        case 0x4600018:
            peripheralInterface.dom1Pwd = value & 0xff;
            break;
        case 0x460001c:
            peripheralInterface.dom1Pgs = value & 0xff;
            break;
        case 0x4600020:
            peripheralInterface.dom1Rls = value & 0xff;
            break;
        case 0x4700000:
            rdInterface.mode.value = value & 0xf;
            break;
        case 0x4700004:
            rdInterface.config.value = value & 0x7f;
            break;
        case 0x4700008:
            rdInterface.currentLoad = value;
            break;
        case 0x470000C:
            rdInterface.select.value = value & 0xff;
            break;
        case 0x4800018:
            serialInterface.status.interrupt = 0;
            clearInterrupt(SI_INTERRUPT_FLAG);
            break;
        default:
            if (actualAddress <= 0x03EFFFFF) {
                uint32_t cycles = ignoreCache ? 9 : 32;

                cpu->cop0.addCycles(cycles);

                Bus::writeWord(&rdram[actualAddress], value);

                return;
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                uint32_t offset = actualAddress - 0x1fc007c0;

                Bus::writeWord(&pif.ram[offset], value);

                cpu->scheduler.addEvent(Event(PIFExecuteCommand, cpu->cop0.count + 3200));

                serialInterface.status.dmaBusy = 1;
                serialInterface.status.ioBusy = 1;
                return;
            }
            if (actualAddress >= 0x4000000 && actualAddress <= 0x4000FFF) {
                uint32_t offset = actualAddress - 0x4000000;

                Bus::writeWord(&rsp.dmem[offset], value);

                return;
            }
            if (actualAddress >= 0x4001000 && actualAddress <= 0x4001FFF) {

                uint32_t offset = actualAddress - 0x4001000;

                Bus::writeWord(&rsp.imem[offset], value);

                return;
            }

            std::cout << "(memWrite32) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

uint32_t Bus::readDataCache(uint64_t address) {
    uint32_t lineIndex = (address >> 4) & 0x1ff;

    if (!(dcacheHit(lineIndex, address))) {
        if (dcache[lineIndex].valid && dcache[lineIndex].dirty) {
            dcacheWriteback(lineIndex);
        }
        fillDataCache(lineIndex, address);
    } else {
        cpu->cop0.addCycles(1);
    }

    return dcache[lineIndex].words[(address >> 2) & 3];
}

void Bus::writeDataCache(uint64_t address, uint32_t value, int64_t mask) {
    uint32_t lineIndex = (address >> 4) & 0x1ff;

    if (!dcacheHit(lineIndex, address)) {
        if (dcache[lineIndex].valid && dcache[lineIndex].dirty) {
            dcacheWriteback(lineIndex);
        }
        fillDataCache(lineIndex, address);
    } else {
        cpu->cop0.addCycles(1);
    }

    uint32_t returnValue = value;
    uint32_t oldValue = dcache[lineIndex].words[(address >> 2) & 3];

    if (mask != -1) {
        returnValue = (oldValue & ~mask) | (value & mask);
    }

    dcache[lineIndex].words[(address >> 2) & 3] = returnValue;
    dcache[lineIndex].dirty = true;
}

void Bus::fillDataCache(uint32_t lineIndex, uint64_t address) {
    cpu->cop0.addCycles(7);

    dcache[lineIndex].valid = true;
    dcache[lineIndex].dirty = false;

    dcache[lineIndex].tag = (uint32_t)(address & ~0xfff);

    uint64_t cacheAddress = (uint64_t)((uint32_t)((dcache[lineIndex].tag) | (dcache[lineIndex].index)) & 0x1ffffffc);

    dcache[lineIndex].words[0] = memRead32(cacheAddress, true);
    dcache[lineIndex].words[1] = memRead32(cacheAddress + 4, true);
    dcache[lineIndex].words[2] = memRead32(cacheAddress + 8, true);
    dcache[lineIndex].words[3] = memRead32(cacheAddress + 12, true);
}

bool Bus::dcacheHit(uint32_t lineIndex, uint64_t address) {
    return dcache[lineIndex].valid && (dcache[lineIndex].tag & 0x1ffffffc) == (uint32_t)(address & ~0xfff);
}

void Bus::writeDoubleWord(uint8_t* ptr, uint64_t value) {
    for (int i = 7; i >= 0; i--) {
        int shift = 7 - i;
        uint8_t byte = (value >> shift * 8) & 0xff;
        ptr[i] = byte;
    }
}

void Bus::writeWord(uint8_t* ptr, uint32_t value) {
    for (int i = 3; i >= 0; i--) {
        int shift = 3 - i;
        uint8_t byte = (value >> shift * 8) & 0xff;
        ptr[i] = byte;
    }
}

void Bus::writeHalf(uint8_t* ptr, uint16_t value) {
    for (int i = 1; i >= 0; i--) {
        int shift = 1 - i;
        uint8_t byte = (value >> shift * 8) & 0xff;
        ptr[i] = byte;
    }
}

void Bus::dcacheWriteback(uint64_t line) {
    cpu->cop0.addCycles(36);
    dcache[line].dirty = false;

    uint64_t cacheAddress = (dcache[line].tag | dcache[line].index) & 0x1ffffffc;

    for (int i = 0; i < 4; i++) {
        uint64_t currAddress = cacheAddress | (i * 4);
        memWrite32(currAddress, dcache[line].words[i], true);
    }
}

uint64_t Bus::translateAddress(uint64_t address) {
    return address & 0x1FFFFFFF;
}

void Bus::tlbWrite(uint32_t index) {
    if (index > 31) {
        return;
    }

    tlbUnmap(index);

    tlbEntries[index].g = (uint8_t)(cpu->cop0.entryLo0 & cpu->cop0.entryLo1 & 1);

    tlbEntries[index].pfnEven = (cpu->cop0.entryLo0 >> 6) & 0xfffff;
    tlbEntries[index].pfnOdd = (cpu->cop0.entryLo1 >> 6) & 0xfffff;
    tlbEntries[index].cEven = (uint8_t)((cpu->cop0.entryLo0 >> 3) & 7);
    tlbEntries[index].cOdd = (uint8_t)((cpu->cop0.entryLo1 >> 3) & 7);
    tlbEntries[index].dEven = (uint8_t)((cpu->cop0.entryLo0 >> 2) & 1);
    tlbEntries[index].dOdd = (uint8_t)((cpu->cop0.entryLo1 >> 2) & 1);
    tlbEntries[index].vEven = (uint8_t)((cpu->cop0.entryLo0 >> 1) & 1);
    tlbEntries[index].vOdd = (uint8_t)((cpu->cop0.entryLo1 >> 1) & 1);

    tlbEntries[index].asid = (uint8_t)cpu->cop0.entryHi;

    tlbEntries[index].mask = ((cpu->cop0.pageMask >> 13) & 0xfff) & 0b101010101010;
    tlbEntries[index].vpn2 = (cpu->cop0.entryHi >> 13) & 0x7ffffff;

    tlbEntries[index].region = (uint8_t)(cpu->cop0.entryHi >> 62);

    tlbEntries[index].mask |= tlbEntries[index].mask >> 1;

    tlbEntries[index].vpn2 &= tlbEntries[index].mask;

    tlbEntries[index].startEven = (tlbEntries[index].vpn2 << 13) & 0xffffffff;
    tlbEntries[index].endEven = tlbEntries[index].startEven + (tlbEntries[index].mask << 12) + 0xfff;

    tlbEntries[index].physEven = tlbEntries[index].pfnEven << 12;

    tlbEntries[index].startOdd = tlbEntries[index].endEven + 1;
    tlbEntries[index].endOdd = tlbEntries[index].startOdd + (tlbEntries[index].mask << 12) + 0xfff;

    tlbEntries[index].physOdd = tlbEntries[index].pfnOdd << 12;

    tlbMap(index);
}

void Bus::tlbRead(uint32_t index) {
    if (index > 31) {
        return;
    }

    cpu->cop0.pageMask = tlbEntries[index].mask << 13;

    cpu->cop0.entryHi = (uint64_t)tlbEntries[index].region << 62 |
        (uint64_t)tlbEntries[index].vpn2 << 13 |
        (uint64_t)tlbEntries[index].asid;

    cpu->cop0.entryLo0 = (uint64_t)tlbEntries[index].pfnEven << 6 |
        (uint64_t)tlbEntries[index].cEven << 3 |
        (uint64_t)tlbEntries[index].dEven << 2 |
        (uint64_t)tlbEntries[index].vEven << 1 |
        (uint64_t)tlbEntries[index].g;

    cpu->cop0.entryLo1 = (uint64_t)tlbEntries[index].pfnOdd << 6 |
        (uint64_t)tlbEntries[index].cOdd << 3 |
        (uint64_t)tlbEntries[index].dOdd << 2 |
        (uint64_t)tlbEntries[index].vOdd << 1 |
        (uint64_t)tlbEntries[index].g;
}

void Bus::recalculateDelay() {
    double expectedRefreshRate = (double)videoInterface.clock / (double)(videoInterface.vTotal +1) / (double)((videoInterface.hTotal) + 1) * 2.0;

    videoInterface.delay = (uint32_t)((double)cpu->clock / expectedRefreshRate);
}

void Bus::tlbProbe() {
    uint64_t returnVal = 0x80000000;

    for (uint64_t i = 0; i < tlbEntries.size(); i++) {
        TlbEntry entry = tlbEntries[i];

        if ((entry.vpn2 & ~entry.mask) == (((cpu->cop0.entryHi >> 13) & 0x7ffffff) & ~entry.mask) &&
            entry.region == (uint8_t)(cpu->cop0.entryHi >> 62) &&
            (entry.g != 0 || entry.asid == (uint8_t)cpu->cop0.entryHi)
        ) {
            returnVal = (uint64_t)i;
            break;
        }
    }

    cpu->cop0.index = returnVal;
}

void Bus::tlbMap(uint32_t index) {
    TlbEntry entry = tlbEntries[index];

    if (entry.vEven != 0 &&
        entry.startEven < entry.endEven &&
        !(entry.startEven >= 0x80000000 && entry.endEven < 0xc0000000) &&
        entry.physEven < 0x20000000
    ) {
        for (int i = entry.startEven; i < entry.endEven; i += 0x1000) {
            tlbReadLut[i >> 12].address = 0x80000000 | (entry.physEven + (i - entry.startEven) + 0xfff);
            tlbReadLut[i >> 12].cached = entry.cEven != 2;
        }
    }

    if (entry.vOdd != 0 &&
        entry.startOdd < entry.endOdd &&
        !(entry.startOdd >= 0x80000000 && entry.endOdd < 0xc0000000) &&
        entry.physOdd < 0x20000000
    ) {
        for (int i = entry.startOdd; i < entry.endOdd; i += 0x1000) {
            tlbWriteLut[i >> 12].address = 0x80000000 | (entry.physOdd + (i - entry.startOdd) + 0xfff);
            tlbWriteLut[i >> 12].cached = entry.cOdd != 2;
        }
    }

}

void Bus::tlbUnmap(uint32_t index) {
    TlbEntry entry = tlbEntries[index];

    if (entry.vEven != 0) {
        for (int i = entry.startEven; i < entry.endEven; i += 0x1000) {
            tlbReadLut[i >> 12].address = 0;
            tlbReadLut[i >> 12].cached = false;
        }
    }

    if (entry.vOdd != 0) {
        for (int i = entry.startOdd; i < entry.endOdd; i += 0x1000) {
            tlbWriteLut[i >> 12].address = 0;
            tlbWriteLut[i >> 12].cached = false;
        }
    }
}

void Bus::setInterrupt(uint32_t flag) {
    mips.mipsInterrupt.value |= flag;

    if ((mips.mipsInterrupt.value & mips.mipsMask.value) != 0) {
        cpu->cop0.cause &= ~(0x1f << 2);
        cpu->cop0.cause |= 1 << 10;
    }
    cpu->checkIrqs();
}

void Bus::clearInterrupt(uint32_t flag) {
    mips.mipsInterrupt.value &= ~flag;

    if ((mips.mipsInterrupt.value & mips.mipsMask.value) == 0) {
        cpu->cop0.cause &= ~(1 << 10);
    }
    cpu->checkIrqs();
}

void Bus::dmaWrite() {
    uint32_t currDramAddr = peripheralInterface.dramAddress;
    uint32_t currCartAddr = peripheralInterface.cartAddress;
    uint32_t length = peripheralInterface.wrLen + 1;

    for (int i = 0; i < length; i++) {
        if (currCartAddr + i >= cartridge.size()) {
            rdram[currDramAddr + i] = 0;
        } else {
            rdram[currDramAddr + i] = cartridge[currCartAddr + i];
        }
    }

    peripheralInterface.dramAddress += length;
    peripheralInterface.cartAddress += length;

    // TODO: set dmaBusy, calculate cycles, and schedule dmaBusy = false
    // peripheralInterface.piStatus.dmaBusy = 1;
}

// TODO: move this to mips interface. currently having compile
// issues putting the code in there :/
void Bus::checkIrqs() {
    if ((mips.mipsMask.value & mips.mipsInterrupt.value) != 0) {
        cpu->cop0.cause &= ~(0x1f << 2);
        cpu->cop0.cause |= 1 << 10;
    } else {
        cpu->cop0.cause &= ~(1 << 10);
    }
}

void Bus::handleRspDma(SPDma dma) {
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
                uint32_t value = std::byteswap(*(uint32_t*)&rdram[dramAddress]);

                uint8_t* ramPtr = isImem ? &rsp.imem[memAddress] : &rsp.dmem[memAddress];

                writeWord(ramPtr, value);

                memAddress += 4;
                dramAddress += 4;
            }
            dramAddress += skip;
        }
    } else {
        for (int i = 0; i < count; i++) {
            for (int j = 0; j < length; j += 4) {
                uint8_t* ramPtr = isImem ? &rsp.imem[memAddress] : &rsp.dmem[memAddress];

                uint32_t value = std::byteswap(*(uint32_t*)ramPtr);

                writeWord(&rdram[dramAddress], value);

                memAddress += 4;
                dramAddress += 4;
            }
            dramAddress += skip;
        }
    }

    rsp.dmaMemAddress = memAddress;
    rsp.dmaRamAddress = dramAddress;
    rsp.spReadLength.value = 0xff8;
    rsp.spWriteLength.value = 0xff8;

    cpu->scheduler.addEvent(
        Event(
            RspDmaPop,
            cpu->cop0.count + calculateRdRamCycles(count * length) + 9
        )
    );
}

uint32_t Bus::calculateRdRamCycles(uint32_t length) {
    return 31 + (length / 3);
}

uint64_t Bus::runRsp() {
    return 0;
}

// tried to put this in RSP but due to needing to use the bus class
// it was causing a circular reference so i couldn't compile the code.
// TODO: try again at some point, although it seems like it's not letting
// me do this for a reason lol
void Bus::updateRspStatus(uint32_t value) {
    SPStatus* status = &rsp.status;

    bool previousHalt = status->halted;

    if ((value & 0b1) == 1 && ((value >> 1) & 0b1) == 0) {
        status->halted = 0;
    }
    if ((value & 0b1) == 0 & ((value >> 1) & 0b1) == 1) {
        status->halted = 1;
        // todo: remove from scheduler rsp pc event
    }
    if (((value >> 2) & 0b1) == 1) {
        status->broke = 0;
    }
    if (((value >> 3) & 0b1) == 1 && ((value >> 4) & 0b1) == 0) {
        clearInterrupt(SP_INTERRUPT_FLAG);
    }
    if (((value >> 3) & 0b1) == 0 && ((value >> 4) & 0b1) == 1) {
        setInterrupt(SP_INTERRUPT_FLAG);
    }
    if (((value >> 5) & 0b1) == 1 && ((value >> 6) & 0b1) == 0) {
        status->singleStep = 0;
    }
    if (((value >> 5) & 0b1) == 0 && ((value >> 6) & 0b1) == 1) {
        status->singleStep = 1;
    }
    if (((value >> 7) & 0b1) == 1 && ((value >> 8) & 0b1) == 0) {
        status->intBreak = 0;
    }
    if (((value >> 7) & 0b1) == 0 && ((value >> 8) & 0b1) == 1) {
        status->intBreak = 1;
    }
    if (((value >> 9) & 0b1) == 1 && ((value >> 10) & 0b1) == 0) {
        status->flag0 = 0;
    }
    if (((value >> 9) & 0b1) == 0 && ((value >> 10) & 0b1) == 1) {
        status->flag0 = 1;
    }
    if (((value >> 11) & 0b1) == 1 && ((value >> 12) & 0b1) == 0) {
        status->flag1 = 0;
    }
    if (((value >> 11) & 0b1) == 0 && ((value >> 12) & 0b1) == 1) {
        status->flag1 = 1;
    }
    if (((value >> 13) & 0b1) == 1 && ((value >> 14) & 0b1) == 0) {
        status->flag2 = 0;
    }
    if (((value >> 13) & 0b1) == 0 && ((value >> 14) & 0b1) == 1) {
        status->flag2 = 1;
    }
    if (((value >> 15) & 0b1) == 1 && ((value >> 16) & 0b1) == 0) {
        status->flag3 = 0;
    }
    if (((value >> 15) & 0b1) == 0 && ((value >> 16) & 0b1) == 1) {
        status->flag3 = 1;
    }
    if (((value >> 17) & 0b1) == 1 && ((value >> 18) & 0b1) == 0) {
        status->flag4 = 0;
    }
    if (((value >> 17) & 0b1) == 0 && ((value >> 18) & 0b1) == 1) {
        status->flag4 = 1;
    }
    if (((value >> 19) & 0b1) == 1 && ((value >> 20) & 0b1) == 0) {
        status->flag5 = 0;
    }
    if (((value >> 19) & 0b1) == 0 && ((value >> 20) & 0b1) == 1) {
        status->flag5 = 1;
    }
    if (((value >> 21) & 0b1) == 1 && ((value >> 22) & 0b1) == 0) {
        status->flag6 = 0;
    }
    if (((value >> 21) & 0b1) == 0 && ((value >> 22) & 0b1) == 1) {
        status->flag6 = 1;
    }
    if (((value >> 23) & 0b1) == 1 && ((value >> 24) & 0b1) == 0) {
        status->flag7 = 0;
    }
    if (((value >> 23) & 0b1) == 0 && ((value >> 24) & 0b1) == 1) {
        status->flag7 = 1;
    }

    if (!status->halted && previousHalt) {
       runRsp();
    }
}