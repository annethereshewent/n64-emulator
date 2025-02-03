#include <cstdint>
#include "SPStatus.hpp"
#include "SPLength.hpp"


enum DmaDirection {
    Read,
    Write
};

class SPDma {
public:
    DmaDirection direction = Read;
    SPLength length;
    uint32_t dramAddress = 0;
    uint32_t memAddress = 0;
};

class RSP {
public:
    SPStatus status;
    uint32_t pc = 0;

    bool cpuBroken = false;
    uint64_t cycleCounter = 0;

    uint16_t dmaMemAddress = 0;
    uint32_t dmaRamAddress = 0;
    SPLength spReadLength;
    SPLength spWriteLength;

    std::array<SPDma, 2> fifo;

    std::array<uint8_t, 0x1000> imem = {};
    std::array<uint8_t, 0x1000> dmem = {};

    void pushDma(DmaDirection direction);
    bool popDma();
};