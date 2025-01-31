#pragma once

#include <cstdint>
#include <iostream>
#include "../bus/Bus.cpp"
#include "CPU.hpp"

typedef unsigned __int128 u128;

void CPU::set(int i, uint64_t val) {
    if (i != 0) {
        r[i] = val;
    }
}

void CPU::lui(CPU* cpu, uint32_t instruction) {

    uint32_t immediate = getImmediate(instruction);

    int reg = getRt(instruction);

    uint64_t value = (int32_t)(int64_t)(uint64_t)(immediate << 16);

    cpu->r[reg] = value;
}

void CPU::addi(CPU* cpu, uint32_t instruction) {
    CPU::addiu(cpu, instruction);
}

void CPU::addiu(CPU* cpu, uint32_t instruction) {

    uint64_t immediate = getSignedImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    cpu->r[rt] = (int32_t)(int64_t)(uint64_t)cpu->r[rs] + immediate;
}

void CPU::andi(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = getImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    cpu->r[rt] = cpu->r[rs] & immediate;
}
void CPU::beq(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint32_t immediate = getImmediate(instruction);

    if (cpu->r[rs] == cpu->r[rt]) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
    }
}
void CPU::beql(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint32_t immediate = getImmediate(instruction);
    if (cpu->r[rs] == cpu->r[rt]) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
   } else {
        cpu->pc = cpu->nextPc;
        cpu->discarded = true;
   }
}
void CPU::bgtz(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bgtz\n";
    exit(1);
}
void CPU::bgtzl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bgtzl\n";
    exit(1);
}
void CPU::blez(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);

    if ((int64_t)cpu->r[rs] <= 0) {
        uint32_t immediate = getImmediate(instruction);

        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
    }
}
void CPU::blezl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: blezl\n";
    exit(1);
}
void CPU::bne(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint32_t immediate = getImmediate(instruction);

    if (cpu->r[rs] != cpu->r[rt]) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
   }
}
void CPU::bnel(CPU* cpu, uint32_t instruction) {

    uint32_t immediate = getImmediate(instruction);
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    if (cpu->r[rs] != cpu->r[rt]) {
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
   } else {
        cpu->pc = cpu->nextPc;
        cpu->discarded = true;
   }
}
void CPU::cache(CPU* cpu, uint32_t instruction) {
    uint32_t cacheOp = getRt(instruction);

    uint64_t offset = getSignedImmediate(instruction);
    uint32_t base = getRs(instruction);

    uint64_t address = cpu->r[base] + offset;

    uint64_t actualAddress = Bus::translateAddress(address);

    switch (cacheOp) {
        case 0x0: {
            uint64_t line = (actualAddress >> 5) & 0x1ff;
            cpu->bus.icache[line].valid = false;
            break;
        }
        case 0x1: {
            // write-back invalidate of dcachce
            uint64_t line = (actualAddress >> 4) & 0x1ff;

            if (cpu->bus.dcache[line].dirty && cpu->bus.dcache[line].valid) {
                // do the writeback
                cpu->bus.dcacheWriteback(line);
            }
            cpu->bus.dcache[line].valid = false;

            break;
        }
        case 0x8: {
            uint64_t line = (actualAddress >> 5) & 0x1ff;
            bool isValid = (cpu->cop0.tagLo >> 7 & 0b1) == 1;
            uint32_t tag = (cpu->cop0.tagLo & 0xFFFFF00) << 4;

            cpu->bus.icache[line].valid = isValid;
            cpu->bus.icache[line].tag = tag;
            break;
        }
        case 0x10: {
            uint64_t line = (actualAddress >> 5) & 0x1ff;
            ICache* icachePtr = &cpu->bus.icache[line];

            if (icachePtr[0].valid && (icachePtr[0].tag & 0x1ffffffc) == (uint32_t)(address & ~0xfff)) {
                icachePtr[0].valid = false;
            }
            break;
        }
        case 0x19: {
            uint64_t line = (actualAddress >> 4) & 0x1ff;

            if (cpu->bus.dcache[line].valid && (cpu->bus.dcache[line].tag & 0x1ffffffc) == (uint32_t)(address & ~0xfff) && cpu->bus.dcache[line].dirty) {
                cpu->bus.dcacheWriteback(line);
            }
            break;
        }
        default: {
            std::cout << "cache op not yet implemented: " << std::hex << cacheOp << "\n";
            exit(1);
            break;
        }
    }
}
void CPU::daddi(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: daddi\n";
    exit(1);
}
void CPU::daddiu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: daddiu\n";
    exit(1);
}
void CPU::j(CPU* cpu, uint32_t instruction) {
    uint32_t offset = (instruction & 0x3ffffff) << 2;

    uint32_t address = (cpu->pc & 0xfffffffff0000000) | offset;

    cpu->nextPc = address;
}
void CPU::jal(CPU* cpu, uint32_t instruction) {
    // TODO: check if this is correct. some other emulators
    // do some stuff like check if delay slot is taken and
    // only branch if thats not the case but idk if i need
    // to do that with my code?
    cpu->r[31] = cpu->nextPc;

    uint32_t offset = (instruction & 0x3ffffff) << 2;

    uint32_t address = (cpu->pc & 0xfffffffff0000000) | offset;

    cpu->nextPc = address;
}
void CPU::lb(CPU* cpu, uint32_t instruction) {
    uint64_t offset = getSignedImmediate(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t base = getRs(instruction);

    uint32_t address = cpu->r[base] + offset;

    uint64_t value = (int8_t)(int64_t)(uint64_t)cpu->bus.memRead8(address);

    cpu->r[rt] = value;
}
void CPU::lbu(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = getSignedImmediate(instruction);
    uint32_t baseReg = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    cpu->r[rt] = (uint64_t)cpu->bus.memRead8(address);
}
void CPU::ld(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = getSignedImmediate(instruction);
    uint32_t baseReg = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    cpu->r[rt] = cpu->bus.memRead64(address);
}
void CPU::ldl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ldl\n";
    exit(1);
}
void CPU::ldr(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ldr\n";
    exit(1);
}
void CPU::lh(CPU* cpu, uint32_t instruction) {
    uint64_t offset = getSignedImmediate(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t base = getRs(instruction);

    uint32_t address = cpu->r[base] + offset;

    uint64_t value = (int16_t)(int64_t)(uint64_t)cpu->bus.memRead16(address);

    cpu->r[rt] = value;
}
void CPU::lhu(CPU* cpu, uint32_t instruction) {
    uint64_t offset = getSignedImmediate(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t base = getRs(instruction);

    uint32_t address = cpu->r[base] + offset;

    uint64_t value = (uint64_t)cpu->bus.memRead16(address);

    cpu->r[rt] = value;
}
void CPU::ll(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ll\n";
    exit(1);
}
void CPU::lld(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lld\n";
    exit(1);
}
void CPU::lw(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = getSignedImmediate(instruction);
    uint32_t baseReg = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint64_t address = cpu->r[baseReg] + immediate;

    uint32_t value = cpu->bus.memRead32(address);

    cpu->r[rt] = (int32_t)(int64_t)(uint64_t)value;
}
void CPU::lwl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lwl\n";
    exit(1);
}
void CPU::lwr(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lwr\n";
    exit(1);
}
void CPU::lwu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lwu\n";
    exit(1);
}
void CPU::ori(CPU* cpu, uint32_t instruction) {
    uint32_t imm = getImmediate(instruction);

    uint32_t rt = getRt(instruction);
    uint32_t rs = getRs(instruction);

    cpu->r[rt] = cpu->r[rs] | imm;
}
void CPU::reserved(CPU* cpu, uint32_t instruction) {
    std::cout << "Error: opcode reserved\n";
    exit(1);
}
void CPU::sb(CPU* cpu, uint32_t instruction) {
    uint8_t byte = (uint8_t)cpu->r[getRt(instruction)];

    uint64_t immediate = getSignedImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint64_t address = immediate + cpu->r[rs];

    cpu->bus.memWrite8(address, byte);
}
void CPU::sc(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sc\n";
    exit(1);
}
void CPU::scd(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: scd\n";
    exit(1);
}
void CPU::sd(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = getSignedImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint64_t address = immediate + cpu->r[rs];

    cpu->bus.memWrite64(address, cpu->r[getRt(instruction)]);
}
void CPU::sdl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sdl\n";
    exit(1);
}
void CPU::sdr(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sdr\n";
    exit(1);
}
void CPU::sh(CPU* cpu, uint32_t instruction) {
    uint8_t half = (uint16_t)cpu->r[getRt(instruction)];

    uint64_t immediate = getSignedImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint64_t address = immediate + cpu->r[rs];

    cpu->bus.memWrite16(address, half);
}
void CPU::slti(CPU* cpu, uint32_t instruction) {
    int64_t immediate = (int16_t)(int64_t)getImmediate(instruction);

    uint64_t val = 0;
    if ((int64_t)cpu->r[getRs(instruction)] < immediate) {
        val = 1;
    }

    cpu->r[getRt(instruction)] = val;
}
void CPU::sltiu(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = getSignedImmediate(instruction);
    uint64_t val = 0;

    if (cpu->r[getRs(instruction)] < immediate) {
        val = 1;
    }
    cpu->r[getRt(instruction)] = val;
}
void CPU::sw(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = getSignedImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint64_t address = immediate + cpu->r[rs];

    cpu->bus.memWrite32(address, (uint32_t)cpu->r[rt]);
}
void CPU::swl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: swl\n";
    exit(1);
}
void CPU::swr(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: swr\n";
    exit(1);
}
void CPU::xori(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: xori\n";
    exit(1);
}

