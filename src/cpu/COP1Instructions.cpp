#pragma once

#include <cstdint>
#include <iostream>
#include "../bus/Bus.cpp"
#include "CPU.hpp"
#include <cmath>

void COP1::ldc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint64_t doubleWord = cpu->bus.memRead64(address);

    uint32_t index = CPU::getRt(instruction);

    if (!cpu->cop0.status.fr) {
        cpu->cop1.fgr32[index] = (uint32_t)doubleWord;
        cpu->cop1.fgr32[index + 1] = (uint32_t)(doubleWord >> 32);
    } else {
        cpu->cop1.fgr64[index] = doubleWord;
    }
}
void COP1::lwc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint32_t word = cpu->bus.memRead32(address);

    uint32_t index = CPU::getRt(instruction);

    if (!cpu->cop0.status.fr) {
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
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint64_t doubleWord = !cpu->cop0.status.fr ? (uint64_t)cpu->cop1.fgr32[rt] | ((uint64_t)cpu->cop1.fgr32[rt + 1] << 32) : cpu->cop1.fgr64[rt];

    cpu->bus.memWrite64(address, doubleWord);
}
void COP1::swc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint32_t word = !cpu->cop0.status.fr ? cpu->cop1.fgr32[rt] : (uint32_t)cpu->cop1.fgr64[rt];

    cpu->bus.memWrite32(address, word);
}

void COP1::cfc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }
    cpu->r[CPU::getRt(instruction)] = (int32_t)(int64_t)(uint64_t)cpu->cop1.readRegister(CPU::getRd(instruction));
}

void COP1::cop1_b_instrs(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    cpu->cop1.bInstructions[(instruction >> 16) & 0x3](cpu, instruction);
}

void COP1::cop1_d_instrs(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    cpu->cop1.dInstructions[instruction & 0x3f](cpu, instruction);
}

void COP1::cop1_l_instrs(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cop1_l_instrs\n";
    exit(1);
}

void COP1::cop1_s_instrs(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);

        cpu->enterException(true);
        return;
    }

    cpu->cop1.sInstructions[instruction & 0x3f](cpu, instruction);
}

void COP1::cop1_w_instrs(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
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
    if (!cpu->cop0.status.cu1) {
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
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
        cpu->enterException(true);
        return;
    }

    uint32_t returnVal;

    uint32_t rd = CPU::getRd(instruction);

    if (!cpu->cop0.status.fr) {
        returnVal = cpu->cop1.fgr32[rd];
    } else {
        returnVal = (uint32_t)cpu->cop1.fgr64[rd];
    }

    cpu->r[CPU::getRt(instruction)] = returnVal;
}

void COP1::mtc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
        cpu->enterException(true);
        return;
    }
    uint32_t value = (uint32_t)cpu->r[CPU::getRt(instruction)];
    uint32_t index = CPU::getRd(instruction);

    if (!cpu->cop0.status.fr) {
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

    if (cpu->cop0.status.fr) {
        value = (int32_t)cpu->cop1.fgr64[index];
    }

    uint32_t dest = CPU::getFd(instruction);

    int32_t destValue = ((union convi32){.f32 = (float)value}).i32;

    if (!cpu->cop0.status.fr) {
        cpu->cop1.fgr32[dest] = destValue;
    } else {
        cpu->cop1.fgr64[dest] = (uint64_t)destValue;
    }

    cpu->cop0.addCycles(4);
}

void COP1::addS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float float1 = cpu->cop1.getFloat(rd);
    float float2 = cpu->cop1.getFloat(rt);

    float result = float1 + float2;

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, result);

    cpu->cop0.addCycles(2);
}
void COP1::subS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float float1 = cpu->cop1.getFloat(rd);
    float float2 = cpu->cop1.getFloat(rt);

    float result = float1 - float2;

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, result);

    cpu->cop0.addCycles(2);
}
void COP1::mulS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float float1 = cpu->cop1.getFloat(rd);
    float float2 = cpu->cop1.getFloat(rt);

    float result = float1 * float2;

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, result);

    cpu->cop0.addCycles(4);
}
void COP1::divS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float numerator = cpu->cop1.getFloat(rd);
    float denominator = cpu->cop1.getFloat(rt);

    float result = numerator / denominator;

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, result);

    cpu->cop0.addCycles(28);
}
void COP1::sqrtS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);

    float value = cpu->cop1.getFloat(rd);

    float result = sqrt(value);

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, result);

    cpu->cop0.addCycles(28);
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
    uint32_t rd = CPU::getRd(instruction);

    float value = cpu->cop1.getFloat(rd);

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, -value);
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

    if (!cpu->cop0.status.fr) {
        bits = cpu->cop1.fgr32[rd];
    } else {
        bits = (uint32_t)cpu->cop1.fgr64[rd];
    }

    int32_t truncated = (int32_t)((convu32){.u32 = bits}).f32;

    float result = ((convi32){.i32 = truncated}).f32;

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, result);

    cpu->cop0.addCycles(4);
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
    uint32_t rd = CPU::getRd(instruction);

    uint32_t bits;

    uint32_t dest = CPU::getFd(instruction);

    if (!cpu->cop0.status.fr) {
        bits = cpu->cop1.fgr32[rd];

        cpu->cop1.fgr32[dest] = bits;
        cpu->cop1.fgr32[dest + 1] = (uint32_t)((uint64_t)bits >> 32);
    } else {
        bits = (uint32_t)cpu->cop1.fgr64[rd];

        cpu->cop1.fgr64[dest] = (uint64_t)bits;
    }

    cpu->cop0.addCycles(4);
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
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float float1 = cpu->cop1.getFloat(rd);
    float float2 = cpu->cop1.getFloat(rt);

    if (std::isnan(float1) || std::isnan(float2)) {
        cpu->cop1.fcsr.condition = 0;
    } else if (float1 == float2) {
        cpu->cop1.fcsr.condition = 1;
    } else {
        cpu->cop1.fcsr.condition = 0;
    }
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
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float float1 = cpu->cop1.getFloat(rd);
    float float2 = cpu->cop1.getFloat(rt);

    if (float1 < float2) {
        cpu->cop1.fcsr.condition = 1;
    } else {
        cpu->cop1.fcsr.condition = 0;
    }
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

