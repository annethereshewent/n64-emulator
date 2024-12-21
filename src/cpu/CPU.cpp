
#pragma once

#include "CPU.hpp"
#include "../bus/Bus.cpp"
#include "CPUInstructions.cpp"
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

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

    secondary = {
        CPU::sll,    // 0
        COP0::reserved,           // 1
        CPU::srl,    // 2
        CPU::sra,    // 3
        CPU::sllv,   // 4
        COP0::reserved,           // 5
        CPU::srlv,   // 6
        CPU::srav,   // 7
        CPU::jr,     // 8
        CPU::jalr,   // 9
        COP0::reserved,           // 10
        COP0::reserved,           // 11
        COP0::syscall,            // 12
        COP0::break_,             // 13
        COP0::reserved,           // 14
        CPU::sync,   // 15
        CPU::mfhi,   // 16
        CPU::mthi,   // 17
        CPU::mflo,   // 18
        CPU::mtlo,   // 19
        CPU::dsllv,  // 20
        COP0::reserved,           // 21
        CPU::dsrlv,  // 22
        CPU::dsrav,  // 23
        CPU::mult,   // 24
        CPU::multu,  // 25
        CPU::div,    // 26
        CPU::divu,   // 27
        CPU::dmult,  // 28
        CPU::dmultu, // 29
        CPU::ddiv,   // 30
        CPU::ddivu,  // 31
        CPU::add,    // 32
        CPU::addu,   // 33
        CPU::sub,    // 34
        CPU::subu,   // 35
        CPU::and_,    // 36
        CPU::or_,     // 37
        CPU::xor_,    // 38
        CPU::nor,    // 39
        COP0::reserved,           // 40
        COP0::reserved,           // 41
        CPU::slt,    // 42
        CPU::sltu,   // 43
        CPU::dadd,   // 44
        CPU::daddu,  // 45
        CPU::dsub,   // 46
        CPU::dsubu,  // 47
        CPU::tge,    // 48
        CPU::tgeu,   // 49
        CPU::tlt,    // 50
        CPU::tltu,   // 51
        CPU::teq,    // 52
        COP0::reserved,           // 53
        CPU::tne,    // 54
        COP0::reserved,           // 55
        CPU::dsll,   // 56
        COP0::reserved,           // 57
        CPU::dsrl,   // 58
        CPU::dsra,   // 59
        CPU::dsll32, // 60
        COP0::reserved,           // 61
        CPU::dsrl32, // 62
        CPU::dsra32, // 63
    };

    registerInstructions = {
        CPU::bltz,    // 0
        CPU::bgez,    // 1
        CPU::bltzl,   // 2
        CPU::bgezl,   // 3
        COP0::reserved,            // 4
        COP0::reserved,            // 5
        COP0::reserved,            // 6
        COP0::reserved,            // 7
        CPU::tgei,    // 8
        CPU::tgeiu,   // 9
        CPU::tlti,    // 10
        CPU::tltiu,   // 11
        CPU::teqi,    // 12
        COP0::reserved,            // 13
        CPU::tnei,    // 14
        COP0::reserved,            // 15
        CPU::bltzal,  // 16
        CPU::bgezal,  // 17
        CPU::bltzall, // 18
        CPU::bgezall, // 19
        COP0::reserved,            // 20
        COP0::reserved,            // 21
        COP0::reserved,            // 22
        COP0::reserved,            // 23
        COP0::reserved,            // 24
        COP0::reserved,            // 25
        COP0::reserved,            // 26
        COP0::reserved,            // 27
        COP0::reserved,            // 28
        COP0::reserved,            // 29
        COP0::reserved,            // 30
        COP0::reserved,            // 31
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

    bus.cartridge = rom;
}

void CPU::step() {    uint32_t opcode = bus.memRead32(pc);
    uint32_t command = opcode >> 26;
    uint64_t oldPc = pc;

    if (!discarded) {
        pc = nextPc;
    } else {
        nextPc += 4;
        pc = nextPc;
    }

    discarded = false;
    nextPc += 4;

    switch(command) {
        case 0:
            secondary[opcode & 0x3f](this, opcode);
            break;
        case 1:
            registerInstructions[(opcode >> 16) & 0x1f](this, opcode);
            break;
        case 16:
            cop0.instructions[(opcode >> 21) & 0x1f](this, opcode);
            break;
        case 17:

            exit(1);
            break;
        case 18:

            exit(1);
            break;
        default:
            instructions[command](this, opcode);
            break;
    }
}