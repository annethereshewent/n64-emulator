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

    auto [actualAddress, error, cached] = cpu->bus.translateAddress(address);

    if (error) {
        return;
    }

    uint64_t doubleWord = cpu->bus.memRead64(address, cached);

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

    auto [actualAddress, error, cached] = cpu->bus.translateAddress(address);

    if (error) {
        return;
    }

    uint32_t word = cpu->bus.memRead32(actualAddress, cached);

    uint32_t index = CPU::getRt(instruction);

    if (!cpu->cop0.status.fr) {
        cpu->cop1.fgr32[index] = word;
    } else {
        cpu->cop1.fgr64[index] = (cpu->cop1.fgr64[index] & 0xffffffff00000000) | (uint64_t)word;
    }

}
void COP1::reserved(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("instruction reserved for COP1\n");

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

    auto [actualAddress, error, cached] = cpu->bus.translateAddress(address, true);

    if (error) {
        return;
    }


    uint64_t doubleWord = !cpu->cop0.status.fr ? (uint64_t)cpu->cop1.fgr32[rt] | ((uint64_t)cpu->cop1.fgr32[rt + 1] << 32) : cpu->cop1.fgr64[rt];

    cpu->bus.memWrite64(actualAddress, doubleWord, cached);
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

    auto [actualAddress, error, cached] = cpu->bus.translateAddress(address, true);

    if (error) {
        return;
    }

    uint32_t word = !cpu->cop0.status.fr ? cpu->cop1.fgr32[rt] : (uint32_t)cpu->cop1.fgr64[rt];

    cpu->bus.memWrite32(actualAddress, word, cached);
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
    switch (instruction & 0x3f) {
        case 32:
            COP1::cvtSL(cpu, instruction);
            break;
        case 33:
            COP1::cvtDL(cpu, instruction);
            break;
        default:
            std::println("invalid option for l instrs given: {}", instruction & 0x3f);

            break;
    }
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
    throw std::runtime_error("TODO: dcfc1");
}

void COP1::dctc1(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: dctc1");
}

void COP1::dmfc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
        cpu->enterException(true);
        return;
    }

    uint64_t returnVal;

    uint32_t rd = CPU::getRd(instruction);

    if (!cpu->cop0.status.fr) {
        returnVal = (uint64_t)cpu->cop1.fgr32[rd] | (uint64_t)cpu->cop1.fgr32[rd + 1] << 32;
    } else {
        returnVal = cpu->cop1.fgr64[rd];
    }

    cpu->r[CPU::getRt(instruction)] = returnVal;
}

void COP1::dmtc1(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop0.status.cu1) {
        cpu->cop0.cause = (11 << 2) | (1 << 28);
        cpu->enterException(true);
        return;
    }
    uint64_t value = cpu->r[CPU::getRt(instruction)];
    uint32_t index = CPU::getRd(instruction);

    if (!cpu->cop0.status.fr) {
        cpu->cop1.fgr32[index] = (uint32_t)value;
        cpu->cop1.fgr32[index + 1] = (uint32_t)(value >> 32);
    } else {
        cpu->cop1.fgr64[index] = value;
    }
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

    cpu->r[CPU::getRt(instruction)] = (int32_t)(int64_t)(uint32_t)returnVal;
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
                throw std::runtime_error("lmao im supposed to throw some sort of floating point exception\n");
            }
            break;
        }
        default:
            throw std::runtime_error("invalid index given: " + std::to_string(index));
            break;
    }
}

uint32_t COP1::readRegister(uint32_t index) {
    switch (index) {
        case 0:
            return 0xa00;
            break;
        case 0x1f:
            return fcsr.value;
            break;
        default:
            throw std::runtime_error("invalid index given for COP1::readRegister: " + std::to_string(index));
            break;
    }
}

void COP1::cvtDW(CPU* cpu, uint32_t instruction) {
    uint32_t index = CPU::getRd(instruction);

    int32_t value = (int32_t)cpu->cop1.fgr32[index];

    if (cpu->cop0.status.fr) {
        value = (int32_t)cpu->cop1.fgr64[index];
    }

    cpu->cop1.setDouble(CPU::getFd(instruction), (double)value);
    cpu->cop0.addCycles(4);
}

