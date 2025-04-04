#pragma once

#include "Bus.hpp"
#include <iostream>
#include <bit>
#include <regex>
#include <CommonCrypto/CommonDigest.h>
#include "pif/PIF.cpp"
#include "../cpu/CPU.hpp"
#include "rsp/RSP.cpp"
#include "mips_interface/MIPSInterface.cpp"
#include "peripheral_interface/PeripheralInterface.cpp"
#include "serial_interface/SerialInterface.cpp"
#include "rdp/RDPInterface.cpp"
#include "audio_interface/AudioInterface.cpp"
#include "video_interface/VideoInterface.cpp"
#include "../interface.cpp"

const uint32_t TLBS = 12;
const uint32_t TLBL = 8;

uint8_t Bus::memRead8(uint64_t actualAddress, bool cached) {
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

    if (actualAddress <= 0x03EFFFFF) {
        switch (actualAddress & 0x3) {
            case 0:
                return rdram[actualAddress + 3];
                break;
            case 1:
                return rdram[(actualAddress & ~0x3) + 2];
                break;
            case 2:
                return rdram[(actualAddress & ~0x3) + 1];
                break;
            case 3:
                return rdram[actualAddress & ~0x3];
                break;
        }
    }
    if (actualAddress >= 0x10000000 && actualAddress <= 0x1FBFFFFF) {
        uint64_t offset = actualAddress - 0x10000000;

        if (offset < cartridge.size()) {
            return cartridge[offset];
        }

        throw std::runtime_error("invalid cartridge offset given!");
    }

    if (actualAddress >= 0x4000000 && actualAddress <= 0x4ffffff) {
        uint32_t value = memRead32(actualAddress, cached);

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

    std::println("(memRead8) unsupported address received: {:x}", actualAddress);
    throw std::runtime_error("");

}

uint16_t Bus::memRead16(uint64_t actualAddress, bool cached) {
    if (cached) {
        uint32_t value = readDataCache(actualAddress & ~0x3);

        if ((actualAddress & 0x3) == 0) {
            return (uint16_t)(value >> 16);
        }

        return (uint16_t)value;
    }

    if (actualAddress <= 0x03EFFFFF) {
        if ((actualAddress & 0x3) == 0) {
            return *(uint16_t*)&rdram[actualAddress + 2];
        } else {
            return *(uint16_t*)&rdram[actualAddress & ~0x3];
        }
    }
    std::cout << "(memRead16) unsupported address received: " << std::hex << actualAddress << "\n";
    throw std::runtime_error("");

}

void Bus::memWrite8(uint64_t actualAddress, uint8_t value, bool cached) {
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
                switch (actualAddress & 0x3) {
                    case 0:
                        rdram[actualAddress + 3] = value;
                        break;
                    case 1:
                        rdram[(actualAddress & ~0x3) + 2] = value;
                        break;
                    case 2:
                        rdram[(actualAddress & ~0x3) + 1] = value;
                        break;
                    case 3:
                        rdram[actualAddress & ~0x3] = value;
                        break;
                }

                return;
            }

            std::cout << "(memWrite8) unsupported address given: " << std::hex << actualAddress << "\n";
            throw std::runtime_error("");
            break;
    }
}

void Bus::memWrite16(uint64_t actualAddress, uint16_t value, bool cached) {
    if (cached) {
        if ((actualAddress & 0x3) == 0) {
            writeDataCache(actualAddress & ~0x3, (uint32_t)value << 16, 0xffff0000);
        } else {
            writeDataCache(actualAddress & ~0x3, (uint32_t)value, 0x0000ffff);
        }

        return;
    }

    switch (actualAddress) {
        default:
            if (actualAddress <= 0x03EFFFFF) {
                // due to little endian/big endian conversions, need to something like this.
                if ((actualAddress & 0x3) == 0) {
                    memcpy(&rdram[actualAddress + 2], &value, sizeof(uint16_t));
                } else {
                    memcpy(&rdram[actualAddress & ~0x3], &value, sizeof(uint16_t));
                }

                return;
            }

            std::cout << "(memWrite16) unsupported address given: " << std::hex << actualAddress << "\n";
            throw std::runtime_error("");
            break;
    }
}

void Bus::memWrite64(uint64_t actualAddress, uint64_t value, bool cached, uint64_t mask) {

    if (cached) {
        writeDataCache(actualAddress + 4, (uint32_t)value, (uint32_t)(int64_t)mask);
        writeDataCache(actualAddress, (uint32_t)(value >> 32), (uint32_t)(int64_t)(mask >> 32));
        return;
    }

    if (actualAddress <= 0x03EFFFFF) {
        memcpy(&rdram[actualAddress], &value, sizeof(uint64_t));
        return;
    }
}

