#include "CPU.hpp"
#include <iostream>
#include <fstream>

using namespace std;


uint32_t getImmediate(uint32_t instruction) {
    return instruction & 0xffff;
}

uint64_t getSignedImmediate(uint32_t instruction) {
    return (int16_t)(int64_t)(uint64_t)(instruction & 0xffff);
}

uint32_t getRs(uint32_t instruction) {
    return (instruction >> 21) & 0x1f;
}

uint32_t getRt(uint32_t instruction) {
    return (instruction >> 16) & 0x1f;
}

void CPU::set(int i, uint64_t val) {
    if (i != 0) {
        r[i] = val;
    }
}

COP0::COP0() {
    instructions = {
        COP0::mfc0, // 0
        COP0::dmfc0, // 1
        COP0::reserved, // 2
        COP0::reserved, // 3
        COP0::mtc0, // 4
        COP0::dmtc0, // 5
        COP0::reserved, // 6
        COP0::reserved, // 7
        COP0::reserved, // 8
        COP0::reserved, // 9
        COP0::reserved, // a
        COP0::reserved, // b
        COP0::reserved, // c
        COP0::reserved, // d
        COP0::reserved, // e
        COP0::reserved, // f
        COP0::cp0, // 10
        COP0::reserved, // 11
        COP0::reserved, // 12
        COP0::reserved, // 13
        COP0::reserved, // 14
        COP0::reserved, // 15
        COP0::reserved, // 16
        COP0::reserved, // 17
        COP0::reserved, // 18
        COP0::reserved, // 19
        COP0::reserved, // 1a
        COP0::reserved, // 1b
        COP0::reserved, // 1c
        COP0::reserved, // 1d
        COP0::reserved, // 1e
        COP0::reserved, // 1f
    };
}

CPU::CPU() {
    r[0] = 0;

    pc = 0xBFC00000;
    nextPc = 0xBFC00004;

    instructions = {
        CPU::reserved, // 0
        COP0::reserved, // 1
        CPU::j, // 2
        CPU::jal, // 3
        CPU::beq, // 4
        CPU::bne, // 5
        CPU::blez, // 6
        CPU::bgtz, // 7
        CPU::addi, // 8
        CPU::addiu, // 9
        CPU::slti, // a
        CPU::sltiu, // b
        CPU::andi, // c
        CPU::ori, // d
        CPU::xori, // e
        CPU::lui, // f
        COP0::reserved, // 10
        COP0::reserved, // 11
        COP0::reserved, // 12
        COP0::reserved, // 13
        CPU::beql, // 14
        CPU::bnel, // 15
        CPU::blezl, // 16
        CPU::bgtzl, // 17
        CPU::daddi, // 18
        CPU::daddiu, // 19
        CPU::ldl, // 1a
        CPU::ldr, // 1b
        COP0::reserved, // 1c
        COP0::reserved, // 1d
        COP0::reserved, // 1e
        COP0::reserved, // 1f
        CPU::lb, // 20
        CPU::lh, // 21
        CPU::lwl, // 22
        CPU::lw, // 23
        CPU::lbu, // 24
        CPU::lhu, // 25
        CPU::lwr, // 26
        CPU::lwu, // 27
        CPU::sb, // 28
        CPU::sh, // 29
        CPU::swl, // 2a
        CPU::sw, // 2b
        CPU::sdl, // 2c
        CPU::sdr, // 2d
        CPU::swr, // 2e
        CPU::cache, // 2f
        CPU::ll, // 30
        COP1::lwc1, // 31
        COP0::reserved, // 32
        COP0::reserved, // 33
        CPU::lld, // 34
        COP1::ldc1, // 35
        COP0::reserved, // 36
        CPU::ld, // 37
        CPU::sc, // 38
        COP1::swc1, // 39
        COP0::reserved, // 3a
        COP0::reserved, // 3b
        CPU::scd, // 3c
        COP1::sdc1, // 3d
        COP0::reserved, // 3e
        CPU::sd, // 3f
    };
}

void CPU::loadRom(std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<uint8_t> rom;
    rom.resize(fileSize);

    file.read(reinterpret_cast<char*>(rom.data()), fileSize);

    // copy first 0x1000 bytes of rom to sp dmem
}

void CPU::step() {
    std::cout << "pc is now = " << std::hex << pc << "\n";

    uint32_t opcode = bus.memRead32(pc);

    uint32_t command = opcode >> 26;

    std::cout << "received command " << std::hex << command << "\n";

    uint64_t oldPc = pc;

    if (!discarded) {
        pc = nextPc;
    } else {
        nextPc += 4;
        pc = nextPc;
    }

    uint64_t oldR0 = r[0];

    discarded = false;
    nextPc += 4;

    switch(command) {
        case 0:
        case 1:
        case 16:
            cop0.instructions[(opcode >> 21) & 0x1f](this, opcode);
            break;
        case 17:
        case 18:
        default:
            instructions[command](this, opcode);
            break;
    }

    if (oldR0 != r[0] && r[0] == 0xa) {
        cout << "the pc is " << std::hex << oldPc << "\n";
    }
}

void CPU::lui(CPU* cpu, uint32_t instruction) {
    std::cout << "the instruction we received is " << std::hex << instruction << "\n";
    uint32_t immediate = getImmediate(instruction);

    int reg = (instruction >> 16) & 0x1f;

    uint32_t value = immediate << 16;

    cpu->r[reg] = value;

    std::cout << "register " << reg << " is now " << cpu->r[reg] << "\n";
}

