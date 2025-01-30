#pragma once

#include <cstdint>
#include "../bus/Bus.hpp"
#include "../bus/cop1/FCSR.hpp"
#include <vector>
#include "CPUInstruction.hpp"
#include <unordered_set>

class CPU;

class COP0 {
public:
    uint32_t index;
    uint32_t random;
    uint64_t entryLo0;
    uint64_t entryLo1;
    uint64_t context;
    uint32_t pageMask;
    uint32_t wired;
    uint64_t badVAddress;
    uint32_t count;
    uint64_t entryHi;
    uint32_t compare;
    uint32_t status;
    uint32_t cause;
    uint64_t epc;
    uint32_t prid;
    uint32_t config;
    uint32_t llAddress;
    uint32_t watchLo;
    uint32_t watchHi;
    uint64_t xContext;
    uint32_t parityError;
    uint32_t cacheError;
    uint32_t tagLo;
    uint32_t tagHi;
    uint64_t errorEpc;

    uint64_t latch;

    COP0();

    static void mfc0(CPU* cpu, uint32_t instruction);
    static void dmfc0(CPU* cpu, uint32_t instruction);
    static void mtc0(CPU* cpu, uint32_t instruction);
    static void dmtc0(CPU* cpu, uint32_t instruction);
    static void cp0(CPU* cpu, uint32_t instruction);
    static void syscall(CPU* cpu, uint32_t instruction);
    static void break_(CPU* cpu, uint32_t instruction);
    static void tlbr(CPU* cpu, uint32_t instruction);
    static void tlbwi(CPU* cpu, uint32_t instruction);
    static void tlbwr(CPU* cpu, uint32_t instruction);
    static void tlbp(CPU* cpu, uint32_t instruction);
    static void eret(CPU* cpu, uint32_t instruction);

    static void reserved(CPU* cpu, uint32_t instruction);

    void writeRegister(uint32_t index, uint64_t value);
    uint64_t readRegister(uint32_t index);

    std::array<CPUInstruction, 64> instructions;
    std::array<CPUInstruction, 32> cp0Instructions;
};

class COP1 {
public:

    FCSR fcsr;
    COP1();

    std::array<uint32_t, 32> fgr32;
    std::array<uint64_t, 32> fgr64;

    static void reserved(CPU* cpu, uint32_t instruction);
    static void lwc1(CPU* cpu, uint32_t instruction);
    static void ldc1(CPU* cpu, uint32_t instruction);
    static void swc1(CPU* cpu, uint32_t instruction);
    static void sdc1(CPU* cpu, uint32_t instruction);

    static void mfc1(CPU* cpu, uint32_t instructon);
    static void dmfc1(CPU* cpu, uint32_t instructon);
    static void cfc1(CPU* cpu, uint32_t instructon);
    static void dcfc1(CPU* cpu, uint32_t instructon);
    static void mtc1(CPU* cpu, uint32_t instructon);
    static void dmtc1(CPU* cpu, uint32_t instructon);
    static void ctc1(CPU* cpu, uint32_t instructon);
    static void dctc1(CPU* cpu, uint32_t instructon);

    static void cop1_b_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_s_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_d_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_w_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_l_instrs(CPU* cpu, uint32_t instructon);

    void writeRegister(uint32_t index, uint64_t value);
    uint32_t readRegister(uint32_t index);

    std::array<CPUInstruction, 32> instructions;
};

class CPU {
public:
    std::array<uint64_t, 32> r;
    bool llbit;

    uint64_t hi;
    uint64_t lo;

    uint64_t previousPc;
    uint64_t pc;
    uint64_t nextPc;

    Bus bus;

    COP0 cop0;
    COP1 cop1;

    bool discarded;

    CPU();

    std::array<CPUInstruction, 64> instructions;
    std::array<CPUInstruction, 64> secondary;
    std::array<CPUInstruction, 32> registerInstructions;

    std::unordered_set<uint64_t> visited;

    void set(int i, uint64_t val);

    void step();

    void loadRom(std::string filename);

