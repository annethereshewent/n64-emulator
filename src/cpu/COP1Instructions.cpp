#pragma once

#include <cstdint>
#include <iostream>
#include "../bus/Bus.cpp"
#include "CPU.hpp"

// gotten from https://stackoverflow.com/questions/11611787/convert-a-32-bits-to-float-value
union conv32
{
    uint32_t u32; // here_write_bits
    float    f32; // here_read_float
};

void COP1::ldc1(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint64_t doubleWord = cpu->bus.memRead64(address);

    std::cout << "storing doubleWord " << std::hex << doubleWord << "\n";

    uint32_t index = CPU::getRt(instruction);

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[index] = (uint32_t)doubleWord;
        cpu->cop1.fgr32[index + 1] = (uint32_t)(doubleWord >> 32);
    } else {
        cpu->cop1.fgr64[index] = doubleWord;
    }
}
void COP1::lwc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lwc1\n";
    exit(1);
}
void COP1::reserved(CPU* cpu, uint32_t instruction) {
    std::cout << "instruction reserved for COP1\n";
    exit(1);
}
void COP1::sdc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sdc1\n";
    exit(1);
}
void COP1::swc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: swc1\n";
    exit(1);
}

void COP1::cfc1(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }
    cpu->r[CPU::getRt(instruction)] = (int32_t)(int64_t)(uint64_t)cpu->cop1.readRegister(CPU::getRd(instruction));
}

void COP1::cop1_b_instrs(CPU* cpu, uint32_t instructon) {
    std::cout << "TODO: cop1_b_instrs\n";
    exit(1);
}

void COP1::cop1_d_instrs(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cop1_d_instrs\n";
    exit(1);
}

void COP1::cop1_l_instrs(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cop1_l_instrs\n";
    exit(1);
}

void COP1::cop1_s_instrs(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }
}

void COP1::cop1_w_instrs(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    switch (instruction & 0x3f) {
        case 32:
            COP1::cvtSW(cpu, instruction);
            break;
        case 33:
            COP1::cvtDW(cpu, instruction);
            break;
        default: {
            COP1::reserved(cpu, instruction);
            break;
        }
    }
}

void COP1::ctc1(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t value = cpu->r[CPU::getRt(instruction)];
    uint32_t index = CPU::getRd(instruction);

    cpu->cop1.writeRegister(index, value);
}

void COP1::dcfc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dcfc1\n";
    exit(1);
}

void COP1::dctc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dctc1\n";
    exit(1);
}

void COP1::dmfc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dmfc1\n";
    exit(1);
}

void COP1::dmtc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dmtc1\n";
    exit(1);
}

void COP1::mfc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: mfc1\n";
    exit(1);
}

void COP1::mtc1(CPU* cpu, uint32_t instruction) {
    if (Bus::translateAddress(cpu->previousPc) == 0x325978) {
        std::cout << "we're at the address!!!!!!\n";
    }
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
        std::cout << "entering an exception :-(\n";
        cpu->enterException(true);
        return;
    }
    uint32_t value = (uint32_t)cpu->r[CPU::getRt(instruction)];
    uint32_t index = CPU::getRd(instruction);

    std::cout << "storing value " << std::hex << value << " at index " << std::dec << index << "\n";
    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[index] = value;
    } else {
        cpu->cop1.fgr64[index] = (cpu->cop1.fgr64[index] & 0xffffffff00000000) | (uint64_t)value;
    }
}

void COP1::writeRegister(uint32_t index, uint64_t value) {
    switch (index) {
        case 0:
            break;
        case 0x1f: {
            fcsr.value = (uint32_t)value;
            fcsr.unused = 0;

            uint32_t enableBits = (fcsr.value >> 7) & 0x1f;

            uint32_t causeBits = (fcsr.value >> 12) & 0x1f;

            if (enableBits & causeBits || fcsr.caseUnimplemented) {
                std::cout << "lmao im supposed to throw some sort of floating point exception\n";
            }
            break;
        }
        default:
            std::cout << "invalid index given: " << index << "\n";
            exit(1);
            break;
    }
}

uint32_t COP1::readRegister(uint32_t index) {
    switch (index) {
        case 0:
            return 0b101000000000;
            break;
        case 0x1f:
            return fcsr.value;
            break;
        default:
            std::cout << "invalid index given for COP1::readRegister: " << index << "\n";
            exit(1);
            break;
    }
}

void COP1::cvtDW(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cvtDW\n";
    exit(1);
}

void COP1::cvtSW(CPU* cpu, uint32_t instruction) {
    uint32_t index = CPU::getRd(instruction);

    int32_t value = (int32_t)cpu->cop1.fgr32[index];

    if (((cpu->cop0.status >> 26) & 0b1) == 1) {
        value = (int32_t)cpu->cop1.fgr64[index];
    }

    uint32_t dest = CPU::getFd(instruction);

    uint32_t destValue = ((union conv32){.f32 = (float)value}).u32;

    std::cout << "got destValue " << std::hex << destValue << "\n";

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[dest] = destValue;
    } else {
        cpu->cop1.fgr64[dest] = (cpu->cop1.fgr64[dest] & 0xffffffff00000000) | (uint64_t)destValue;
    }
}