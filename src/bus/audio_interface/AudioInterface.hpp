#include <cstdint>

class AudioInterface {
public:
    uint32_t dramAddress;
    uint32_t audioLength;
    uint16_t dacRate;
    uint8_t bitRate;
    bool dmaEnable;
};