    void checkIrqs();

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
    static void sll(CPU* cpu, uint32_t instruction);
    static void srl(CPU* cpu, uint32_t instruction);
    static void sra(CPU* cpu, uint32_t instruction);
    static void sllv(CPU* cpu, uint32_t instruction);
    static void srlv(CPU* cpu, uint32_t instruction);
    static void srav(CPU* cpu, uint32_t instruction);
    static void jr(CPU* cpu, uint32_t instruction);
    static void jalr(CPU* cpu, uint32_t instruction);
    static void sync(CPU* cpu, uint32_t instruction);
    static void mfhi(CPU* cpu, uint32_t instruction);
    static void mthi(CPU* cpu, uint32_t instruction);
    static void mflo(CPU* cpu, uint32_t instruction);
    static void mtlo(CPU* cpu, uint32_t instruction);
    static void dsllv(CPU* cpu, uint32_t instruction);
    static void dsrlv(CPU* cpu, uint32_t instruction);
    static void dsrav(CPU* cpu, uint32_t instruction);
    static void mult(CPU* cpu, uint32_t instruction);
    static void multu(CPU* cpu, uint32_t instruction);
    static void div(CPU* cpu, uint32_t instruction);
    static void divu(CPU* cpu, uint32_t instruction);
    static void dmult(CPU* cpu, uint32_t instruction);
    static void dmultu(CPU* cpu, uint32_t instruction);
    static void ddiv(CPU* cpu, uint32_t instruction);
    static void ddivu(CPU* cpu, uint32_t instruction);
    static void add(CPU* cpu, uint32_t instruction);
    static void addu(CPU* cpu, uint32_t instruction);
    static void sub(CPU* cpu, uint32_t instruction);
    static void subu(CPU* cpu, uint32_t instruction);
    static void and_(CPU* cpu, uint32_t instruction);
    static void or_(CPU* cpu, uint32_t instruction);
    static void xor_(CPU* cpu, uint32_t instruction);
    static void nor(CPU* cpu, uint32_t instruction);
    static void slt(CPU* cpu, uint32_t instruction);
    static void sltu(CPU* cpu, uint32_t instruction);
    static void dadd(CPU* cpu, uint32_t instruction);
    static void daddu(CPU* cpu, uint32_t instruction);
    static void dsub(CPU* cpu, uint32_t instruction);
    static void dsubu(CPU* cpu, uint32_t instruction);
    static void tge(CPU* cpu, uint32_t instruction);
    static void tgeu(CPU* cpu, uint32_t instruction);
    static void tlt(CPU* cpu, uint32_t instruction);
    static void tltu(CPU* cpu, uint32_t instruction);
    static void teq(CPU* cpu, uint32_t instruction);
    static void tne(CPU* cpu, uint32_t instruction);
    static void dsll(CPU* cpu, uint32_t instruction);
    static void dsrl(CPU* cpu, uint32_t instruction);
    static void dsra(CPU* cpu, uint32_t instruction);
    static void dsll32(CPU* cpu, uint32_t instruction);
    static void dsrl32(CPU* cpu, uint32_t instruction);
    static void dsra32(CPU* cpu, uint32_t instruction);

    static void bltz(CPU* cpu, uint32_t instruction);
    static void bgez(CPU* cpu, uint32_t instruction);
    static void bltzl(CPU* cpu, uint32_t instruction);
    static void bgezl(CPU* cpu, uint32_t instruction);

    static void tgei(CPU* cpu, uint32_t instruction);
    static void tgeiu(CPU* cpu, uint32_t instruction);
    static void tlti(CPU* cpu, uint32_t instruction);
    static void tltiu(CPU* cpu, uint32_t instruction);
    static void teqi(CPU* cpu, uint32_t instruction);

    static void tnei(CPU* cpu, uint32_t instruction);

    static void bltzal(CPU* cpu, uint32_t instruction);
    static void bgezal(CPU* cpu, uint32_t instruction);
    static void bltzall(CPU* cpu, uint32_t instruction);
    static void bgezall(CPU* cpu, uint32_t instruction);

};