void CPU::sll(CPU* cpu, uint32_t instruction) {
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);
    uint32_t shift = shiftAmount(instruction);

    cpu->r[rd] = cpu->r[rt] << shift;
}
void CPU::srl(CPU* cpu, uint32_t instruction) {
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);
    uint32_t shift = shiftAmount(instruction);

    cpu->r[rd] = (int32_t)(uint32_t)(uint64_t)(cpu->r[rt] >> shift);
}
void CPU::sra(CPU* cpu, uint32_t instruction) {
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);
    uint32_t shift = shiftAmount(instruction);

    cpu->r[rd] = (int32_t)(int64_t)(uint64_t)((int64_t)cpu->r[rt] >> shift);
}
void CPU::sllv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x1f;


    cpu->r[rd] = (int32_t)(int64_t)(uint64_t)((uint32_t)cpu->r[rt] << shift);
}
void CPU::srlv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x1f;

    cpu->r[rd] = (int32_t)(int64_t)(uint64_t)((uint32_t)cpu->r[rt] >> shift);
}
void CPU::srav(CPU* cpu, uint32_t instruction) {

    exit(1);
}
void CPU::jr(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);

    cpu->nextPc = cpu->r[rs];
}
void CPU::jalr(CPU* cpu, uint32_t instruction) {
    // TODO: see jal comments
    cpu->r[getRd(instruction)] = cpu->nextPc;

    cpu->nextPc = cpu->r[getRs(instruction)];
}
void CPU::sync(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sync\n";
    exit(1);
}
void CPU::mfhi(CPU* cpu, uint32_t instruction) {
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = cpu->hi;
}
void CPU::mthi(CPU* cpu, uint32_t instruction) {
    cpu->hi = cpu->r[getRs(instruction)];
}
void CPU::mflo(CPU* cpu, uint32_t instruction) {
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = cpu->lo;
}
void CPU::mtlo(CPU* cpu, uint32_t instruction) {
    cpu->lo = cpu->r[getRs(instruction)];
}
void CPU::dsllv(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsllv\n";
    exit(1);
}
void CPU::dsrlv(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsrlv\n";
    exit(1);
}
void CPU::dsrav(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsrav\n";
    exit(1);
}
void CPU::mult(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: mult\n";
    exit(1);
}
void CPU::multu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint64_t val1 = (uint32_t)(uint64_t)cpu->r[rs];
    uint64_t val2 = (uint32_t)(uint64_t)cpu->r[rt];

    uint64_t result = val1 * val2;


    cpu->lo = (int32_t)(int64_t)(uint64_t)result;

    cpu->hi = (int32_t)(int64_t)(uint64_t)(result >> 32);
}
void CPU::div(CPU* cpu, uint32_t instruction) {
    uint64_t numerator = (int32_t)(int64_t)(uint64_t)cpu->r[getRs(instruction)];
    uint64_t denominator = (int32_t)(int64_t)(uint64_t)cpu->r[getRt(instruction)];

    if (denominator != 0) {
        cpu->lo = numerator / denominator;
        cpu->hi = numerator % denominator;
    } else {
        if ((int32_t)numerator < 0) {
            cpu->lo = 1;
        } else {
            cpu->lo = 0xffffffffffffffff;
        }
        cpu->hi = numerator;
    }

    // TODO: add cycles, do this for all div/mult operations
}
void CPU::divu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: divu\n";
    exit(1);
}
void CPU::dmult(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dmult\n";
    exit(1);
}
void CPU::dmultu(CPU* cpu, uint32_t instruction) {
    u128 result = (u128)cpu->r[getRs(instruction)] * (u128)cpu->r[getRt(instruction)];

    cpu->lo = (uint64_t)result;
    cpu->hi = (uint64_t)(result >> 64);
}
void CPU::ddiv(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ddiv\n";
    exit(1);
}
void CPU::ddivu(CPU* cpu, uint32_t instruction) {
    uint64_t numerator = cpu->r[getRs(instruction)];
    uint64_t denominator = cpu->r[getRt(instruction)];

    if (denominator != 0) {
        cpu->lo = numerator / denominator;
        cpu->hi = numerator % denominator;
    } else {
        cpu->lo = 0xffffffffffffffff;
        cpu->hi = numerator;
    }
    // TODO: add cycles here
}
void CPU::add(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = (int32_t)(int64_t)(uint64_t)((uint32_t)cpu->r[rt] + (uint32_t)cpu->r[rs]);

    // TODO: possibly add integer overflow exception
}
void CPU::addu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = (int32_t)(int64_t)(uint64_t)((uint32_t)cpu->r[rs] + (uint32_t)cpu->r[rt]);
}
void CPU::sub(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sub\n";
    exit(1);
}
void CPU::subu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = cpu->r[rs] - cpu->r[rt];
}
void CPU::and_(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = cpu->r[rs] & cpu->r[rt];
}
void CPU::or_(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = cpu->r[rs] | cpu->r[rt];
}
void CPU::xor_(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    cpu->r[rd] = cpu->r[rs] ^ cpu->r[rt];
}
void CPU::nor(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: nor\n";
    exit(1);
}
void CPU::slt(CPU* cpu, uint32_t instruction) {
    uint64_t value = 0;

    if ((int64_t)cpu->r[getRs(instruction)] < (int64_t)cpu->r[getRt(instruction)]) {
        value = 1;
    }

    cpu->r[getRd(instruction)] = value;
}
void CPU::sltu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);
    uint32_t rd = getRd(instruction);

    if (cpu->r[rs] < cpu->r[rt]) {
        cpu->r[rd] = 1;
    } else {
        cpu->r[rd] = 0;
    }
}
void CPU::dadd(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dadd\n";
    exit(1);
}
void CPU::daddu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: daddu\n";
    exit(1);
}
void CPU::dsub(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsub\n";
    exit(1);
}
void CPU::dsubu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsubu\n";
    exit(1);
}
void CPU::tge(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tge\n";
    exit(1);
}
void CPU::tgeu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tgeu\n";
    exit(1);
}
void CPU::tlt(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tlt\n";
    exit(1);
}
void CPU::tltu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tltu\n";
    exit(1);
}
void CPU::teq(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: teq\n";
    exit(1);
}
void CPU::tne(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tne\n";
    exit(1);
}
void CPU::dsll(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsll\n";
    exit(1);
}
void CPU::dsrl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsrl\n";
    exit(1);
}
void CPU::dsra(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsra\n";
    exit(1);
}
void CPU::dsll32(CPU* cpu, uint32_t instruction) {
    uint32_t shift = shiftAmount(instruction) + 32;
    cpu->r[getRd(instruction)] = cpu->r[getRt(instruction)] << shift;
}
void CPU::dsrl32(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: dsrl32\n";
    exit(1);
}
void CPU::dsra32(CPU* cpu, uint32_t instruction) {
    uint32_t shift = shiftAmount(instruction) + 32;
    cpu->r[getRd(instruction)] = (uint64_t)((int64_t)cpu->r[getRt(instruction)] >> shift);
}