void CPU::addi(CPU* cpu, uint32_t instruction) {
    std::cout << "not yet implemented: addi\n";
    exit(1);
}

void CPU::addiu(CPU* cpu, uint32_t instruction) {
    std::cout << "instruction = " << std::hex << instruction << "\n";
    uint64_t immediate = getSignedImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    std::cout << "adding " << std::hex << immediate << " with " << cpu->r[rs] << "\n";

    std::cout << "rt = " << rt << "\n";

    cpu->r[rt] = cpu->r[rs] + immediate;
}

void CPU::andi(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = getImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    std::cout << "immediate = " << std::hex << immediate << ", rs = " << rs << " value = " << cpu->r[rs] << "\n";

    cpu->r[rt] = cpu->r[rs] & immediate;

    std::cout << "set register " << rt << " to " << cpu->r[rt] << "\n";
}
void CPU::beq(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: beq\n";
    exit(1);
}
void CPU::beql(CPU* cpu, uint32_t instruction) {
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint32_t immediate = getImmediate(instruction);

    std::cout << "rs = " << rs << "\n";

    std::cout << "comparing " << std::hex << cpu->r[rs] << " with " << cpu->r[rt] << "\n";

    if (cpu->r[rs] == cpu->r[rt]) {
        // std::cout << "yes it did the jump!\n";

        // std::cout << "immediate = " << std::hex << immediate << "\n";

        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        std::cout << amount << "\n";

        cpu->nextPc += amount;

        // std::cout << "next pc will be " << cpu->nextPc << "\n";
    } else {
        cpu->pc = cpu->nextPc;
        std::cout << "discarding the delay slot, pc = " << cpu->pc << ", nextPc = " << cpu->nextPc << "\n";

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
    std::cout << "TODO: blez\n";
    exit(1);
}
void CPU::blezl(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: blezl\n";
    exit(1);
}
void CPU::bne(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: bne\n";
    exit(1);
}
void CPU::bnel(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = getImmediate(instruction);
    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    std::cout << "using registers " << rs << " and " << rt << " which = " << std::hex << cpu->r[rs] << ", " << cpu->r[rt] << " respectively\n";

    if (cpu->r[rs] != cpu->r[rt]) {
        std::cout << "shouldn't happen\n";
        exit(1);
        uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

        cpu->nextPc += amount;
    } else {
        cpu->pc = cpu->nextPc;
        cpu->discarded = true;
    }
}
void CPU::cache(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cache\n";
    exit(1);
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
    std::cout << "TODO: j\n";
    exit(1);
}
void CPU::jal(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: jal\n";
    exit(1);
}
void CPU::lb(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lb\n";
    exit(1);
}
void CPU::lbu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lbu\n";
    exit(1);
}
void CPU::ld(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ld\n";
    exit(1);
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
    std::cout << "TODO: lh\n";
    exit(1);
}
void CPU::lhu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: lhu\n";
    exit(1);
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
    std::cout << "inside lw\n";
    uint32_t immediate = getSignedImmediate(instruction);
    uint32_t base_reg = getRs(instruction);
    uint32_t rt = getRt(instruction);

    std::cout << "immediate = " << std::hex << immediate << "\n";

    uint64_t address = cpu->r[base_reg] + immediate;

    std::cout << "address = " << std::hex << address << "\n";

    uint32_t value = cpu->bus.memRead32(address);

    cpu->r[rt] = (int64_t)(uint64_t)value;

    std::cout << "set " << rt << " to " << cpu->r[rt] << "\n";
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
    // std::cout << "inside ori\n";

    uint32_t imm = getImmediate(instruction);

    uint32_t rt = getRt(instruction);
    uint32_t rs = getRs(instruction);

    cpu->r[rt] = cpu->r[rs] | imm;

    // std::cout << "register " << rt << " is now " << std::hex << cpu->r[rs] << "\n";
}
void CPU::reserved(CPU* cpu, uint32_t instruction) {
    std::cout << "Error: opcode reserved\n";
    exit(1);
}
void CPU::sb(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sb\n";
    exit(1);
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
    std::cout << "TODO: sd\n";
    exit(1);
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
    std::cout << "TODO: sh\n";
    exit(1);
}
void CPU::slti(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: slti\n";
    exit(1);
}
void CPU::sltiu(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: sltiu\n";
    exit(1);
}
void CPU::sw(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = getImmediate(instruction);

    uint32_t rs = getRs(instruction);
    uint32_t rt = getRt(instruction);

    uint64_t address = immediate + cpu->r[rs];

    cpu->bus.memWrite32(address, (uint32_t)cpu->r[rt]);

    std::cout << "wrote value " << std::hex << cpu->r[rt] << " to address " << address << "\n";
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

void COP0::reserved(CPU* cpu, uint32_t instruction) {
    std::cout << "instruction reserved for COP0\n";
    exit(1);
}

void COP1::ldc1(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: ldc1\n";
    exit(1);
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

void COP0::cp0(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: cp0\n";
    exit(1);
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
    std::cout << "TODO: mfc0\n";
    exit(1);
}

void COP0::mtc0(CPU* cpu, uint32_t instruction) {
    std::cout << "inside mtc0\n";

    uint32_t rd = (instruction >> 10) & 0x1f;
    uint32_t rt = (instruction >> 15) & 0x1f;

    cpu->cop0.r[rd] = cpu->r[rt];

    std::cout << "set cop0 register " << rd << " to " << std::hex << cpu->cop0.r[rd] << "\n";
}