void COP1::cvtSW(CPU* cpu, uint32_t instruction) {
    uint32_t index = CPU::getRd(instruction);

    int32_t value = (int32_t)cpu->cop1.fgr32[index];

    if (cpu->cop0.status.fr) {
        value = (int32_t)cpu->cop1.fgr64[index];
    }

    cpu->cop1.setFloat(CPU::getFd(instruction), (float)value);

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
    float value = cpu->cop1.getFloat(CPU::getRd(instruction));

    cpu->cop1.setFloat(CPU::getFd(instruction), std::abs(value));
    cpu->cop0.addCycles(2);
}
void COP1::movS(CPU* cpu, uint32_t instruction) {
    double result = cpu->cop1.getDouble(CPU::getRd(instruction));
    cpu->cop1.setDouble(CPU::getFd(instruction), result);
}
void COP1::negS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);

    float value = cpu->cop1.getFloat(rd);

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setFloat(dest, -value);
}
void COP1::roundLS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: roundLS");
}
void COP1::truncLS(CPU* cpu, uint32_t instruction) {
    float value = cpu->cop1.getFloat(CPU::getRd(instruction));

    int64_t intValue = (int64_t)trunc(value);

    double returnValue = ((convi64){.i64 = intValue}).f64;

    cpu->cop1.setDouble(CPU::getFd(instruction), returnValue);
    cpu->cop0.addCycles(4);
}
void COP1::ceilLS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: ceilLS");
}
void COP1::floorLS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: floorLS");
}
void COP1::roundWS(CPU* cpu, uint32_t instruction) {
    float value = cpu->cop1.getFloat(CPU::getRd(instruction));

    int32_t valueI32 = (int32_t)(round(value / 2.0) * 2.0);

    float returnValue = ((convi32){.i32 = valueI32}).f32;

    cpu->cop1.setFloat(CPU::getFd(instruction), returnValue);
    cpu->cop0.addCycles(4);
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
    throw std::runtime_error("TODO: ceilWS");
}
void COP1::floorWS(CPU* cpu, uint32_t instruction) {
    float value = cpu->cop1.getFloat(CPU::getRd(instruction));

    int32_t intVal = (int32_t)std::floor(value);

    float result = ((convi32){.i32 = intVal}).f32;

    cpu->cop1.setFloat(CPU::getFd(instruction), result);

    cpu->cop0.addCycles(4);
}
void COP1::cvtDS(CPU* cpu, uint32_t instruction) {
    float result = cpu->cop1.getFloat(CPU::getRd(instruction));

    cpu->cop1.setDouble(CPU::getFd(instruction), (double)result);

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
            throw std::runtime_error("should be unreachable\n");

            break;
    }
}
void COP1::cvtLS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cvtLS");
}
void COP1::CFS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: CFS");
}
void COP1::cUnS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUnS");
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
    throw std::runtime_error("TODO: cUeqS");
}

