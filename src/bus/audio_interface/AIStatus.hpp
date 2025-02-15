#include <cstdint>

typedef union AIStatus {
    struct {
        unsigned int dmaFull1: 1;
        unsigned int count: 15;
        unsigned int bc: 1;
        unsigned int unused0: 1;
        unsigned int unused1: 1;
        unsigned int wc: 1;
        unsigned int unused2: 1;
        unsigned int unused3: 1;
        unsigned int unused4: 1;
        unsigned int unused5: 1;
        unsigned int unused6: 1;
        unsigned int enabled: 1;
        unsigned int unused7: 1;
        unsigned int unused8: 1;
        unsigned int unused9: 1;
        unsigned int unused10: 1;
        unsigned int unused11: 1;
        unsigned int dmaBusy: 1;
        unsigned int dmaFull2: 1;
    };

    uint32_t value = 0;
} aistatusbitset;