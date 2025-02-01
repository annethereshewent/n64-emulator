#include <cstdint>

typedef union RDSelect {
    struct {
        unsigned int rsel: 4;
        unsigned int tsel: 4;
    };

    uint32_t value = 0;
} rdselbitset;