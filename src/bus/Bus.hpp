#include <cstdint>
#include <vector>

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdramBit;
    Bus();
};