uint64_t Bus::memRead64(uint64_t actualAddress, bool cached) {
    if (cached) {
        return ((uint64_t)readDataCache(actualAddress) << 32) | (uint64_t)readDataCache(actualAddress + 4);
    }

    if (actualAddress <= 0x03EFFFFF) {
        throw std::runtime_error("TODO: memRead64");
        return *(uint64_t*)&rdram[actualAddress];
    }

    std::cout << "(memRead64) unsupported address given: " << std::hex << actualAddress << "\n";
    throw std::runtime_error("");

}
uint32_t Bus::memRead32(uint64_t actualAddress, bool cached, Width bitWidth, bool ignoreCycles) {
    if (cached && bitWidth != WidthDCache && bitWidth != WidthICache) {
        return readDataCache(actualAddress, ignoreCycles);
    }

    switch (actualAddress) {
        case 0x4040010:
            cpu.cop0.addCycles(20);
            return rsp.status.value;
            break;
        case 0x4040014:
            cpu.cop0.addCycles(20);
            return rsp.status.dmaFull;
            break;
        case 0x4040018:
            cpu.cop0.addCycles(20);
            return rsp.status.dmaBusy;
            break;
        case 0x4080000:
            cpu.cop0.addCycles(20);
            return rsp.pc;
            break;
        case 0x410000c:
            cpu.cop0.addCycles(20);
            return rdp.status.value;
            break;
        case 0x4100010:
            cpu.cop0.addCycles(20);
            return 0xffffff;
            break;
        case 0x4100014:
            cpu.cop0.addCycles(20);
            // TODO
            return 0;
            break;
        case 0x4100018:
            cpu.cop0.addCycles(20);
            return rdp.pipeBusy;
            break;
        case 0x410001c:
            cpu.cop0.addCycles(20);
            return 0;
            break;
        case 0x4300004:
            cpu.cop0.addCycles(20);
            return mips.mipsVersion;
            break;
        case 0x4300008:
            cpu.cop0.addCycles(20);
            return mips.mipsInterrupt.value;
            break;
        case 0x430000c:
            cpu.cop0.addCycles(20);
            return mips.mipsMask.value;
            break;
        case 0x4400010:
            cpu.cop0.addCycles(20);
            videoInterface.updateVCurrent(cpu.scheduler, cpu.cop0.count);
            return videoInterface.vcurrent.value;
            break;
        case 0x4500004:
            cpu.cop0.addCycles(20);
            return audioInterface.audioLength;
            break;
        case 0x450000c:
            cpu.cop0.addCycles(20);
            return audioInterface.status.value;
            break;
        case 0x4600010:
            cpu.cop0.addCycles(20);
            return peripheralInterface.piStatus.value;
            break;
        case 0x4600014:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom1Latch;
            break;
        case 0x4600018:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom1Pwd;
            break;
        case 0x460001c:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom1Pgs;
            break;
        case 0x4600020:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom1Rls;
            break;
        case 0x4600024:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom2Latch;
            break;
        case 0x4600028:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom2Pwd;
            break;
        case 0x460002c:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom2Pgs;
            break;
        case 0x4600030:
            cpu.cop0.addCycles(20);
            return peripheralInterface.dom2Rls;
            break;
        case 0x470000c:
            // just return 0x14 to skip the initialization process
            // TODO: actually implement rdInterface related stuff
            cpu.cop0.addCycles(20);
            if (!rdInterface.init) {
                // simulate initialization of rdInterface
                cpu.cop0.addCycles(cpu.clock / 2);
                rdInterface.init = true;
            }
            return 0x14;
            // return rdInterface.select.value;
            break;
        case 0x4800000:
            cpu.cop0.addCycles(20);
            return serialInterface.dramAddress;
            break;
        case 0x4800018:
            cpu.cop0.addCycles(20);
            return serialInterface.status.value;
            break;
        default:
            if (actualAddress <= 0x03EFFFFF) {
                uint32_t cycles = calculateRdRamCycles(bitWidth) / (bitWidth / 4);

                if (!ignoreCycles) {
                    cpu.cop0.addCycles(cycles);
                }

                return *(uint32_t*)&rdram[actualAddress];
            }
            if (actualAddress >= 0x08000000 && actualAddress <= 0x0801ffff) {
                // cartridge sram
                // TODO: implement saves
                if (actualAddress >= 0x8000000 && actualAddress <= 0x801ffff) {
                    if (std::find(saveTypes.begin(), saveTypes.end(), Sram) != saveTypes.end()) {
                        uint32_t offset = actualAddress - 0x8000000;

                        formatSram();

                        return std::byteswap(*(uint32_t*)&sram[offset]);
                    } else {
                        if ((actualAddress & 0x1ffff) == 0 && flashMode == Status) {
                            return flashStatus;
                        }
                        if ((actualAddress & 0x1ffff) == 0 && flashMode == ReadArray) {
                            return 0;
                        }
                        throw std::runtime_error("invalid option given to flash read");
                    }
                }
            }
            if (actualAddress >= 0x4000000 && actualAddress <= 0x4000FFF) {
                uint64_t offset = actualAddress - 0x4000000;

                if (!ignoreCycles) {
                    cpu.cop0.addCycles(bitWidth / 4);
                }

                return std::byteswap(*(uint32_t*)&rsp.dmem[offset]);
            }
            if (actualAddress >= 0x4001000 && actualAddress <= 0x4001FFF) {
                uint64_t offset = actualAddress - 0x4001000;

                if (!ignoreCycles) {
                    cpu.cop0.addCycles(bitWidth / 4);
                }

                return std::byteswap(*(uint32_t*)&rsp.imem[offset]);
            }
            if (actualAddress >= 0x5000000 && actualAddress <= 0x07ffffff) {
                // N64 DD registers, ignore
                return actualAddress & 0xffff | ((actualAddress & 0xffff) << 16);
            }
            if (actualAddress >= 0x10000000 && actualAddress <= 0x1FBFFFFF) {
                uint32_t offset = actualAddress & 0xfffffff;

                uint32_t cycles = peripheralInterface.calculateCycles(1, 4);

                if (!ignoreCycles) {
                    cpu.cop0.addCycles(cycles);
                }

                if (offset < cartridge.size()) {
                    return std::byteswap(*(uint32_t*)&cartridge[offset]);
                }

                std::cout << "invalid offset given to cartridge: " << std::hex << offset << "\n";
                throw std::runtime_error("");
            }
            if (actualAddress >= 0x1FC00000 && actualAddress <= 0x1FC007BF) {
                if (!ignoreCycles) {
                    cpu.cop0.addCycles(3000);
                }
                uint64_t pifAddress = actualAddress - 0x1FC00000;

                return std::byteswap(*(uint32_t*)&pif.rom[pifAddress]);
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                if (!ignoreCycles) {
                    cpu.cop0.addCycles(3000);
                }
                uint32_t offset = actualAddress - 0x1fc007c0;

                return std::byteswap(*(uint32_t*)&pif.ram[offset]);
            }
            if (actualAddress >= 0x1fd00000 && actualAddress <= 0x1fffffff) {
                return actualAddress & 0xffff | ((actualAddress & 0xffff) << 16);
            }

            std::cout << "(memRead32) unsupported address received: " << std::hex << actualAddress << "\n";
            throw std::runtime_error("");
            break;
    }
}

