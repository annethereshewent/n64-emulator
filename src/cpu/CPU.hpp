#include <cstdint>
#include "../bus/Bus.cpp"
#include <vector>

class CPU;
typedef void (*CPUInstruction)(CPU* cpu, uint32_t instruction);

class COP0 {
public:
    uint32_t r[32];

    COP0();

    static void mfc0(CPU* cpu, uint32_t instruction);
    static void dmfc0(CPU* cpu, uint32_t instruction);
    static void mtc0(CPU* cpu, uint32_t instruction);
    static void dmtc0(CPU* cpu, uint32_t instruction);
    static void cp0(CPU* cpu, uint32_t instruction);


    static void reserved(CPU* cpu, uint32_t instruction);

    std::array<CPUInstruction, 64> instructions;
};

class COP1 {
public:
    static void reserved(CPU* cpu, uint32_t instruction);
    static void lwc1(CPU* cpu, uint32_t instruction);
    static void ldc1(CPU* cpu, uint32_t instruction);
    static void swc1(CPU* cpu, uint32_t instruction);
    static void sdc1(CPU* cpu, uint32_t instruction);
};

class CPU {
public:
    uint64_t r[32];

    uint64_t pc;
    uint64_t nextPc;

    Bus bus;

    COP0 cop0;

    bool discarded;

    std::vector<uint8_t> cartridge;

    CPU();

    std::array<CPUInstruction, 64> instructions;

    void set(int i, uint64_t val);

    void step();

    void loadRom(std::string filename);

    // instructions
    static void reserved(CPU* cpu, uint32_t instruction);
    static void j(CPU* cpu, uint32_t instruction);
    static void jal(CPU* cpu, uint32_t instruction);
    static void beq(CPU* cpu, uint32_t instruction);
    static void bne(CPU* cpu, uint32_t instruction);
    static void blez(CPU* cpu, uint32_t instruction);
    static void bgtz(CPU* cpu, uint32_t instruction);
    static void addi(CPU* cpu, uint32_t instruction);
    static void addiu(CPU* cpu, uint32_t instruction);
    static void slti(CPU* cpu, uint32_t instruction);
    static void sltiu(CPU* cpu, uint32_t instruction);
    static void andi(CPU* cpu, uint32_t instruction);
    static void ori(CPU* cpu, uint32_t instruction);
    static void xori(CPU* cpu, uint32_t instruction);
    static void lui(CPU* cpu, uint32_t instruction);
    static void beql(CPU* cpu, uint32_t instruction);
    static void bnel(CPU* cpu, uint32_t instruction);
    static void blezl(CPU* cpu, uint32_t instruction);
    static void bgtzl(CPU* cpu, uint32_t instruction);
    static void daddi(CPU* cpu, uint32_t instruction);
    static void daddiu(CPU* cpu, uint32_t instruction);
    static void ldl(CPU* cpu, uint32_t instruction);
    static void ldr(CPU* cpu, uint32_t instruction);
    static void lb(CPU* cpu, uint32_t instruction);
    static void lh(CPU* cpu, uint32_t instruction);
    static void lwl(CPU* cpu, uint32_t instruction);
    static void lw(CPU* cpu, uint32_t instruction);
    static void lbu(CPU* cpu, uint32_t instruction);
    static void lhu(CPU* cpu, uint32_t instruction);
    static void lwr(CPU* cpu, uint32_t instruction);
    static void lwu(CPU* cpu, uint32_t instruction);
    static void sb(CPU* cpu, uint32_t instruction);
    static void sh(CPU* cpu, uint32_t instruction);
    static void swl(CPU* cpu, uint32_t instruction);
    static void sw(CPU* cpu, uint32_t instruction);
    static void sdl(CPU* cpu, uint32_t instruction);
    static void sdr(CPU* cpu, uint32_t instruction);
    static void swr(CPU* cpu, uint32_t instruction);
    static void cache(CPU* cpu, uint32_t instruction);
    static void ll(CPU* cpu, uint32_t instruction);
    static void lld(CPU* cpu, uint32_t instruction);
    static void ld(CPU* cpu, uint32_t instruction);
    static void sc(CPU* cpu, uint32_t instruction);
    static void scd(CPU* cpu, uint32_t instruction);
    static void sd(CPU* cpu, uint32_t instruction);
};