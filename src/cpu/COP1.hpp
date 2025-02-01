#pragma once

#include "../bus/cop1/FCSR.hpp"
#include <vector>
#include <cstdint>
#include "CPUInstruction.hpp"

class CPU;

class COP1 {
public:

    FCSR fcsr;
    COP1();

    std::array<uint32_t, 32> fgr32 = {};
    std::array<uint64_t, 32> fgr64 = {};

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

    // TODO: rename these to camel case
    static void cop1_b_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_s_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_d_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_w_instrs(CPU* cpu, uint32_t instructon);
    static void cop1_l_instrs(CPU* cpu, uint32_t instructon);

    static void cvtSW(CPU* cpu, uint32_t instruction);
    static void cvtDW(CPU* cpu, uint32_t instruction);

    static void addS(CPU* cpu, uint32_t instruction);
    static void subS(CPU* cpu, uint32_t instruction);
    static void mulS(CPU* cpu, uint32_t instruction);
    static void divS(CPU* cpu, uint32_t instruction);
    static void sqrtS(CPU* cpu, uint32_t instruction);
    static void absS(CPU* cpu, uint32_t instruction);
    static void movS(CPU* cpu, uint32_t instruction);
    static void negS(CPU* cpu, uint32_t instruction);
    static void roundLS(CPU* cpu, uint32_t instruction);
    static void truncLS(CPU* cpu, uint32_t instruction);
    static void ceilLS(CPU* cpu, uint32_t instruction);
    static void floorLS(CPU* cpu, uint32_t instruction);
    static void roundWS(CPU* cpu, uint32_t instruction);
    static void truncWS(CPU* cpu, uint32_t instruction);
    static void ceilWS(CPU* cpu, uint32_t instruction);
    static void floorWS(CPU* cpu, uint32_t instruction);
    static void cvtDS(CPU* cpu, uint32_t instruction);
    static void cvtWS(CPU* cpu, uint32_t instruction);
    static void cvtLS(CPU* cpu, uint32_t instruction);
    static void CFS(CPU* cpu, uint32_t instruction);
    static void cUnS(CPU* cpu, uint32_t instruction);
    static void cEqS(CPU* cpu, uint32_t instruction);
    static void cUeqS(CPU* cpu, uint32_t instruction);
    static void cOltS(CPU* cpu, uint32_t instruction);
    static void cUltS(CPU* cpu, uint32_t instruction);
    static void cOleS(CPU* cpu, uint32_t instruction);
    static void cUleS(CPU* cpu, uint32_t instruction);
    static void cSfS(CPU* cpu, uint32_t instruction);
    static void cNgleS(CPU* cpu, uint32_t instruction);
    static void cSeqS(CPU* cpu, uint32_t instruction);
    static void cNglS(CPU* cpu, uint32_t instruction);
    static void cLtS(CPU* cpu, uint32_t instruction);
    static void cNgeS(CPU* cpu, uint32_t instruction);
    static void cLeS(CPU* cpu, uint32_t instruction);
    static void cNgtS(CPU* cpu, uint32_t instruction);

    void writeRegister(uint32_t index, uint64_t value);
    uint32_t readRegister(uint32_t index);

    std::array<CPUInstruction, 32> instructions;
    std::array<CPUInstruction, 64> sInstructions;
};