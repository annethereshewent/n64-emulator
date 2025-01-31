#pragma once

#include <cstdint>
#include <iostream>
#include "../bus/Bus.cpp"
#include "CPU.hpp"

// gotten from https://stackoverflow.com/questions/11611787/convert-a-32-bits-to-float-value
union convu32
{
    uint32_t u32; // here_write_bits
    float    f32; // here_read_float
};

union convi32
{
    int32_t i32; // here_write_bits
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
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint32_t word = cpu->bus.memRead32(address);

    uint32_t index = CPU::getRt(instruction);

    std::cout << "storing " << std::hex << word << " at index " << std::dec << index << "\n";

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[index] = word;
    } else {
        cpu->cop1.fgr64[index] = (cpu->cop1.fgr64[index] & 0xffffffff00000000) | (uint64_t)word;
    }

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

    cpu->cop1.sInstructions[instruction & 0x3f](cpu, instruction);
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
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
        cpu->enterException(true);
        return;
    }

    uint32_t returnVal;

    uint32_t rd = CPU::getRd(instruction);

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        returnVal = cpu->cop1.fgr32[rd];
    } else {
        returnVal = (uint32_t)cpu->cop1.fgr64[rd];
    }

    cpu->r[CPU::getRt(instruction)] = returnVal;
}

void COP1::mtc1(CPU* cpu, uint32_t instruction) {
    if (((cpu->cop0.status >> 29) & 0b1) == 0) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
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
                exit(1);
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

    int32_t destValue = ((union convi32){.f32 = (float)value}).i32;

    std::cout << "storing " << std::hex << destValue << " at index " << std::dec << dest << "\n";

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[dest] = destValue;
    } else {
        cpu->cop1.fgr64[dest] = (uint64_t)destValue;
    }

    // TODO: add cycles
}

