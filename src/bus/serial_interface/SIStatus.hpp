#include <cstdint>

typedef union SIStatus {
    struct {
        unsigned int dmaBusy: 1;
        unsigned int ioBusy: 1;
        unsigned int readPending: 1;
        unsigned int dmaError: 1;
        unsigned int pchState: 4;
        unsigned int dmaState: 4;
        unsigned int interrupt: 1;
        unsigned int reserved: 19;
    };

    uint32_t value = 0;
} sibitset;