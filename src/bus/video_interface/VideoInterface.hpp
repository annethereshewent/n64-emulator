#include <cstdint>
#include "VCurrent.hpp"
#include "VCtrl.hpp"
#include "HTotalLeap.hpp"
#include "VVideo.hpp"
#include "XScale.hpp"
#include "YScale.hpp"

class VideoInterface {
public:
    uint32_t vInterrupt = 0;
    uint32_t hVideo = 0;
    VCurrent vcurrent;
    VCtrl ctrl;
    uint32_t origin = 0;
    uint32_t width = 0;
    uint32_t viBurst = 0;
    uint32_t vTotal = 0;
    uint32_t hTotal = 0;
    HTotalLeap hTotalLeap;
    VVideo vVideo;
    uint32_t vBurst = 0;
    XScale xScale;
    YScale yScale;

    uint32_t clock = 48681812;

    bool interruptStarted;
    uint32_t delay;
};