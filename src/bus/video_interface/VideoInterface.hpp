#include <cstdint>
#include "VCurrent.hpp"
#include "VCtrl.hpp"

class VideoInterface {
public:
    uint16_t vInterrupt;
    uint16_t hVideo;
    VCurrent vcurrent;
    VCtrl ctrl;
};