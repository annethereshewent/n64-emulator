#include <cstdint>
#include "../bus/Bus.cpp"
#include <vector>

class COP0 {
public:
    uint32_t r[32];
};

class CPU {
public:
    uint64_t r[32];
    uint64_t pc;
    Bus bus;

    COP0 cop0;

    std::vector<uint8_t> cartridge;

    CPU();

    void set(int i, uint64_t val);

    void step();

    void loadRom(std::string filename);
};