void Bus::memWrite32(uint64_t actualAddress, uint32_t value, bool cached, bool ignoreCache, int64_t mask) {
    if (cached && !ignoreCache) {
        writeDataCache(actualAddress, value, mask);
        return;
    }

    switch (actualAddress) {
        case 0x4040000:
            Bus::writeWithMask32(&rsp.dmaMemAddress, value & 0x1ffc, mask);
            break;
        case 0x4040004:
            Bus::writeWithMask32(&rsp.dmaRamAddress, value & 0xfffffc, mask);
            break;
        case 0x4040008:
            Bus::writeWithMask32(&rsp.spReadLength.value, value, mask);
            rsp.pushDma(DmaDirection::Read);
            break;
        case 0x404000c:
            Bus::writeWithMask32(&rsp.spWriteLength.value, value, mask);
            rsp.pushDma(DmaDirection::Write);
            break;
        case 0x4040010:
            rsp.updateStatus(value);
            break;
        case 0x404001c:
            rsp.semaphore = 0;
            break;
        case 0x4080000:
            Bus::writeWithMask32(&rsp.pc, value & 0xffc, mask);
            rsp.nextPc = rsp.pc + 4;
            break;
        case 0x410000c:
            Bus::writeWithMask32(&rdp.status.value, value, mask);
            break;
        case 0x4300000:
            mips.write(value);
            if ((value >> 11 & 0b1) == 1) {
                clearInterrupt(DP_INTERRUPT_FLAG);
            }
            checkIrqs();
            cpu.checkIrqs();
            break;
        case 0x4300008:
            Bus::writeWithMask32(&mips.mipsInterrupt.value, value, mask);
            checkIrqs();
            cpu.checkIrqs();
            break;
        case 0x430000c:
            mips.setMask(value);
            checkIrqs();
            cpu.checkIrqs();
            break;
        case 0x4400000:
            Bus::writeWithMask32(&videoInterface.ctrl.value, value, mask);
            videoInterface.ctrl.unused = 0;

            rdp_set_vi_register(0, videoInterface.ctrl.value);
            break;
        case 0x4400004:
            Bus::writeWithMask32(&videoInterface.origin, value, mask);
            rdp_set_vi_register(1, videoInterface.origin);
            break;
        case 0x4400008:
            Bus::writeWithMask32(&videoInterface.width, value, mask);
            rdp_set_vi_register(2, videoInterface.width);
            break;
        case 0x440000c:
            Bus::writeWithMask32(&videoInterface.vInterrupt, value, mask);
            rdp_set_vi_register(3, videoInterface.vInterrupt);
            break;
        case 0x4400010:
            rdp_set_vi_register(4, value);
            clearInterrupt(VI_INTERRUPT_FLAG);
            break;
        case 0x4400014:
            Bus::writeWithMask32(&videoInterface.viBurst, value, mask);
            rdp_set_vi_register(5, videoInterface.viBurst);
            break;
        case 0x4400018:
            if (videoInterface.vTotal != value) {
                Bus::writeWithMask32(&videoInterface.vTotal, value, mask);


                recalculateDelay();

                if (!videoInterface.interruptStarted) {
                    videoInterface.interruptStarted = true;

                    cpu.scheduler.addEvent(Event(VideoInterrupt, cpu.cop0.count + videoInterface.delay));
                }
            }
            rdp_set_vi_register(6, videoInterface.vTotal);
            break;
        case 0x440001c:
            if (videoInterface.hTotal != value) {
                Bus::writeWithMask32(&videoInterface.hTotal, value, mask);

                recalculateDelay();
            }
            rdp_set_vi_register(7, videoInterface.hTotal);
            break;
        case 0x4400020:
            Bus::writeWithMask32(&videoInterface.hTotalLeap.value, value, mask);
            videoInterface.hTotalLeap.unused = 0;
            rdp_set_vi_register(0x20 / 4, videoInterface.hTotalLeap.value);
            break;
        case 0x4400024:
            Bus::writeWithMask32(&videoInterface.hVideo, value, mask);
            rdp_set_vi_register(0x24 / 4, videoInterface.hVideo);
            break;
        case 0x4400028:
            Bus::writeWithMask32(&videoInterface.vVideo.value, value, mask);
            videoInterface.vVideo.unused = 0;
            rdp_set_vi_register(0x28 / 4, videoInterface.vVideo.value);
            break;
        case 0x440002c:
            Bus::writeWithMask32(&videoInterface.vBurst, value, mask);
            rdp_set_vi_register(0x2c / 4, videoInterface.vBurst);
            break;
        case 0x4400030:
            Bus::writeWithMask32(&videoInterface.xScale.value, value, mask);
            videoInterface.xScale.unused = 0;
            rdp_set_vi_register(0x30 / 4, videoInterface.xScale.value);
            break;
        case 0x4400034:
            Bus::writeWithMask32(&videoInterface.yScale.value, value, mask);
            videoInterface.yScale.unused = 0;
            rdp_set_vi_register(0x34 / 4, videoInterface.yScale.value);
            break;
        case 0x4500000:
            Bus::writeWithMask32(&audioInterface.dramAddress, value & 0xffffff, mask);
            break;
        case 0x4500004:
            Bus::writeWithMask32(&audioInterface.audioLength, value & 0x3ffff, mask);

            // start audio DMA
            audioInterface.pushDma();
            break;
        case 0x4500008:
            audioInterface.dmaEnable = (value & 0b1) == 1;
            break;
        case 0x450000c:
            clearInterrupt(AI_INTERRUPT_FLAG);
            break;
        case 0x4500010:
            if (audioInterface.dacRate != (value & mask)){
                audioInterface.frequency = videoInterface.clock / (1 + (value & mask));

                // reopen stream with new frequency
                restartAudio();
            }
            Bus::writeWithMask32(&audioInterface.dacRate, value & 0x3fff, mask);
            break;
        case 0x4500014:
            Bus::writeWithMask32(&audioInterface.bitRate, value & 0xf, mask);
            break;
        case 0x4600000:
            Bus::writeWithMask32(&peripheralInterface.dramAddress, value & 0xfffffe, mask);
            break;
        case 0x4600004:
            Bus::writeWithMask32(&peripheralInterface.cartAddress, value, mask);
            break;
        case 0x4600008: {
            Bus::writeWithMask32(&peripheralInterface.rdLen, value & 0xffffff, mask);
            uint32_t cartAddress = peripheralInterface.cartAddress;

            if (cartAddress >= 0x1ffe0000 && cartAddress <= 0x1ffe1fff) {
                throw std::runtime_error("todo: sc64 dma");
            } else if (cartAddress >= 0x10000000 && cartAddress <= 0x1fbfffff) {
                throw std::runtime_error("TODO: dmaCartRead");
                // dmaCartRead();
            } else if (cartAddress >= 0x8000000 && cartAddress <= 0xfffffff) {
                if (std::find(saveTypes.begin(), saveTypes.end(), Sram) != saveTypes.end()) {
                    dmaSramRead();
                } else if (std::find(saveTypes.begin(), saveTypes.end(), Flash) != saveTypes.end()) {
                    dmaFlashRead();
                } else {
                    std::println("invalid address given for PI dma write: {:x}", cartAddress);
                    throw std::runtime_error("");
                }
            } else {
                uint64_t cycles = peripheralInterface.calculateCycles(1, peripheralInterface.rdLen + 1);

                cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
            }

            break;
        }
        case 0x460000c: {
            Bus::writeWithMask32(&peripheralInterface.wrLen, value & 0xffffff, mask);
            uint32_t cartAddress = peripheralInterface.cartAddress;

            if (cartAddress >= 0x1ffe0000 && cartAddress <= 0x1ffe1fff) {
                throw std::runtime_error("todo: sc64 dma");
            } else if (cartAddress >= 0x10000000 && cartAddress <= 0x1fbfffff) {
                dmaCartWrite();
            } else if (cartAddress >= 0x8000000 && cartAddress <= 0xfffffff) {
                if (std::find(saveTypes.begin(), saveTypes.end(), Sram) != saveTypes.end()) {
                    dmaSramWrite();
                } else if (std::find(saveTypes.begin(), saveTypes.end(), Flash) != saveTypes.end()) {
                    dmaFlashWrite();
                } else {
                    std::println("invalid address given for PI dma write: {:x}", cartAddress);
                    throw std::runtime_error("");
                }
            } else {
                uint32_t length = peripheralInterface.wrLen + 1;
                uint32_t dramAddr = peripheralInterface.dramAddress;

                for (uint32_t i = 0; i < length; i++) {
                    rdram[(dramAddr + i) ^ 3] = 0;
                }

                uint64_t cycles = peripheralInterface.calculateCycles(1, length);

                cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
            }
            break;
        }
        case 0x4600010:
            if ((value & 0b1) == 1) {
                peripheralInterface.piStatus.value = 0;
            }
            if (((value >> 1) & 0b1) == 1) {
                clearInterrupt(PI_INTERRUPT_FLAG);
                peripheralInterface.piStatus.dmaCompleted = 0;
            }
            break;
        case 0x4600014:
            Bus::writeWithMask32(&peripheralInterface.dom1Latch, value & 0xff, mask);
            break;
        case 0x4600018:
            Bus::writeWithMask32(&peripheralInterface.dom1Pwd, value & 0xff, mask);
            break;
        case 0x460001c:
            Bus::writeWithMask32(&peripheralInterface.dom1Pgs, value & 0xff, mask);
            break;
        case 0x4600020:
            Bus::writeWithMask32(&peripheralInterface.dom1Rls, value & 0x3, mask);
            break;
        case 0x4600024:
            Bus::writeWithMask32(&peripheralInterface.dom2Latch, value & 0xff, mask);
            break;
        case 0x4600028:
            Bus::writeWithMask32(&peripheralInterface.dom2Pwd, value & 0xff, mask);
            break;
        case 0x460002c:
            Bus::writeWithMask32(&peripheralInterface.dom2Pgs, value & 0xf, mask);
            break;
        case 0x4600030:
            Bus::writeWithMask32(&peripheralInterface.dom2Rls, value & 0x3, mask);
            break;
        case 0x4700000:
            Bus::writeWithMask32(&rdInterface.mode.value, value & 0xf, mask);
            break;
        case 0x4700004:
            Bus::writeWithMask32(&rdInterface.config.value, value & 0x7f, mask);
            break;
        case 0x4700008:
            Bus::writeWithMask32(&rdInterface.currentLoad, value, mask);
            break;
        case 0x470000C:
            Bus::writeWithMask32(&rdInterface.select.value, value & 0xff, mask);
            break;
        case 0x4800000:
            Bus::writeWithMask32(&serialInterface.dramAddress, value & 0xffffff, mask);
            break;
        case 0x4800004: {
            serialInterface.dir = DmaDirection::Read;

            serialInterface.status.dmaBusy = 1;

            uint64_t cycles = serialInterface.processRam(*this);
            cpu.scheduler.addEvent(Event(SIDma, cpu.cop0.count + cycles));
            break;
        }
        case 0x4800010:
            serialInterface.dir = DmaDirection::Write;

            serialInterface.handleDma(*this);

            serialInterface.status.dmaBusy = 1;

            cpu.scheduler.addEvent(Event(SIDma, cpu.cop0.count + 6000));
            break;
        case 0x4800018:
            serialInterface.status.interrupt = 0;
            clearInterrupt(SI_INTERRUPT_FLAG);
            break;
        default:
            if (actualAddress <= 0x03EFFFFF) {
                uint32_t returnVal = *(uint32_t*)&rdram[actualAddress];

                Bus::writeWithMask32(&returnVal, value, mask);

                memcpy(&rdram[actualAddress], &returnVal, sizeof(uint32_t));

                return;
            }
            if (actualAddress >= 0x13ff0000 && actualAddress <= 0x13ffffff) {
                uint32_t offset = actualAddress & 0xffff;

                if (offset == 0x14) {
                    uint32_t length = mask != -1 ? value & mask : value;

                    char result[length + 1];

                    memcpy(&result, &consoleBuffer[0x20], length * sizeof(uint8_t));
                    result[length] = 0;

                    std::string output = result;

                    std::print("{}", output);
                } else {
                    uint32_t returnVal = std::byteswap(*(uint32_t*)&consoleBuffer[offset]);

                    Bus::writeWithMask32(&returnVal, value, mask);

                    returnVal = std::byteswap(returnVal);

                    memcpy(&consoleBuffer[offset], &returnVal, sizeof(uint32_t));
                }

                return;
            }
            if (actualAddress >= 0x1FC007C0 && actualAddress <= 0x1FC007FF) {
                uint32_t offset = actualAddress - 0x1fc007c0;

                uint32_t returnVal = std::byteswap(*(uint32_t*)&pif.ram[offset]);

                Bus::writeWithMask32(&returnVal, value, mask);

                returnVal = std::byteswap(returnVal);

                memcpy(&pif.ram[offset], &returnVal, sizeof(uint32_t));

                cpu.scheduler.addEvent(Event(PIFExecuteCommand, cpu.cop0.count + 3200));

                serialInterface.status.dmaBusy = 1;
                serialInterface.status.ioBusy = 1;
                return;
            }
            if (actualAddress >= 0x4000000 && actualAddress <= 0x4000FFF) {
                uint32_t offset = actualAddress - 0x4000000;

                uint32_t returnVal = std::byteswap(*(uint32_t*)&rsp.dmem[offset]);

                Bus::writeWithMask32(&returnVal, value, mask);

                returnVal = std::byteswap(returnVal);

                memcpy(&rsp.dmem[offset], &returnVal, sizeof(uint32_t));

                return;
            }
            if (actualAddress >= 0x4001000 && actualAddress <= 0x4001FFF) {

                uint32_t offset = actualAddress - 0x4001000;

                uint32_t returnVal = std::byteswap(*(uint32_t*)&rsp.imem[offset]);

                Bus::writeWithMask32(&returnVal, value, mask);

                returnVal = std::byteswap(returnVal);

                memcpy(&rsp.imem[offset], &returnVal, sizeof(uint32_t));

                return;
            }
            if (actualAddress >= 0x5000000 && actualAddress <= 0x05ffffff) {
                // N64 DD registers, ignore
                return;
            }
            if (actualAddress >= 0x8000000 && actualAddress <= 0x801ffff) {
                if (std::find(saveTypes.begin(), saveTypes.end(), Sram) != saveTypes.end()) {
                    uint32_t offset = actualAddress - 0x8000000;

                    formatSram();

                    uint32_t returnVal = std::byteswap(*(uint32_t*)&sram[offset]);

                    Bus::writeWithMask32(&returnVal, value, mask);

                    returnVal = std::byteswap(returnVal);

                    memcpy(&sram[offset], &returnVal, sizeof(uint32_t));
                } else if (std::find(saveTypes.begin(), saveTypes.end(), Flash) != saveTypes.end()) {
                    if ((actualAddress & 0x1ffff) == 0x10000) {
                        formatFlash();
                        executeFlashCommand(value & mask);
                    } else if ((actualAddress & 0x1ffff) == 0 && flashMode == Status) {
                        flashStatus = (value & mask) & 0xff;
                    } else {
                        throw std::runtime_error("invalid flash command given");
                    }

                }

                return;
            }

            std::cout << "(memWrite32) unsupported address received: " << std::hex << actualAddress << "\n";
            throw std::runtime_error("");
            break;
    }
}