void COP1::bc1f(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop1.fcsr.condition) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(CPU::getSignedImmediate(instruction) << 2);

        cpu->fastForwardRelativeLoop((int16_t)amount);

        cpu->nextPc = cpu->pc + amount;
    }
    cpu->inDelaySlot = true;
}
void COP1::bc1t(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bc1t\n";
    exit(1);
}
void COP1::bc1fl(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop1.fcsr.condition) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(CPU::getSignedImmediate(instruction) << 2);

        cpu->fastForwardRelativeLoop((int16_t)amount);

        cpu->nextPc = cpu->pc + amount;

        cpu->inDelaySlot = true;
    } else {
        cpu->discarded = true;
    }
}
void COP1::bc1tl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bc1tl\n";
    exit(1);
}

void COP1::addD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setDouble(dest, double1 + double2);

    cpu->cop0.addCycles(2);
}
void COP1::subD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setDouble(dest, double1 - double2);

    cpu->cop0.addCycles(2);
}
void COP1::mulD(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    double double1 = cpu->cop1.getDouble(rd);
    double double2 = cpu->cop1.getDouble(rt);

    uint32_t dest = CPU::getFd(instruction);

    double result = double1 * double2;

    cpu->cop1.setDouble(dest, result);

    cpu->cop0.addCycles(7);
}
void COP1::divD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: divD\n";
    exit(1);
}
void COP1::sqrtD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sqrtD\n";
    exit(1);
}
void COP1::absD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: absD\n";
    exit(1);
}
void COP1::movD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: movD\n";
    exit(1);
}
void COP1::negD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: negD\n";
    exit(1);
}
void COP1::roundLD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: roundLD\n";
    exit(1);
}
void COP1::truncLD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: truncLD\n";
    exit(1);
}
void COP1::ceilLD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ceilD\n";
    exit(1);
}
void COP1::floorLD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: floorLD\n";
    exit(1);
}
void COP1::roundWD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: roundWD\n";
    exit(1);
}
void COP1::truncWD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: truncWD\n";
    exit(1);
}
void COP1::ceilWD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ceilWD\n";
    exit(1);
}
void COP1::floorWD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: floorWD\n";
    exit(1);
}
void COP1::cvtSD(CPU* cpu, uint32_t instruction) {
    double result = cpu->cop1.getDouble(CPU::getRd(instruction));

    cpu->cop1.setFloat(CPU::getFd(instruction), (float)result);
}
void COP1::cvtWD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cvtWD\n";
    exit(1);
}
void COP1::cvtLD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cvtLD\n";
    exit(1);
}
void COP1::cFD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cFD\n";
    exit(1);
}
void COP1::cUnD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUnD\n";
    exit(1);
}
void COP1::cEqD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cEqD\n";
    exit(1);
}
void COP1::cUeqD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUeqD\n";
    exit(1);
}
void COP1::cOltD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cOltD\n";
    exit(1);
}
void COP1::cUltD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUltD\n";
    exit(1);
}
void COP1::cOleD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cOleD\n";
    exit(1);
}
void COP1::cUleD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cUleD\n";
    exit(1);
}
void COP1::cSfD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cSfD\n";
    exit(1);
}
void COP1::cNgleD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNgleD\n";
    exit(1);
}
void COP1::cSeqD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cSeqD\n";
    exit(1);
}
void COP1::cNglD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNglD\n";
    exit(1);
}
void COP1::cLtD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cLtD\n";
    exit(1);
}
void COP1::cNgeD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNgeD\n";
    exit(1);
}
void COP1::cLeD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    if (double1 <= double2) {
        cpu->cop1.fcsr.condition = 1;
    } else {
        cpu->cop1.fcsr.condition = 0;
    }
}
void COP1::cNgtD(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cNgtD\n";
    exit(1);
}