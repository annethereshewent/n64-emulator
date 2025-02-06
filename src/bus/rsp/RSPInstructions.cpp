#pragma once

#include "RSP.hpp"
#include <iostream>
#include <algorithm>
#include "../Bus.hpp"

void RSP::addi(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRt(instruction)] = rsp->r[CPU::getRs(instruction)] + (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
}
void RSP::addiu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: addiu\n";
    exit(1);
}
void RSP::andi(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRt(instruction)] = rsp->r[CPU::getRs(instruction)] & CPU::getImmediate(instruction);
}
void RSP::beq(RSP* rsp, uint32_t instruction) {
    if (rsp->r[CPU::getRs(instruction)] == rsp->r[CPU::getRt(instruction)]) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);
    }
    rsp->inDelaySlot = true;
}
void RSP::bgtz(RSP* rsp, uint32_t instruction) {
    if ((int32_t)rsp->r[CPU::getRs(instruction)] > 0) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);
    }
    rsp->inDelaySlot = true;
}
void RSP::blez(RSP* rsp, uint32_t instruction) {
    if ((int32_t)rsp->r[CPU::getRs(instruction)] <= 0) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);
    }
    rsp->inDelaySlot = true;
}
void RSP::bne(RSP* rsp, uint32_t instruction) {
    if (rsp->r[CPU::getRs(instruction)] != rsp->r[CPU::getRt(instruction)]) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);
    }
    rsp->inDelaySlot = true;
}
void RSP::j(RSP* rsp, uint32_t instruction) {
    rsp->nextPc = (rsp->pc & 0xf0000000) | ((instruction & 0x3ffffff) << 2);
    rsp->inDelaySlot = true;
}
void RSP::jal(RSP* rsp, uint32_t instruction) {
    rsp->r[31] = rsp->nextPc;

    rsp->nextPc = (rsp->pc & 0xf0000000) | ((instruction & 0x3ffffff) << 2);
    rsp->inDelaySlot = true;
}
void RSP::lb(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lb\n";
    exit(1);
}
void RSP::lbu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lbu\n";
    exit(1);
}
void RSP::lh(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint32_t value = (int16_t)(int32_t)(uint32_t)rsp->memRead16(address & 0xfff);

    rsp->r[CPU::getRt(instruction)] = value;
}
void RSP::lhu(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint32_t value = (uint32_t)rsp->memRead16(address & 0xfff);

    rsp->r[CPU::getRt(instruction)] = value;
}
void RSP::lui(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lui\n";
    exit(1);
}
void RSP::lw(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint32_t value = rsp->memRead32(&rsp->dmem[address & 0xfff]);

    rsp->r[CPU::getRt(instruction)] = value;
}
void RSP::lwu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lwu\n";
    exit(1);
}
void RSP::ori(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRt(instruction)] = rsp->r[CPU::getRs(instruction)] | CPU::getImmediate(instruction);
}
void RSP::reserved(RSP* rsp, uint32_t instruction) {
    std::cout << "reserved RSP instruction received\n";
    exit(1);
}
void RSP::sb(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sb\n";
    exit(1);
}
void RSP::sh(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    rsp->memWrite16(address & 0xfff, (uint16_t)rsp->r[CPU::getRt(instruction)]);

}
void RSP::slti(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: slti\n";
    exit(1);
}
void RSP::sltiu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sltiu\n";
    exit(1);
}
void RSP::sw(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    rsp->memWrite32(address & 0xfff, rsp->r[CPU::getRt(instruction)]);
}
void RSP::xori(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: xori\n";
    exit(1);
}

void RSP::sll(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRt(instruction)] << CPU::shiftAmount(instruction);
}
void RSP::srl(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRt(instruction)] >> CPU::shiftAmount(instruction);
}
void RSP::sra(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sra\n";
    exit(1);
}
void RSP::sllv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sllv\n";
    exit(1);
}
void RSP::srlv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: srlv\n";
    exit(1);
}
void RSP::srav(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: srav\n";
    exit(1);
}
void RSP::jr(RSP* rsp, uint32_t instruction) {
    rsp->nextPc = rsp->r[CPU::getRs(instruction)];
    rsp->inDelaySlot = true;
}
void RSP::jalr(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: jalr\n";
    exit(1);
}
void RSP::break_(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: break\n";
    exit(1);
}
void RSP::add(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRs(instruction)] + rsp->r[CPU::getRt(instruction)];
}
void RSP::addu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: addu\n";
    exit(1);
}
void RSP::sub(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sub\n";
    exit(1);
}
void RSP::subu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: subu\n";
    exit(1);
}
void RSP::and_(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: and\n";
    exit(1);
}
void RSP::or_(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: or\n";
    exit(1);
}
void RSP::xor_(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: xor\n";
    exit(1);
}
void RSP::nor(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: nor\n";
    exit(1);
}

