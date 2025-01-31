#pragma once

#include "Bus.hpp"
#include <iostream>
#include <bit>
#include "pif/PIF.cpp"
#include "../cpu/CPU.hpp"

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

                return std::byteswap(*(uint32_t*)&cartridge[offset]);
            }
            if (actualAddress >= 0x1FC00000 && actualAddress <= 0x1FC007BF) {

                uint64_t pifAddress = actualAddress - 0x1FC00000;

                return std::byteswap(*(uint32_t*)&PIF_BOOT_ROM[pifAddress]);
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
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
        case 0x4040010:
            rsp.status.value = value;
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
            videoInterface.vTotal = value & 0x3ff;
            break;
        case 0x440001c:
            videoInterface.hTotal = value & 0xfff;
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
            // TODO: schedule dma write transfer
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
                Bus::writeWord(&rdram[actualAddress], value);

                return;
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                uint32_t offset = actualAddress - 0x1fc007c0;

                Bus::writeWord(&pif.ram[offset], value);

                // TODO: add scheduler and schedule this for later
                pif.executeCommand();

                serialInterface.status.dmaBusy = 0;
                serialInterface.status.ioBusy = 0;
                serialInterface.status.interrupt = 1;

                setInterrupt(SI_INTERRUPT_FLAG);
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
        // TODO: add cycles here
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
        // TODO: add cycles
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
    // TODO: add cycles

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
    // TODO: add cycles

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