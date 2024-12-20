#include <cstdint>
#include <vector>
#include "SPStatus.cpp"
#include "PIStatus.cpp"


class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<uint8_t> spdmem;

    uint16_t vInterrupt;
    uint16_t hVideo;

    SPStatus spStatus;
    PIStatus piStatus;
    Bus();

    uint32_t memRead32(uint64_t address);
    void memWrite32(uint64_t address, uint32_t value);

    static uint64_t translateAddress(uint64_t address);
};