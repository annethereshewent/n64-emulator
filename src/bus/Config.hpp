#include <cstdint>

typedef union Config {
    struct {
        unsigned int cc: 6;
        unsigned int autoCC: 1;
    };

    uint32_t value;
} configbitset;