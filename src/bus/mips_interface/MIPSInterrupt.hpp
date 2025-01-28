#include <cstdint>

typedef union MIPSInterrupt {
    struct {
        unsigned int spInterrupt: 1;
        unsigned int siInterrupt: 1;
        unsigned int aiInterrupt: 1;
        unsigned int viInterrupt: 1;
        unsigned int piInterrupt: 1;
        unsigned int dpInterrupt: 1;
    };

    uint8_t value;
} mipsinterruptbitset;