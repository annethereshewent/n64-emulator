#include <cstdint>
#include <vector>
#include "PeripheralInterface.hpp"
#include "RSP.cpp"
#include "VideoInterface.hpp"
#include "AudioInterface.hpp"

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<uint8_t> spdmem;

    std::array<uint8_t, 0x3f> pifRam;

    Bus();

    PeripheralInterface peripheralInterface;
    RSP rsp;
    AudioInterface audioInterface;
    VideoInterface videoInterface;

    uint32_t memRead32(uint64_t address);
    void memWrite32(uint64_t address, uint32_t value);

    static uint64_t translateAddress(uint64_t address);

    static void writeWord(uint8_t* ptr, uint32_t address, uint32_t value);
};