#pragma once

#include <cstdint>
#include <vector>
#include "peripheral_interface/PeripheralInterface.hpp"
#include "rsp/RSP.hpp"
#include "video_interface/VideoInterface.hpp"
#include "audio_interface/AudioInterface.hpp"
#include "serial_interface/SerialInterface.hpp"
#include "rdp/RDP.hpp"
#include "pif/PIF.cpp"
#include "rdram_interface/RDInterface.hpp"
#include "mips_interface/MIPSInterface.cpp"
#include "cache/ICache.hpp"
#include "cache/DCache.hpp"

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<uint8_t> spdmem;

    PIF pif;

    std::vector<uint8_t> cartridge;

    std::array<ICache, 512> icache;
    std::array<DCache, 512> dcache;

    Bus();

    PeripheralInterface peripheralInterface;
    RSP rsp;
    RDP rdp;
    AudioInterface audioInterface;
    VideoInterface videoInterface;

    SerialInterface serialInterface;

    RDInterface rdInterface;

    MIPSInterface mips;

    uint64_t memRead64(uint64_t address);
    uint32_t memRead32(uint64_t address);
    uint16_t memRead16(uint64_t address);
    uint8_t memRead8(uint64_t address);

    void memWrite32(uint64_t address, uint32_t value);
    void memWrite16(uint64_t address, uint16_t value);

    void dcacheWriteback(uint64_t line);

    void dmaWrite();

    static uint64_t translateAddress(uint64_t address);

    static void writeWord(uint8_t* ptr, uint32_t address, uint32_t value);
    static void writeHalf(uint8_t* ptr, uint32_t address, uint16_t value);
};