#include <cstdint>
#include <vector>

typedef union SPStatus {
    struct {
        unsigned int halted: 1;
        unsigned int broke: 1;
        unsigned int dmaBusy: 1;
        unsigned int dmaFull: 1;
        unsigned int ioBusy: 1;
        unsigned int singleStep: 1;
        unsigned int intBreak: 1;
        unsigned int flag1: 1;
        unsigned int flag2: 1;
        unsigned int flag3: 1;
        unsigned int flag4: 1;
        unsigned int flag5: 1;
        unsigned int flag6: 1;
        unsigned int flag7: 1;
        unsigned int flag8: 1;
    };

    uint32_t value;
} bitset;

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<uint8_t> spdmem;

    SPStatus spStatus;

    Bus();

    uint32_t memRead32(uint64_t address);

    static uint64_t translateAddress(uint64_t address);
};