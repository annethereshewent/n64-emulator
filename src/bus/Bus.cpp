#pragma once

#include "Bus.hpp"
#include <iostream>
#include <bit>
#include "PIF.cpp"

Bus::Bus() {
    rdram.resize(0x800000);
    rdram9.resize(0x800000);
    spdmem.resize(0x1000);

    rsp.spStatus.value = 1;
}

uint32_t Bus::memRead32(uint64_t address) {
    uint64_t actualAddress = Bus::translateAddress(address);

    switch (actualAddress) {
        case 0x4040010:
            return rsp.spStatus.value;
            break;
        case 0x4040018:
            return rsp.spStatus.dmaBusy;
            break;
        case 0x410000c:
            return rdp.status.value;
            break;
        case 0x4800018:
            return serialInterface.status.value;
            break;
        default:
            if (actualAddress >= 0x10000000 && actualAddress <= 0x1FBFFFFF) {
                uint32_t offset = actualAddress - 0x10000000;



                return std::byteswap(*(uint32_t*)&cartridge[offset]);
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                uint32_t offset = actualAddress - 0x1fc007c0;

                return std::byteswap(*(uint32_t*)&pif.ram[offset]);
            }
            if (actualAddress >= 0x1FC00000 && actualAddress <= 0x1FC007BF) {

                uint64_t pifAddress = actualAddress - 0x1FC00000;

                return std::byteswap(*(uint32_t*)&PIF_BOOT_ROM[pifAddress]);
            }
            if (actualAddress >= 0x4000000 && actualAddress <= 0x4000FFF) {
                uint64_t offset = actualAddress - 0x4000000;

                return std::byteswap(*(uint32_t*)&rsp.dmem[offset]);
            }
            if (actualAddress >= 0x4001000 && actualAddress <= 0x4001FFF) {
                uint64_t offset = actualAddress - 0x4001000;

                return std::byteswap(*(uint32_t*)&rsp.imem[offset]);
            }


            exit(1);
            break;
    }
}

void Bus::memWrite32(uint64_t address, uint32_t value) {
    uint64_t actualAddress = Bus::translateAddress(address);



    switch (actualAddress) {
        case 0x4040010:
            rsp.spStatus.value = value;
            break;
        case 0x410000c:
            rdp.status.value = value;
            break;
        case 0x4400010:
            // TODO: clear interrupt
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
        case 0x4600010:
            peripheralInterface.piStatus.value = value & 0xf;
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
        default:
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                uint32_t offset = actualAddress - 0x1fc007c0;

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

uint64_t Bus::translateAddress(uint64_t address) {
    // if (address >= 0x80000000 && address <= 0x9FFFFFFF) {
    //     return address - 0x80000000;
    // }
    // if (address >= 0xA0000000 && address <= 0xBFFFFFFF) {
    //     return address - 0xA0000000;
    // }

    return address & 0x1FFFFFFF;
}