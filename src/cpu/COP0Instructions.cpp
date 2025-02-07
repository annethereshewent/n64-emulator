#pragma once

#include <cstdint>
#include <iostream>
#include "../bus/Bus.cpp"
#include "CPU.hpp"

void COP0::reserved(CPU* cpu, uint32_t instruction) {
    std::cout << "instruction reserved for COP0\n";
    exit(1);
}

void COP0::cp0(CPU* cpu, uint32_t instruction) {
   cpu->cop0.cp0Instructions[instruction & 0x3f](cpu, instruction);
}

void COP0::dmfc0(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dmfc0\n";
    exit(1);
}

void COP0::dmtc0(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dmtc0\n";
    exit(1);
}

void COP0::mfc0(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    cpu->r[rt] = cpu->cop0.readRegister(rd);
}

void COP0::mtc0(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    cpu->cop0.writeRegister(rd, (int32_t)(int64_t)(uint64_t)cpu->r[rt], &cpu->scheduler);

    cpu->checkIrqs();
}

void COP0::break_(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: break\n";
    exit(1);
}

void COP0::syscall(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: syscall";
    exit(1);
}

void COP0::tlbp(CPU* cpu, uint32_t instruction) {
    cpu->bus.tlbProbe();
}

void COP0::tlbr(CPU* cpu, uint32_t instruction) {
    cpu->bus.tlbRead(cpu->cop0.index);
}

void COP0::tlbwi(CPU* cpu, uint32_t instruction) {
    cpu->bus.tlbWrite(cpu->cop0.index);
}

void COP0::tlbwr(CPU* cpu, uint32_t instruction) {
    exit(1);
}

void COP0::eret(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 2) & 0b1) == 1) {
        cpu->pc = cpu->cop0.errorEpc;
        cpu->nextPc = cpu->pc + 4;
        cpu->cop0.status &= ~(1 << 2);
    } else {
        cpu->pc = cpu->cop0.epc;
        cpu->nextPc = cpu->pc + 4;
        cpu->cop0.status &= ~(1 << 1);
    }

    cpu->llbit = false;

    cpu->checkIrqs();
}