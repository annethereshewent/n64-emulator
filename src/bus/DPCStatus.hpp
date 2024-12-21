#include <cstdint>

typedef union DPCStatus {
    struct {
        unsigned int xbus: 1;
        unsigned int freeze: 1;
        unsigned int flush: 1;
        unsigned int gclk: 1;
        unsigned int tmemBusy: 1;
        unsigned int pipeBusy: 1;
        unsigned int cmdBusy: 1;
        unsigned int cbufReady: 1;
        unsigned int dmaBusy: 1;
        unsigned int endPending: 1;
        unsigned int startPending: 1;
    };

    uint32_t value;
} dpcbitfield;