void COP1::addS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    std:: cout << "rd = " << std::dec << rd << ", rt = " << rt << "\n";

    uint32_t op1;
    uint32_t op2;

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        op1 = cpu->cop1.fgr32[rd];
        op2 = cpu->cop1.fgr32[rt];
    } else {
        op1 = (uint32_t)cpu->cop1.fgr64[rd];
        op2 = (uint32_t)cpu->cop1.fgr64[rt];
    }

    float float1 = ((union convu32){.u32 = op1}).f32;
    float float2 = ((union convu32){.u32 = op2}).f32;

    std::cout << "adding " << float1 << " and " << float2 << " together\n";

    float result = float1 + float2;

    uint32_t returnVal = ((union convu32){.f32 = result }).u32;

    std::cout << "returning back " << std::hex << returnVal << "\n";

    uint32_t dest = CPU::getFd(instruction);

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[dest] = returnVal;
    } else {
        cpu->cop1.fgr64[dest] = (uint64_t)returnVal;
    }

    // TODO: add cycles
}
void COP1::subS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: subS\n";
    exit(1);
}
void COP1::mulS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    std:: cout << "rd = " << std::dec << rd << ", rt = " << rt << "\n";

    uint32_t op1;
    uint32_t op2;

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        op1 = cpu->cop1.fgr32[rd];
        op2 = cpu->cop1.fgr32[rt];
    } else {
        op1 = (uint32_t)cpu->cop1.fgr64[rd];
        op2 = (uint32_t)cpu->cop1.fgr64[rt];
    }

    float float1 = ((union convu32){.u32 = op1}).f32;
    float float2 = ((union convu32){.u32 = op2}).f32;

    std::cout << "float1 = " << float1 << ", float2 = " << float2 << "\n";

    float result = float1 * float2;

    std::cout << "result = " << result << "\n";

    uint32_t returnVal = ((convu32){.f32 = result }).u32;

    uint32_t dest = CPU::getFd(instruction);

    std::cout << "returning back " << std::hex << returnVal << " at index " << std::dec << dest << "\n";

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[dest] = returnVal;
    } else {
        cpu->cop1.fgr64[dest] = (uint64_t)returnVal;
    }

    // TODO: add cycles
}
void COP1::divS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    std:: cout << "rd = " << std::dec << rd << ", rt = " << rt << "\n";

    uint32_t numeratorBits;
    uint32_t denominatorBits;

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        numeratorBits = cpu->cop1.fgr32[rd];
        denominatorBits = cpu->cop1.fgr32[rt];
    } else {
        numeratorBits = (uint32_t)cpu->cop1.fgr64[rd];
        denominatorBits = (uint32_t)cpu->cop1.fgr64[rt];
    }

    float numerator = ((union convu32){.u32 = numeratorBits}).f32;
    float denominator = ((union convu32){.u32 = denominatorBits }).f32;

    float result = numerator / denominator;
    uint32_t returnVal = ((union convu32){.f32 = result }).u32;

    uint32_t dest = CPU::getFd(instruction);

    std::cout << "storing value " << std::hex << returnVal << " at index = " << std::dec << dest << "\n";

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[dest] = returnVal;
    } else {
        cpu->cop1.fgr64[dest] = (uint64_t)returnVal;
    }

    // TODO: add cycles
}
void COP1::sqrtS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sqrtS\n";
    exit(1);
}
void COP1::absS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: absS\n";
    exit(1);
}
void COP1::movS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: movS\n";
    exit(1);
}
void COP1::negS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: negS\n";
    exit(1);
}
void COP1::roundLS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: roundLS\n";
    exit(1);
}
void COP1::truncLS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: truncLS\n";
    exit(1);
}
void COP1::ceilLS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ceilLS\n";
    exit(1);
}
void COP1::floorLS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: floorLS\n";
    exit(1);
}
void COP1::roundWS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: roundWS\n";
    exit(1);
}
void COP1::truncWS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);

    uint32_t bits;

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        bits = cpu->cop1.fgr32[rd];
    } else {
        bits = (uint32_t)cpu->cop1.fgr64[rd];
    }

    int32_t truncated = (int32_t)((convu32){.u32 = bits}).f32;

    std::cout << "truncated val = " << std::dec << truncated << "\n";
    float result = ((convi32){.i32 = truncated}).f32;

    uint32_t returnVal = ((convu32){.f32 = result }).u32;

    uint32_t dest = CPU::getFd(instruction);

    std::cout << "returning " << std::hex << returnVal << " at index " << dest << "\n";

    if (((cpu->cop0.status >> 26) & 0b1) == 0) {
        cpu->cop1.fgr32[dest] = returnVal;
    } else {
        cpu->cop1.fgr64[dest] = (uint64_t)returnVal;
    }

}
void COP1::ceilWS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ceilWS\n";
    exit(1);
}
void COP1::floorWS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: floorWS\n";
    exit(1);
}
void COP1::cvtDS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cvtDS\n";
    exit(1);
}
void COP1::cvtWS(CPU* cpu, uint32_t instruction) {
    switch (cpu->cop1.fcsr.roundingMode) {
        case 0:
            roundWS(cpu, instruction);
            break;
        case 1:
            truncWS(cpu, instruction);
            break;
        case 2:
            ceilWS(cpu, instruction);
            break;
        case 3:
            floorWS(cpu, instruction);
            break;
        default:
            std::cout << "should be unreachable\n";
            exit(1);
            break;
    }
}
void COP1::cvtLS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cvtLS\n";
    exit(1);
}
void COP1::CFS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: CFS\n";
    exit(1);
}
void COP1::cUnS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUnS\n";
    exit(1);
}
void COP1::cEqS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cEqS\n";
    exit(1);
}
void COP1::cUeqS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUeqS\n";
    exit(1);
}

void COP1::cOltS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cOltS\n";
    exit(1);
}
void COP1::cUltS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUltS\n";
    exit(1);
}
void COP1::cOleS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cOleS\n";
    exit(1);
}
void COP1::cUleS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUleS\n";
    exit(1);
}
void COP1::cSfS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cSfS\n";
    exit(1);
}
void COP1::cNgleS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNgleS\n";
    exit(1);
}
void COP1::cSeqS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cSeqS\n";
    exit(1);
}
void COP1::cNglS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNglS\n";
    exit(1);
}
void COP1::cLtS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cLtS\n";
    exit(1);
}
void COP1::cNgeS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNgeS\n";
    exit(1);
}
void COP1::cLeS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cLeS\n";
    exit(1);
}
void COP1::cNgtS(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNgtS\n";
    exit(1);
}