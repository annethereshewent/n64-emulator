#pragma once

#include "Bus.hpp"
#include <iostream>
#include <bit>
#include "pif/PIF.cpp"

Bus::Bus() {
    rdram.resize(0x800000);
    rdram9.resize(0x800000);
    spdmem.resize(0x1000);

    rsp.status.value = 1;
}

uint8_t Bus::memRead8(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);

    switch (actualAddress) {
        default:
            if (actualAddress >= 0x10000000 && actualAddress <= 0x1FBFFFFF) {
                uint64_t offset = actualAddress - 0x10000000;

                return cartridge[offset] & 0xff;
            }
            std::cout << "(memRead8) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

uint16_t Bus::memRead16(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);

    switch (actualAddress) {
        default:
            std::cout << "(memRead16) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

uint32_t Bus::memRead32(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);

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
        case 0x430000c:
            return mips.miMask;
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

void Bus::memWrite32(uint64_t address, uint32_t value) {
    uint64_t actualAddress = Bus::translateAddress(address);

    switch (actualAddress) {
        case 0x4400010:
            // TODO: clear interrupt
            break;
        case 0x4040010:
            rsp.status.value = value;
            break;
        case 0x410000c:
            rdp.status.value = value;
            break;
        case 0x4300000:
            mips.write(value);
             // TODO: clear dp interrupt
            break;
        case 0x430000c:
            mips.miMask = value & 0xfff;
            // TODO: check interrupts
            break;
        case 0x4400024:
            videoInterface.hVideo = value & 0x3ff;
            break;
        case 0x440000c:
            videoInterface.vInterrupt = value & 0x3ff;
            break;
        case 0x4500000:
            audioInterface.dramAddress = value & 0xffffff;
            break;
        case 0x4500004:
            audioInterface.audioLength = value & 0x3ffff;
            break;
        case 0x450000c:
            // TODO: acknowledge interrupt here
            std::cout << "it should acknowledge an audio interrupt here.\n";
            break;
        case 0x4600000:
            peripheralInterface.dramAddress = value & 0xfffffe;
            break;
        case 0x4600004:
            peripheralInterface.cartAddress = value & 0xfffffe;
            break;
        case 0x460000c:
            peripheralInterface.wrLen = value & 0xffffff;
            // TODO: start dma write transfer
            dmaWrite();
            break;
        case 0x4600010:
            // TODO: clear interrupt and reset dma controller from doing transfer
            std::cout << "it should clear interrupts and reset dma controller\n";
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
            serialInterface.status.value = value & 0x1fff;
            break;
        default:
            if (actualAddress <= 0x03EFFFFF) {
                Bus::writeWord(&rdram[0], actualAddress, value);

                return;
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                uint32_t offset = actualAddress - 0x1fc007c0;

                std::cout << "writing to pif ram value " << std::hex << value << " at address " << actualAddress << "\n";

                Bus::writeWord(&pif.ram[0], offset, value);

                // TODO: add scheduler and schedule this for later
                pif.executeCommand();

                return;
            }
            if (actualAddress >= 0x4000000 && actualAddress <= 0x4000FFF) {
                uint32_t offset = actualAddress - 0x4000000;

                Bus::writeWord(&rsp.dmem[0], offset, value);

                return;
            }
            if (actualAddress >= 0x4001000 && actualAddress <= 0x4001FFF) {

                uint32_t offset = actualAddress - 0x4001000;

                Bus::writeWord(&rsp.imem[0], offset, value);

                return;
            }

            std::cout << "(memWrite32) unsupported address received: " << std::hex << actualAddress << "\n";
            exit(1);
            break;
    }
}

void Bus::writeWord(uint8_t* ptr, uint32_t address, uint32_t value) {
    for (int i = 3; i >= 0; i--) {
        int shift = 3 - i;
        uint8_t byte = (value >> shift * 8) & 0xff;
        ptr[address + i] = byte;
    }
}

void Bus::dcacheWriteback(uint64_t line) {
    dcache[line].dirty = false;

    uint64_t cacheAddress = (dcache[line].tag | dcache[line].index) & 0x1ffffffc;

    for (int i = 0; i < 4; i++) {
        uint64_t currAddress = cacheAddress | (i * 4);
        memWrite32(currAddress, dcache[line].words[i]);
    }
}

uint64_t Bus::translateAddress(uint64_t address) {
    return address & 0x1FFFFFFF;
}

void Bus::dmaWrite() {
    uint32_t currDramAddr = peripheralInterface.dramAddress;
    uint32_t currCartAddr = peripheralInterface.cartAddress;
    uint32_t length = peripheralInterface.wrLen + 1;

    std::cout << "currDramAddr = " << std::hex << currDramAddr << ", length = " << length << "\n";
    std::cout << "curCartAddr = " << std::hex << currCartAddr << "\n";

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