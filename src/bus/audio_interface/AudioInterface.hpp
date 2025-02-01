#include <cstdint>

class AudioInterface {
public:
    uint32_t dramAddress = 0;
    uint32_t audioLength = 0;
    uint16_t dacRate = 0;
    uint8_t bitRate = 0;
    bool dmaEnable = false;
};