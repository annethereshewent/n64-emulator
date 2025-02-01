#include <cstdint>
#include "VCurrent.hpp"
#include "VCtrl.hpp"
#include "HTotalLeap.hpp"
#include "VVideo.hpp"
#include "XScale.hpp"
#include "YScale.hpp"

class VideoInterface {
public:
    uint16_t vInterrupt = 0;
    uint16_t hVideo = 0;
    VCurrent vcurrent;
    VCtrl ctrl;
    uint32_t origin = 0;
    uint16_t width = 0;
    uint32_t viBurst = 0;
    uint16_t vTotal = 0;
    uint16_t hTotal = 0;
    HTotalLeap hTotalLeap;
    VVideo vVideo;
    uint32_t vBurst = 0;
    XScale xScale;
    YScale yScale;

    bool interruptStarted;
    uint32_t delay;
};