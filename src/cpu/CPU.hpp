#include <cstdint>
#include "../bus/Bus.cpp"

class CPU {
public:
    uint64_t r[32];
    uint64_t pc;
    Bus bus;

    void helloWorld();

    CPU();

    void set(int i, uint64_t val);

    void step();
};