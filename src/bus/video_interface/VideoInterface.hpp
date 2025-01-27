#include <cstdint>
#include "VCurrent.hpp"
#include "VCtrl.hpp"
#include "HTotalLeap.hpp"
#include "VVideo.hpp"
#include "XScale.hpp"
#include "YScale.hpp"

class VideoInterface {
public:
    uint16_t vInterrupt;
    uint16_t hVideo;
    VCurrent vcurrent;
    VCtrl ctrl;
    uint32_t origin;
    uint16_t width;
    uint32_t viBurst;
    uint16_t vTotal;
    uint16_t hTotal;
    HTotalLeap hTotalLeap;
    VVideo vVideo;
    uint32_t vBurst;
    XScale xScale;
    YScale yScale;
};