void Bus::openSaves(std::vector<std::string> saveNames) {

    for (std::string saveName: saveNames) {
        saveFile = std::fstream(saveName, std::ios::binary | std::ios::in | std::ios::out);

        if (saveFile.is_open()) {
            saveFile.seekg(0, std::ios::end);
            size_t fileSize = saveFile.tellg();
            saveFile.seekg(0, std::ios::beg);

            if (fileSize != 0) {
                for (SaveType saveType: saveTypes) {
                    switch (saveType) {
                        case Eeprom16k:
                        case Eeprom4k:
                            eeprom.resize(fileSize);
                            saveFile.read(reinterpret_cast<char*>(eeprom.data()), fileSize);
                            break;
                        case Flash:
                            flash.resize(fileSize);
                            saveFile.read(reinterpret_cast<char*>(flash.data()), fileSize);
                            break;
                        case Sram:
                            sram.resize(fileSize);
                            saveFile.read(reinterpret_cast<char*>(sram.data()), fileSize);
                            break;
                        case NoSave:
                            // do nothing
                            break;
                        case Mempak:
                            throw std::runtime_error("todo: mempak");
                            break;
                    }
                }
            }
        } else {
            saveFile = std::fstream(saveName, std::ios::out);
        }
    }
}

void Bus::writeSave() {
    if (saveFile.is_open()) {
        std::println("writing save....");
        saveFile.seekg(0, std::ios::beg);

        for (SaveType type: saveTypes) {
            switch (type) {
                case Eeprom16k:
                case Eeprom4k:
                    saveFile.write((char*)&eeprom[0], sizeof(uint8_t) * eeprom.size());
                    break;
                case Flash:
                    saveFile.write((char*)&flash[0], sizeof(uint8_t) * flash.size());
                    break;
                case Sram:
                    saveFile.write((char*)&sram[0], sizeof(uint8_t) * sram.size());
                    break;
                case NoSave:
                    // do nothing
                    break;
                case Mempak:
                    throw std::runtime_error("todo: mempak");
                    break;
            }
        }
    }

    timeSinceSaveWrite = 0;
}

std::vector<std::string> Bus::getSaveNames(std::string filename) {
    std::string saveName;
    std::string extension;

    std::vector<std::string> names = {};

    for (SaveType type: saveTypes) {
        switch (type) {
            case Eeprom16k:
            case Eeprom4k:
                extension = ".eep";
                break;
            case Flash:
                extension = ".fla";
                break;
            case Sram:
                extension = ".sra";
                break;
            case NoSave:
                break;
            case Mempak:
                extension = ".mem";
                break;
        }
        saveName = std::regex_replace(filename, std::regex("\\.n64$|\\.z64$|\\.N64$|\\.Z64$"), extension);
        saveName = std::regex_replace(saveName, std::regex(".*/"), "");

        names.push_back(saveName);
    }


    return names;
}

