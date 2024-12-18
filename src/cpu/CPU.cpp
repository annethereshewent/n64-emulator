#include "CPU.hpp"
#include <iostream>
#include <fstream>

using namespace std;

void CPU::set(int i, uint64_t val) {
    if (i != 0) {
        r[i] = val;
    }
}

COP0::COP0() {
    instructions = {
        COP0::mfc0,
        COP0::dmfc0,
        COP0::reserved,
        COP0::reserved,
        COP0::mtc0,
        COP0::dmtc0,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::cp0,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
    };
}

CPU::CPU() {
    r[0] = 0;

    pc = 0xBFC00000;

    instructions = {
        CPU::reserved,
        COP0::reserved,
        CPU::j,
        CPU::jal,
        CPU::beq,
        CPU::bne,
        CPU::blez,
        CPU::bgtz,
        CPU::addi,
        CPU::addiu,
        CPU::slti,
        CPU::sltiu,
        CPU::andi,
        CPU::ori,
        CPU::xori,
        CPU::lui,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        CPU::beql,
        CPU::bnel,
        CPU::blezl,
        CPU::bgtzl,
        CPU::daddi,
        CPU::daddiu,
        CPU::ldl,
        CPU::ldr,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        CPU::lb,
        CPU::lh,
        CPU::lwl,
        CPU::lw,
        CPU::lbu,
        CPU::lhu,
        CPU::lwr,
        CPU::lwu,
        CPU::sb,
        CPU::sh,
        CPU::swl,
        CPU::sw,
        CPU::sdl,
        CPU::sdr,
        CPU::swr,
        CPU::cache,
        CPU::ll,
        COP1::lwc1,
        COP0::reserved,
        COP0::reserved,
        CPU::lld,
        COP1::ldc1,
        COP0::reserved,
        CPU::ld,
        CPU::sc,
        COP1::swc1,
        COP0::reserved,
        COP0::reserved,
        CPU::scd,
        COP1::sdc1,
        COP0::reserved,
        CPU::sd,
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
    uint32_t opcode = bus.memRead32(pc);

    uint32_t command = opcode >> 26;

    std::cout << "received command " << std::hex << command << "\n";

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
}

void CPU::lui(CPU* cpu, uint32_t instruction) {
    std::cout << "the instruction we received is " << std::hex << instruction << "\n";
    uint32_t immediate = instruction & 0xffff;

    int reg = ((int)instruction >> 16) & 0x1f;

    cpu->r[reg] = immediate << 16;

    std::cout << "set register " << reg << " to value " << std::hex << cpu->r[reg] << "\n";

    cpu->pc += 4;
}

void CPU::addi(CPU* cpu, uint32_t instruction) {
    std::cout << "not yet implemented: addi\n";
    exit(1);
}

void CPU::addiu(CPU* cpu, uint32_t instruction) {
    std::cout << "not yet implemented: addiu\n";
    exit(1);
}

void CPU::andi(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: andi\n";
    exit(1);
}
void CPU::beq(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: beq\n";
    exit(1);
}
void CPU::beql(CPU* cpu, uint32_t instruction) {
    std::cout << "TODO: beql\n";
    exit(1);
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
    std::cout << "TODO: bnel\n";
    exit(1);
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
    std::cout << "TODO: lw\n";
    exit(1);
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
    std::cout << "TODO: ori\n";
    exit(1);
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
    std::cout << "TODO: sw\n";
    exit(1);
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
}
