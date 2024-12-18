#include <cstdint>
#include <vector>

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<uint8_t> spdmem;

    Bus();

    uint32_t memRead32(uint64_t address);

    static uint64_t translateAddress(uint64_t address);
};