void Bus::formatEeprom() {
    if (eeprom.size() < 0x800) {
        eeprom.resize(0x800);
        std::fill(eeprom.begin(), eeprom.end(), 0xff);
    }
}

void Bus::formatFlash() {
    if (flash.size() < FLASH_SIZE) {
        flash.resize(FLASH_SIZE);
        std::fill(flash.begin(), flash.end(), 0xff);
    }
}

void Bus::executeFlashCommand(uint32_t command) {
    switch (command >> 24) {
        case 0xa5: {
            uint32_t offset = (command & 0xffff) * 128;

            flashStatus |= 0x1;

            for (int i = 0; i < FLASHBUF_SIZE; i++) {
                flash[offset + i] = flashBuffer[i];
            }

            flashStatus &= ~0x1;
            flashStatus |= 0x4;
            flashMode = Status;

            auto p1 = std::chrono::system_clock::now();
            timeSinceSaveWrite = std::chrono::duration_cast<std::chrono::milliseconds>(
                p1.time_since_epoch()
            ).count();
            break;
        }
        case 0xb4:
            flashMode = PageProgram;
            break;
        case 0xf0:
            flashMode = ReadArray;
            break;
        case 0xe1:
            flashMode = ReadSilliconId;
            flashStatus |= 0x1;
            break;
        case 0xd2:
            flashMode = Status;
            break;
        case 0x3c:
            flashMode = EraseChip;
            break;
        case 0x4b:
            flashMode = EraseSector;
            flashSector = command & 0xffff;
            break;
        case 0x78:
            flashStatus |= 0x2;

            if (flashMode == EraseChip) {
                for (int i = 0; i < flash.size(); i++) {
                    flash[i] = 0xff;
                }
                auto p1 = std::chrono::system_clock::now();
                timeSinceSaveWrite = std::chrono::duration_cast<std::chrono::milliseconds>(
                    p1.time_since_epoch()
                ).count();
            } else if (flashMode == EraseSector) {
                uint32_t offset = (flashSector & 0xff80) * 128;

                for (int i = 0; i < SECTOR_SIZE; i++) {
                    flash[offset + i] = 0xff;
                }
                auto p1 = std::chrono::system_clock::now();
                timeSinceSaveWrite = std::chrono::duration_cast<std::chrono::milliseconds>(
                    p1.time_since_epoch()
                ).count();
            } else {
                throw std::runtime_error("invalid erase flash command given");
            }


            flashStatus &= ~0x2;
            flashStatus |= 0x8;
            flashMode = Status;

            break;
        default:
            std::println("todo: {:x}", command >> 24);
            throw std::runtime_error("");
            break;
    }
}

void Bus::loadRomBytes(uint8_t* romBytes, uint32_t romSize) {
    uint32_t cartridgeFormat = std::byteswap(*(uint32_t*)&romBytes[0]);

    std::vector<uint8_t> formattedRom;
    formattedRom.resize(romSize);

    switch (cartridgeFormat) {
        case 0x80371240:
            memcpy(&formattedRom[0], romBytes, romSize);
            break;
        case 0x37804012:
            for (int i = 0; i < romSize; i += 2) {
                uint16_t data = std::byteswap(*(uint16_t*)&romBytes[i]);
                memcpy(&formattedRom[i], &data, sizeof(uint16_t));
            }
            break;
        case 0x40123780:
            for (int i = 0; i < romSize; i += 4) {
                uint32_t data = std::byteswap(*(uint32_t*)&romBytes[i]);
                memcpy(&formattedRom[i], &data, sizeof(uint32_t));
            }
            break;
    }

    cartridge = formattedRom;

    memcpy(&gameId, &cartridge[0x3b], 3);
    gameId[3] = 0;

    char header[3];

    memcpy(header, &cartridge[0x3c], 2);
    header[2] = 0;

    if (strcmp(header, "ED") == 0) {
        uint8_t saveType = cartridge[0x3f] >> 4;

        switch (saveType) {
            case 0:
                saveTypes = {};
                break;
            case 1:
                saveTypes = { Eeprom4k };
                break;
            case 2:
                saveTypes = { Eeprom16k };
                break;
            case 3:
                saveTypes = { Sram };
                break;
            case 5:
                saveTypes = { Flash };
                break;
            default:
                std::cout << "unknown save type detected: " << std::dec << saveType << "\n";
                throw std::runtime_error("");
                break;
        }
    } else {
        std::array<std::string, 23> eeprom16KSaves = {
            "NB7",
            "NGT",
            "NFU",
            "NCW",
            "NCZ",
            "ND6",
            "NDO",
            "ND2",
            "N3D",
            "NMX",
            "NGC",
            "NIM",
            "NNB",
            "NMV",
            "NM8",
            "NEV",
            "NPP",
            "NUB",
            "NPD",
            "NRZ",
            "NR7",
            "NEP",
            "NYS"
        };

        std::array<std::string, 19> flashSaves = {
            "NCC",
            "NDA",
            "NAF",
            "NJF",
            "NKJ",
            "NZS",
            "NM6",
            "NCK",
            "NMQ",
            "NPN",
            "NPF",
            "NPO",
            "CP2",
            "NP3",
            "NRH",
            "NSQ",
            "NT9",
            "NW4",
            "NDP"
        };

        if (std::find(eeprom16KSaves.begin(), eeprom16KSaves.end(), gameId) != eeprom16KSaves.end()) {
            saveTypes = { Eeprom16k };
        } else if (std::find(flashSaves.begin(), flashSaves.end(), gameId) != flashSaves.end()) {
            saveTypes = { Flash };
        } else if (strcmp(gameId, "NPQ") == 0) {
            saveTypes = {};
        } else {
            saveTypes = { Eeprom4k, Sram };
        }
    }
}

void Bus::loadRom(std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint8_t> rom;
    rom.resize(fileSize);

    file.read(reinterpret_cast<char*>(rom.data()), fileSize);
    file.close();

    loadRomBytes(&rom[0], fileSize);
}

uint32_t Bus::readInstructionCache(uint64_t actualAddress) {
    uint32_t lineIndex = ((actualAddress >> 5) & 0x1FF);

    if (!icacheHit(lineIndex, actualAddress)) {
        fillInstructionCache(lineIndex, actualAddress);
    }

    return icache[lineIndex].words[((actualAddress >> 2) & 7)];
}

bool Bus::icacheHit(uint32_t lineIndex, uint64_t address) {
    return icache[lineIndex].valid && (icache[lineIndex].tag & 0x1ffffffc) == (uint32_t)(address & ~0xfff);
}

void Bus::fillInstructionCache(uint32_t lineIndex, uint64_t address) {
    cpu.cop0.addCycles(8);

    icache[lineIndex].valid = true;
    icache[lineIndex].tag = (uint32_t)(address & ~0xfff);

    uint64_t cacheAddress = (uint64_t)((uint32_t)((icache[lineIndex].tag) | (icache[lineIndex].index)) & 0x1ffffffc);

    for (int i = 0; i < 8; i++) {
        icache[lineIndex].words[i] = memRead32(cacheAddress + i * 4, true, WidthICache);
    }
}

uint32_t Bus::readDataCache(uint64_t actualAddress, bool ignoreCycles) {
    uint32_t lineIndex = (actualAddress >> 4) & 0x1ff;

    if (!(dcacheHit(lineIndex, actualAddress))) {
        if (dcache[lineIndex].valid && dcache[lineIndex].dirty) {
            dcacheWriteback(lineIndex, ignoreCycles);
        }
        fillDataCache(lineIndex, actualAddress, ignoreCycles);
    } else {
        if (!ignoreCycles) {
            cpu.cop0.addCycles(1);
        }
    }

    return dcache[lineIndex].words[(actualAddress >> 2) & 3];
}

