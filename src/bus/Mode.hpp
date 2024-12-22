#include <cstdint>

typedef union Mode {
    struct {
        unsigned int opMode: 2;
        unsigned int stopT: 1;
        unsigned int stopR: 1;
    };

    uint32_t value;
} modebitset;