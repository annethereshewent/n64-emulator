#include <cstdint>
#include "VCurrent.hpp"

class VideoInterface {
public:
    uint16_t vInterrupt;
    uint16_t hVideo;
    VCurrent vcurrent;
};