void RSP::slt(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: slt\n";
    exit(1);
}
void RSP::sltu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sltu\n";
    exit(1);
}

void RSP::mtc0(RSP* rsp, uint32_t instruction) {
    rsp->writeRegisters(CPU::getRd(instruction), rsp->r[CPU::getRt(instruction)]);

    if (rsp->status.halted) {
        rsp->status.halted = 0;
        rsp->cpuHalted = true;
    }

    rsp->isRunning = false;
}

void RSP::mfc0(RSP* rsp, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    if (rd < NUM_RSP_REGISTERS) {
        rsp->r[rt] = rsp->readRegisters(CPU::getRd(instruction));
    } else {
        rsp->r[rt] = rsp->bus.rdp.readRegisters(rd - NUM_RSP_REGISTERS);
    }

    rsp->cycleCounter += 4;
    rsp->isRunning = false;
}

void RSP::lbv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lbv\n";
    exit(1);
}
void RSP::lsv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lsv\n";
    exit(1);
}
void RSP::llv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: llv\n";
    exit(1);
}
void RSP::ldv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = getVOffset(instruction) << 3;

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    for (int i = 0; i < 8; i++) {
        rsp->setVec8(vt, (velement + i), rsp->memRead8(address + i));
    }
}
void RSP::lqv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = getVOffset(instruction) << 4;

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    uint32_t end = (address & 0xff0) + 16;

    uint32_t length = std::min(end - address, 16 - (uint32_t)velement);

    for (int i = 0; i < length; i++) {
        rsp->setVec8(vt, velement + i, rsp->memRead8(address + i));
    }
}
void RSP::lrv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lrv\n";
    exit(1);
}
void RSP::lpv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lpv\n";
    exit(1);
}
void RSP::luv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: luv\n";
    exit(1);
}
void RSP::lhv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lhv\n";
    exit(1);
}
void RSP::lfv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lfv\n";
    exit(1);
}
void RSP::lwv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lwv\n";
    exit(1);
}
void RSP::ltv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: ltv\n";
    exit(1);
}

void RSP::sbv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sbv\n";
    exit(1);
}
void RSP::ssv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: ssv\n";
    exit(1);
}
void RSP::slv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: slv\n";
    exit(1);
}
void RSP::sdv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = getVOffset(instruction) << 3;

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    for (int i = 0; i < 8; i++) {
        rsp->memWrite8(address + i, rsp->getVec8(vt, (velement + i) & 0xf));
    }
}
void RSP::sqv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sqv\n";
    exit(1);
}
void RSP::srv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: srv\n";
    exit(1);
}
void RSP::spv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: spv\n";
    exit(1);
}
void RSP::suv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: suv\n";
    exit(1);
}
void RSP::shv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: shv\n";
    exit(1);
}
void RSP::sfv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: sfv\n";
    exit(1);
}
void RSP::swv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: swv\n";
    exit(1);
}
void RSP::stv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: stv\n";
    exit(1);
}

void RSP::mtc2(RSP* rsp, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint8_t velement = getVElement(instruction);
    uint32_t value = rsp->r[CPU::getRt(instruction)];

    std::cout << "setting vector register " << std::hex << rd << "'s  to value " << (uint16_t)value << " from register " << std::dec << CPU::getRt(instruction) << "\n";

    rsp->setVec16UnalignedNoWrap(rd, velement, (uint16_t)value);
}

void RSP::ctc2(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: ctc2\n";
    exit(1);
}

void RSP::mfc2(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: mfc2\n";
    exit(1);
}

void RSP::cfc2(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: cfc2\n";
    exit(1);
}

void RSP::vecInstructions(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vecInstructions\n";
    exit(1);
}