void Bus::writeDataCache(uint64_t actualAddress, uint32_t value, int64_t mask) {
    uint32_t lineIndex = (actualAddress >> 4) & 0x1ff;

    if (!dcacheHit(lineIndex, actualAddress)) {
        if (dcache[lineIndex].valid && dcache[lineIndex].dirty) {
            dcacheWriteback(lineIndex);
        }
        fillDataCache(lineIndex, actualAddress);
    } else {
        cpu.cop0.addCycles(1);
    }

    uint32_t returnValue = value;
    uint32_t oldValue = dcache[lineIndex].words[(actualAddress >> 2) & 3];

    if (mask != -1) {
        returnValue = (oldValue & ~mask) | (value & mask);
    }

    dcache[lineIndex].words[(actualAddress >> 2) & 3] = returnValue;
    dcache[lineIndex].dirty = true;
}

void Bus::fillDataCache(uint32_t lineIndex, uint64_t address, bool ignoreCycles) {
    if (!ignoreCycles) {
        cpu.cop0.addCycles(7);
    }

    dcache[lineIndex].valid = true;
    dcache[lineIndex].dirty = false;

    dcache[lineIndex].tag = (uint32_t)(address & ~0xfff);

    uint64_t cacheAddress = (uint64_t)((uint32_t)((dcache[lineIndex].tag) | (dcache[lineIndex].index)) & 0x1ffffffc);

    dcache[lineIndex].words[0] = memRead32(cacheAddress, true, WidthDCache);
    dcache[lineIndex].words[1] = memRead32(cacheAddress + 4, true, WidthDCache);
    dcache[lineIndex].words[2] = memRead32(cacheAddress + 8, true, WidthDCache);
    dcache[lineIndex].words[3] = memRead32(cacheAddress + 12, true, WidthDCache);
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

void Bus::dcacheWriteback(uint64_t line, bool ignoreCycles) {
    if (!ignoreCycles) {
        cpu.cop0.addCycles(36);
    }
    dcache[line].dirty = false;

    uint64_t cacheAddress = (dcache[line].tag | dcache[line].index) & 0x1ffffffc;

    for (int i = 0; i < 4; i++) {
        uint64_t currAddress = cacheAddress | (i * 4);
        memWrite32(currAddress, dcache[line].words[i], true, true);
    }
}

std::tuple<uint64_t, bool, bool> Bus::translateAddress(uint64_t address, bool isWrite) {
    if ((address & 0xc0000000) != 0x80000000) {
        return getTlbAddress(address, isWrite);
    }
    return std::tuple(address & 0x1fffffff, false, (address & 0x20000000) == 0);
}

std::tuple<uint64_t, bool, bool> Bus::getTlbAddress(uint64_t address, bool isWrite) {
    uint64_t actualAddress = address & 0xffffffff;

    if (isWrite) {
        if (tlbWriteLut[actualAddress >> 12].address != 0) {
            return std::tuple(tlbWriteLut[actualAddress >> 12].address & 0x1ffff000 | (actualAddress & 0xfff), false, tlbWriteLut[actualAddress >> 12].cached);
        }
    } else if (tlbReadLut[actualAddress >> 12].address != 0) {
        return std::tuple(tlbReadLut[actualAddress >> 12].address & 0x1ffff000 | (actualAddress & 0xfff), false, tlbReadLut[actualAddress >> 12].cached);
    }

    tlbException(address, isWrite);

    return std::tuple(0x0, true, false);
}

void Bus::tlbException(uint64_t address, bool isWrite) {
    if (isWrite) {
        cpu.cop0.cause = TLBS;
    } else {
        cpu.cop0.cause = TLBL;
    }

    cpu.cop0.badVAddress = address;

    cpu.cop0.xContext = (cpu.cop0.xContext & ~0x7ffff0) | ((address >> 9) & 0x7ffff0);
    cpu.cop0.context = (cpu.cop0.xContext & ~0x7ffff0) | ((address >> 9) & 0x7ffff0);
    cpu.cop0.xContext = (cpu.cop0.xContext & ~0x180000000) | ((address >> 31) & 0x180000000);

    cpu.cop0.entryHi = (cpu.cop0.entryHi & ~0xffffe000) | address & 0xffffe000;

    bool isValid = true;

    uint64_t offset = 0x180;

    uint64_t maskedAddress = address & ~0x3;

    for (TlbEntry entry: tlbEntries) {
        if (maskedAddress >= entry.startEven && maskedAddress <= entry.endEven) {
            isValid = entry.vEven != 0;

            if (isValid && isWrite && entry.dEven == 0) {
                cpu.cop0.cause = 1 << 2;
                isValid = false;
            }
            break;
        }

        if (maskedAddress >= entry.startOdd && maskedAddress <= entry.endOdd) {
            isValid = entry.vOdd != 0;

            if (isValid && isWrite && entry.dOdd == 0) {
                cpu.cop0.cause = 1 << 2;
                isValid = false;
            }
            break;
        }
    }

    if (isValid && cpu.cop0.status.exl == 0) {
        offset = 0;
    }

    cpu.enterException(false, offset);
}

void Bus::tlbWrite(uint32_t index) {
    if (index > 31) {
        return;
    }

    tlbUnmap(index);

    tlbEntries[index].g = (uint8_t)(cpu.cop0.entryLo0 & cpu.cop0.entryLo1 & 1);

    tlbEntries[index].pfnEven = (cpu.cop0.entryLo0 >> 6) & 0xfffff;
    tlbEntries[index].pfnOdd = (cpu.cop0.entryLo1 >> 6) & 0xfffff;
    tlbEntries[index].cEven = (uint8_t)((cpu.cop0.entryLo0 >> 3) & 7);
    tlbEntries[index].cOdd = (uint8_t)((cpu.cop0.entryLo1 >> 3) & 7);
    tlbEntries[index].dEven = (uint8_t)((cpu.cop0.entryLo0 >> 2) & 1);
    tlbEntries[index].dOdd = (uint8_t)((cpu.cop0.entryLo1 >> 2) & 1);
    tlbEntries[index].vEven = (uint8_t)((cpu.cop0.entryLo0 >> 1) & 1);
    tlbEntries[index].vOdd = (uint8_t)((cpu.cop0.entryLo1 >> 1) & 1);

    tlbEntries[index].asid = (uint8_t)cpu.cop0.entryHi;

    tlbEntries[index].mask = ((cpu.cop0.pageMask >> 13) & 0xfff) & 0b101010101010;
    tlbEntries[index].vpn2 = (cpu.cop0.entryHi >> 13) & 0x7ffffff;

    tlbEntries[index].region = (uint8_t)(cpu.cop0.entryHi >> 62);

    tlbEntries[index].mask |= tlbEntries[index].mask >> 1;

    tlbEntries[index].vpn2 &= ~tlbEntries[index].mask;

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

    cpu.cop0.pageMask = tlbEntries[index].mask << 13;

    cpu.cop0.entryHi = (uint64_t)tlbEntries[index].region << 62 |
        (uint64_t)tlbEntries[index].vpn2 << 13 |
        (uint64_t)tlbEntries[index].asid;

    cpu.cop0.entryLo0 = (uint64_t)tlbEntries[index].pfnEven << 6 |
        (uint64_t)tlbEntries[index].cEven << 3 |
        (uint64_t)tlbEntries[index].dEven << 2 |
        (uint64_t)tlbEntries[index].vEven << 1 |
        (uint64_t)tlbEntries[index].g;

    cpu.cop0.entryLo1 = (uint64_t)tlbEntries[index].pfnOdd << 6 |
        (uint64_t)tlbEntries[index].cOdd << 3 |
        (uint64_t)tlbEntries[index].dOdd << 2 |
        (uint64_t)tlbEntries[index].vOdd << 1 |
        (uint64_t)tlbEntries[index].g;
}

void Bus::recalculateDelay() {
    double expectedRefreshRate = (double)videoInterface.clock / (double)(videoInterface.vTotal +1) / (double)((videoInterface.hTotal) + 1) * 2.0;
    videoInterface.delay = (uint32_t)((double)cpu.clock / expectedRefreshRate);
    videoInterface.countPerScanline = videoInterface.delay  / (videoInterface.vTotal + 1);
}

void Bus::tlbProbe() {
    uint64_t returnVal = 0x80000000;

    for (uint64_t i = 0; i < tlbEntries.size(); i++) {
        TlbEntry entry = tlbEntries[i];

        if ((entry.vpn2 & ~entry.mask) == (((cpu.cop0.entryHi >> 13) & 0x7ffffff) & ~entry.mask) &&
            entry.region == (uint8_t)(cpu.cop0.entryHi >> 62) &&
            (entry.g != 0 || entry.asid == (uint8_t)cpu.cop0.entryHi)
        ) {
            returnVal = (uint64_t)i;
            break;
        }
    }

    cpu.cop0.index = returnVal;
}

void Bus::tlbMap(uint32_t index) {
    TlbEntry entry = tlbEntries[index];

    if (entry.vEven != 0 &&
        entry.startEven < entry.endEven &&
        !(entry.startEven >= 0x80000000 && entry.endEven < 0xc0000000) &&
        entry.physEven < 0x20000000
    ) {
        for (uint32_t i = entry.startEven; i < entry.endEven; i += 0x1000) {
            tlbReadLut[i >> 12].address = 0x80000000 | (entry.physEven + (i - entry.startEven) + 0xfff);
            tlbReadLut[i >> 12].cached = entry.cEven != 2;
        }


        if (entry.dEven != 0) {
            for (uint32_t i = entry.startEven; i < entry.endEven; i += 0x1000) {
                tlbWriteLut[i >> 12].address = 0x80000000 | (entry.physEven + (i - entry.startEven) + 0xfff);
                tlbWriteLut[i >> 12].cached = entry.cEven != 2;
            }
        }
    }

    if (entry.vOdd != 0 &&
        entry.startOdd < entry.endOdd &&
        !(entry.startOdd >= 0x80000000 && entry.endOdd < 0xc0000000) &&
        entry.physOdd < 0x20000000
    ) {
        for (uint32_t i = entry.startOdd; i < entry.endOdd; i += 0x1000) {
            tlbReadLut[i >> 12].address = 0x80000000 | (entry.physOdd + (i - entry.startOdd) + 0xfff);
            tlbReadLut[i >> 12].cached = entry.cOdd != 2;
        }

        if (entry.dOdd != 0) {
            for (uint32_t i = entry.startOdd; i < entry.endOdd; i += 0x1000) {
                tlbWriteLut[i >> 12].address = 0x80000000 | (entry.physOdd + (i - entry.startOdd) + 0xfff);
                tlbWriteLut[i >> 12].cached = entry.cOdd != 2;
            }
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

        if (entry.dEven != 0) {
            for (int i = entry.startEven; i < entry.endEven; i += 0x1000) {
                tlbWriteLut[i >> 12].address = 0;
                tlbWriteLut[i >> 12].cached = false;
            }
        }
    }

    if (entry.vOdd != 0) {
        for (int i = entry.startOdd; i < entry.endOdd; i += 0x1000) {
            tlbReadLut[i >> 12].address = 0;
            tlbReadLut[i >> 12].cached = false;
        }

        if (entry.dOdd != 0) {
            for (int i = entry.startOdd; i < entry.endOdd; i += 0x1000) {
                tlbWriteLut[i >> 12].address = 0;
                tlbWriteLut[i >> 12].cached = false;
            }
        }
    }
}

void Bus::setInterrupt(uint32_t flag) {
    mips.mipsInterrupt.value |= flag;

    if ((mips.mipsInterrupt.value & mips.mipsMask.value) != 0) {
        cpu.cop0.cause &= ~(0x1f << 2);
        cpu.cop0.cause |= 1 << 10;
    }
    cpu.checkIrqs();
}

void Bus::clearInterrupt(uint32_t flag) {
    mips.mipsInterrupt.value &= ~flag;

    if ((mips.mipsInterrupt.value & mips.mipsMask.value) == 0) {
        cpu.cop0.cause &= ~(1 << 10);
    }
    cpu.checkIrqs();
}

void Bus::dmaCartWrite() {
    uint32_t currDramAddr = peripheralInterface.dramAddress & 0xffffff;
    uint32_t currCartAddr = peripheralInterface.cartAddress & 0xfffffff;

    uint32_t length = peripheralInterface.wrLen + 1;

    if (length > 0x7f & (length & 1) != 0) {
        length += 1;
    }
    if (length <= 0x80) {
        length -= (currDramAddr & 0x7);
    }
    for (int i = 0; i < length; i++) {
        if (currCartAddr + i >= cartridge.size()) {
            // xoring with 3 essentially byteswaps the values
            rdram[(currDramAddr + i) ^ 3] = 0;
        } else {
            // see above comment
            rdram[(currDramAddr + i) ^ 3] = cartridge[currCartAddr + i];
        }
    }

    peripheralInterface.dramAddress += length + 7;
    peripheralInterface.dramAddress &= ~0x7;
    peripheralInterface.cartAddress += length + 1;
    peripheralInterface.cartAddress &= ~0b1;

    peripheralInterface.piStatus.dmaBusy = 1;

    uint64_t cycles = peripheralInterface.calculateCycles(1, length);

    cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
}

void Bus::dmaFlashWrite() {
    uint32_t length = peripheralInterface.wrLen + 1;
    uint32_t dramAddress = peripheralInterface.dramAddress & 0xfffffe;
    uint32_t currCartAddr = peripheralInterface.cartAddress;

    if (length >= 0x7f && (length & 1) != 0) {
        length += 1;
    }
    if (length <= 0x80) {
        length -= dramAddress & 0x7;
    }

    if ((currCartAddr & 0x1ffff) == 0 && length == 8 && flashMode == ReadSilliconId) {
        memcpy(&rdram[dramAddress], &FLASH_ID, sizeof(uint32_t));
        memcpy(&rdram[dramAddress + 4], &SILICON_ID, sizeof(uint32_t));
    } else if ((currCartAddr & 0x1ffff) < 0x10000 && flashMode == ReadArray) {
        formatFlash();
        currCartAddr = (currCartAddr & 0xffff) * 2;

        for (int i = 0; i < length; i++) {
            rdram[(dramAddress + i) ^ 3] = flash[currCartAddr + i];
        }
    }

    uint64_t cycles = peripheralInterface.calculateCycles(2, length);

    cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
}

void Bus::dmaFlashRead() {
    uint32_t length = peripheralInterface.rdLen + 1;
    uint32_t dramAddress = peripheralInterface.dramAddress & 0xfffffe;
    uint32_t currCartAddr = peripheralInterface.cartAddress;

    if (length >= 0x7f && (length & 1) != 0) {
        length += 1;
    }
    if (length <= 0x80) {
        length -= dramAddress & 0x7;
    }

    if ((currCartAddr & 0x1ffff) == 0 && length == 128 && flashMode == PageProgram) {
        for (int i = 0; i < length; i++) {
            flashBuffer[i] = rdram[(dramAddress + i) ^ 3];
        }
    } else {
        throw std::runtime_error("invalid dma flash read option given");
    }

    uint64_t cycles = peripheralInterface.calculateCycles(2, length);

    cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
}

void Bus::dmaSramWrite() {
    uint32_t currCartAddr = peripheralInterface.cartAddress & 0xffff;
    uint32_t currDramAddr = peripheralInterface.dramAddress & 0xffffff;

    uint32_t length = peripheralInterface.wrLen + 1;

    if (length >= 0x7f && (length & 1) != 0) {
        length += 1;
    }
    if (length <= 0x80) {
        length -= currDramAddr & 0x7;
    }

    formatSram();

    for (int i = 0; i < length; i++) {
        if (currCartAddr + i == sram.size()) {
            break;
        }

        rdram[(currDramAddr + i) ^ 3] = sram[currCartAddr + i];
    }

    uint64_t cycles = peripheralInterface.calculateCycles(2, length);
    cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
}

void Bus::dmaSramRead() {
    uint32_t currCartAddr = peripheralInterface.cartAddress & 0xffff;
    uint32_t currDramAddr = peripheralInterface.dramAddress & 0xffffff;

    uint32_t length = peripheralInterface.rdLen + 1;

    if (length >= 0x7f && (length & 1) != 0) {
        length += 1;
    }
    if (length <= 0x80) {
        length -= currDramAddr & 0x7;
    }

    formatSram();

    for (int i = 0; i < length; i++) {
        if (currCartAddr + i == sram.size()) {
            break;
        }

        sram[currCartAddr + i] = rdram[(currDramAddr + i) ^ 3];
    }

    auto p1 = std::chrono::system_clock::now();
    timeSinceSaveWrite = std::chrono::duration_cast<std::chrono::milliseconds>(
        p1.time_since_epoch()
    ).count();

    uint64_t cycles = peripheralInterface.calculateCycles(2, length);
    cpu.scheduler.addEvent(Event(PIDma, cpu.cop0.count + cycles));
}


void Bus::formatSram() {
    if (sram.size() < SRAM_SIZE) {
        sram.resize(SRAM_SIZE);
        std::fill(sram.begin(), sram.end(), 0xff);
    }
}


// TODO: move this to mips interface. currently having compile
// issues putting the code in there :/
void Bus::checkIrqs() {
    if ((mips.mipsMask.value & mips.mipsInterrupt.value) != 0) {
        cpu.cop0.cause &= ~(0x1f << 2);
        cpu.cop0.cause |= 1 << 10;
    } else {
        cpu.cop0.cause &= ~(1 << 10);
    }
}

void Bus::finishPiDma() {
    peripheralInterface.piStatus.ioBusy = 0;
    peripheralInterface.piStatus.dmaBusy = 0;
    peripheralInterface.piStatus.dmaCompleted = 1;

    setInterrupt(PI_INTERRUPT_FLAG);
}

uint32_t Bus::calculateRdRamCycles(uint32_t length) {
    return 31 + (length / 3);
}

void Bus::writeWithMask32(uint32_t* oldVal, uint32_t newVal, uint32_t mask) {
    uint32_t returnVal = newVal;
    if (mask != -1) {
        returnVal = (*oldVal & ~mask) | (newVal & mask);
    }

    *oldVal = returnVal;
}

void Bus::writeValueLE(uint8_t* ptr, uint32_t value, int size) {
    for (int i = 0; i < size; i++) {
        int shift = i * 8;

        ptr[i] = (uint8_t)(value >> shift);
    }
}

void Bus::initAudio() {
    const SDL_AudioSpec srcspec = { SDL_AUDIO_S16, 2, (int)audioInterface.frequency };
    SDL_AudioSpec devicespec = { SDL_AUDIO_S16, 2, 48000 };

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &devicespec);

    SDL_AudioSpec dstspec;
    if (!SDL_GetAudioDeviceFormat(device, &dstspec, nullptr)) {
        std::println("couldn't get audio device format: {}", SDL_GetError());
    }

    SDL_AudioStream *stream = SDL_CreateAudioStream(&srcspec, &dstspec);

    if (!SDL_BindAudioStream(device, stream)) {
        std::println("couldn't bind audio stream to device: {}", SDL_GetError());
    }

    this->stream = stream;
}

void Bus::restartAudio() {
    SDL_DestroyAudioStream(stream);
    initAudio();
}

void Bus::pushSamples(uint64_t length, uint64_t dramAddress) {
    int16_t samples[length / 2];

    for (int i = 0; i < length / 2; i++) {
        if ((i & 1) == 0) {
            samples[i] = *(int16_t*)&rdram[dramAddress + (i * 2) + 2];
        } else {
            samples[i] = *(int16_t*)&rdram[dramAddress + (i * 2)];
        }
    }

    if (!SDL_PutAudioStreamData(stream, samples, (length / 2) * sizeof(int16_t))) {
        std::println("couldn't put samples into stream: {}", SDL_GetError());
    }
}

void Bus::updateButton(JoypadButton button, bool state) {
    if (state) {
        input |= 1 << (uint16_t)button;
    } else {
        input &= ~(1 << (uint16_t)button);
    }
}

void Bus::updateAxis(uint8_t xAxis, uint8_t yAxis) {
    input &= 0xffff;
    input |= xAxis << 16;
    input |= yAxis << 24;
}

void Bus::setCic() {
    std::string hash = sha256();

    std::transform(hash.begin(), hash.end(), hash.begin(),
    [](unsigned char c){ return std::toupper(c); });

    // wish c++ supported switch case with strings lmao
    // 0x3f is true for most cases, so worry about the cases
    // that it isn't true for only.
    uint8_t cicSeed = 0x3f;
    if (hash == "BF3620D30817007091EBE9BDDD1B88C23B8A0052170B3309CDE5B6B4238E45E7") {
        cicSeed = 0x78;
    } else if (hash == "04B7BC6717A9F0EB724CF927E74AD3876C381CBB280D841736FC5E55580B756B") {
        cicSeed = 0x91;
    } else if (hash == "36ADC40148AF56F0D78CD505EB6A90117D1FD6F11C6309E52ED36BC4C6BA340E") {
        cicSeed = 0x85;
    } else if (hash == "53C0088FB777870D0AF32F0251E964030E2E8B72E830C26042FD191169508C05") {
        cicSeed = 0xdd;
    }

    pif.ram[0x26] = cicSeed;
    pif.ram[0x26] = cicSeed;
}

void Bus::writeRumblePak(int channel, uint16_t address, int data) {
    if (address == 0xc000) {
        uint16_t rumble = (uint16_t)pif.ram[data + 31];

        // todo: implement more controllers, but for now there's only one.
        if (channel == 0) {
            // todo: check if these values are right or whether they can vary
            SDL_RumbleGamepad(
                gamepad,
                (rumble & 1) * 0xffff,
                (rumble & 1) * 0xffff,
                (rumble & 1) * 60000
            );
        }
    }
}

std::string Bus::sha256() {
    uint16_t size = 0x1000 - 0x40;

    // Safely construct a buffer of the relevant bytes
    const uint8_t* data = &cartridge[0x40];

    // Hash buffer using CommonCrypto
    unsigned char hash[CC_SHA256_DIGEST_LENGTH];
    CC_SHA256(data, size, hash);

    // Convert to hex string
    std::ostringstream result;
    for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) {
        result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return result.str();
}