#include <cstdint>
#include "SPStatus.hpp"

class RSP {
public:
    SPStatus status;
    uint32_t pc;
    std::array<uint8_t, 0x1000> imem;
    std::array<uint8_t, 0x1000> dmem;
};