void CPU::bltz(CPU* cpu, uint32_t instruction) {
    if ((int64_t)cpu->r[getRs(instruction)] < 0) {
        uint32_t immediate = getImmediate(instruction);

        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
    }
}
void CPU::bgez(CPU* cpu, uint32_t instruction) {
    if ((int64_t)cpu->r[getRs(instruction)] >= 0) {
        uint32_t immediate = getImmediate(instruction);

        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc = cpu->pc + amount;
    }
}
void CPU::bltzl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bltzl\n";
    exit(1);
}
void CPU::bgezl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bgezl\n";
    exit(1);
}

void CPU::tgei(CPU* cpu, uint32_t instruction) {
    std::cout << "inside tgei\n";
    uint32_t rs = getRs(instruction);

    int64_t immediate = (int64_t)getSignedImmediate(instruction);
    int64_t value = (int64_t)cpu->r[rs];

    if (value > immediate) {
        // throw an exception here!
        std::cout << "TODO: throw exception\n";
        exit(1);
    }
}
void CPU::tgeiu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tgeiu\n";
    exit(1);
}
void CPU::tlti(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tlti\n";
    exit(1);
}
void CPU::tltiu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tltiu\n";
    exit(1);
}
void CPU::teqi(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: teqi\n";
    exit(1);
}

void CPU::tnei(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: tnei\n";
    exit(1);
}

void CPU::bltzal(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bltzal\n";
    exit(1);
}
void CPU::bgezal(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);

    if ((int64_t)cpu->r[rs] >= 0) {
        uint32_t immediate = getImmediate(instruction);

        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->r[31] = cpu->nextPc;
        cpu->nextPc = cpu->pc + amount;
    }
}
void CPU::bltzall(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bltzall\n";
    exit(1);
}
void CPU::bgezall(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bgezall\n";
    exit(1);
}