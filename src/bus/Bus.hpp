#pragma once

#include <cstdint>
#include <vector>
#include "PeripheralInterface.hpp"
#include "RSP.hpp"
#include "VideoInterface.hpp"
#include "AudioInterface.hpp"
#include "SerialInterface.hpp"
#include "RDP.hpp"
#include "PIF.cpp"

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<uint8_t> spdmem;

    PIF pif;

    std::vector<uint8_t> cartridge;

    Bus();

    PeripheralInterface peripheralInterface;
    RSP rsp;
    RDP rdp;
    AudioInterface audioInterface;
    VideoInterface videoInterface;

    SerialInterface serialInterface;

    uint32_t memRead32(uint64_t address);
    uint8_t memRead8(uint64_t address);

    void memWrite32(uint64_t address, uint32_t value);

    static uint64_t translateAddress(uint64_t address);

    static void writeWord(uint8_t* ptr, uint32_t address, uint32_t value);
};