void COP1::cOltS(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    if (std::isnan(double1) || std::isnan(double2)) {
        cpu->cop1.fcsr.condition = 0;
        return;
    }

    cpu->cop1.fcsr.condition = (uint8_t)(double1 < double2);
}
void COP1::cUltS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUltS");
}
void COP1::cOleS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cOleS");
}
void COP1::cUleS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUleS");
}
void COP1::cSfS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cSfS");
}
void COP1::cNgleS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cNgleS");
}
void COP1::cSeqS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cSeqS");
}
void COP1::cNglS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cNglS");
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
    throw std::runtime_error("TODO: cNgeS");
}
void COP1::cLeS(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    float float1 = cpu->cop1.getFloat(rd);
    float float2 = cpu->cop1.getFloat(rt);

    if (float1 <= float2) {
        cpu->cop1.fcsr.condition = 1;
    } else {
        cpu->cop1.fcsr.condition = 0;
    }
}
void COP1::cNgtS(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cNgtS");
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
    if (cpu->cop1.fcsr.condition) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(CPU::getSignedImmediate(instruction) << 2);

        cpu->fastForwardRelativeLoop((int16_t)amount);

        cpu->nextPc = cpu->pc + amount;
    }
    cpu->inDelaySlot = true;
}
void COP1::bc1fl(CPU* cpu, uint32_t instruction) {
    if (!cpu->cop1.fcsr.condition) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(CPU::getSignedImmediate(instruction) << 2);

        cpu->fastForwardRelativeLoop((int16_t)amount);

        cpu->nextPc = cpu->pc + amount;

        cpu->inDelaySlot = true;
    } else {
        cpu->pc = cpu->nextPc;
        cpu->discarded = true;
    }
}
void COP1::bc1tl(CPU* cpu, uint32_t instruction) {
    if (cpu->cop1.fcsr.condition) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(CPU::getSignedImmediate(instruction) << 2);

        cpu->fastForwardRelativeLoop((int16_t)amount);

        cpu->nextPc = cpu->pc + amount;

        cpu->inDelaySlot = true;
    } else {
        cpu->pc = cpu->nextPc;
        cpu->discarded = true;
    }
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
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    cpu->cop1.setDouble(CPU::getFd(instruction), double1 / double2);

    cpu->cop0.addCycles(57);
}
void COP1::sqrtD(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);

    double value = cpu->cop1.getDouble(rd);

    double result = sqrt(value);

    uint32_t dest = CPU::getFd(instruction);

    cpu->cop1.setDouble(dest, result);

    cpu->cop0.addCycles(57);
}
void COP1::absD(CPU* cpu, uint32_t instruction) {
    double value = cpu->cop1.getDouble(CPU::getRd(instruction));

    cpu->cop1.setDouble(CPU::getFd(instruction), std::abs(value));
    cpu->cop0.addCycles(2);
}
void COP1::movD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));

    cpu->cop1.setDouble(CPU::getFd(instruction), double1);
}
void COP1::negD(CPU* cpu, uint32_t instruction) {
    double value = cpu->cop1.getDouble(CPU::getRd(instruction));

    cpu->cop1.setDouble(CPU::getFd(instruction), -value);
}
void COP1::roundLD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: roundLD");
}
void COP1::truncLD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: truncLD");
}
void COP1::ceilLD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: ceilD");
}
void COP1::floorLD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: floorLD");
}
void COP1::roundWD(CPU* cpu, uint32_t instruction) {
    double value = cpu->cop1.getDouble(CPU::getRd(instruction));

    int32_t valueI32 = (int32_t)(round(value / 2.0) * 2.0);

    float returnValue = ((convi32){.i32 = valueI32}).f32;

    cpu->cop1.setFloat(CPU::getFd(instruction), returnValue);
    cpu->cop0.addCycles(4);
}
void COP1::truncWD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));

    float result = ((union convi32){.i32 = (int32_t)double1}).f32;

    cpu->cop1.setFloat(CPU::getFd(instruction), result);

    cpu->cop0.addCycles(4);
}
void COP1::ceilWD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: ceilWD");
}
void COP1::floorWD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: floorWD");
}
void COP1::cvtSD(CPU* cpu, uint32_t instruction) {
    double result = cpu->cop1.getDouble(CPU::getRd(instruction));

    cpu->cop1.setFloat(CPU::getFd(instruction), (float)result);

    cpu->cop0.addCycles(4);
}
void COP1::cvtWD(CPU* cpu, uint32_t instruction) {
    switch (cpu->cop1.fcsr.roundingMode) {
        case 0:
            COP1::roundWD(cpu, instruction);
            break;
        case 1:
            COP1::truncWD(cpu, instruction);
            break;
        case 2:
            COP1::ceilWD(cpu, instruction);
            break;
        case 3:
            COP1::floorWD(cpu, instruction);
            break;
    }
}
void COP1::cvtLD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cvtLD");
}
void COP1::cFD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cFD");
}
void COP1::cUnD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUnD");
}
void COP1::cEqD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    if (std::isnan(double1) || std::isnan(double2)) {
        cpu->cop1.fcsr.condition = 0;
        return;
    }

    if (double1 == double2) {
        cpu->cop1.fcsr.condition = 1;
    } else {
        cpu->cop1.fcsr.condition = 0;
    }
}
void COP1::cUeqD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUeqD");
}
void COP1::cOltD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cOltD");
}
void COP1::cUltD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUltD");
}
void COP1::cOleD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cOleD");
}
void COP1::cUleD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cUleD");
}
void COP1::cSfD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cSfD");
}
void COP1::cNgleD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cNgleD");
}
void COP1::cSeqD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cSeqD");
}
void COP1::cNglD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cNglD");
}
void COP1::cLtD(CPU* cpu, uint32_t instruction) {
    double double1 = cpu->cop1.getDouble(CPU::getRd(instruction));
    double double2 = cpu->cop1.getDouble(CPU::getRt(instruction));

    if (double1 < double2) {
        cpu->cop1.fcsr.condition = 1;
    } else {
        cpu->cop1.fcsr.condition = 0;
    }
}
void COP1::cNgeD(CPU* cpu, uint32_t instruction) {
    throw std::runtime_error("TODO: cNgeD");
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
    throw std::runtime_error("TODO: cNgtD");
}
void COP1::cvtDL(CPU* cpu, uint32_t instruction) {
    int64_t value;

    uint32_t rd = CPU::getRd(instruction);

    if (!cpu->cop0.status.fr) {
        value = (int64_t)((uint64_t)cpu->cop1.fgr32[rd] | (uint64_t)cpu->cop1.fgr32[rd + 1] << 32);
    } else {
        value = (int64_t)cpu->cop1.fgr64[rd];
    }

    cpu->cop1.setDouble(CPU::getFd(instruction), (double)value);
    cpu->cop0.addCycles(4);
}

void COP1::cvtSL(CPU* cpu, uint32_t instruction) {
    int64_t value;

    uint32_t rd = CPU::getRd(instruction);

    if (!cpu->cop0.status.fr) {
        value = (int64_t)((uint64_t)cpu->cop1.fgr32[rd] | (uint64_t)cpu->cop1.fgr32[rd + 1] << 32);
    } else {
        value = (int64_t)cpu->cop1.fgr64[rd];
    }

    cpu->cop1.setFloat(CPU::getFd(instruction), (float)value);
    cpu->cop0.addCycles(4);
}