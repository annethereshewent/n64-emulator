#include <cstdint>

class MIPSInterface {
public:
    bool upperMode;
    // number of bytes (minus 1) to write in repeat mode
    uint8_t repeatCount;
    bool repeatMode;
    bool ebus;
    // TODO: add dp interrupt support

    void write(uint32_t value);
};