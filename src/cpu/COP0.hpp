#pragma once

#include <cstdint>
#include <vector>
#include "CPUInstruction.hpp"

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