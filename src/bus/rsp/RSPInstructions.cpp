#pragma once

#include "RSP.hpp"
#include <iostream>
#include "../Bus.hpp"

void RSP::addi(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRt(instruction)] = rsp->r[CPU::getRs(instruction)] + (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    std::cout << "added together " << std::hex << rsp->r[CPU::getRs(instruction)] << " and " << (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << " which = " << rsp->r[CPU::getRt(instruction)] << "\n";
}
void RSP::addiu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: addiu\n";
    exit(1);
}
void RSP::andi(RSP* rsp, uint32_t instruction) {
    std::cout << "and'd " << std::hex << rsp->r[CPU::getRs(instruction)] << " and " << CPU::getImmediate(instruction) << " together\n";
    rsp->r[CPU::getRt(instruction)] = rsp->r[CPU::getRs(instruction)] & CPU::getImmediate(instruction);
}
void RSP::beq(RSP* rsp, uint32_t instruction) {
    if (rsp->r[CPU::getRs(instruction)] == rsp->r[CPU::getRt(instruction)]) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

        std::cout << "set nextPC to " << std::hex << rsp->nextPc << "\n";
    }
    rsp->inDelaySlot = true;
}
void RSP::bgtz(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: bgtz\n";
    exit(1);
}
void RSP::blez(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: blez\n";
    exit(1);
}
void RSP::bne(RSP* rsp, uint32_t instruction) {
    if (rsp->r[CPU::getRs(instruction)] != rsp->r[CPU::getRt(instruction)]) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

        std::cout << "set nextPC to " << std::hex << rsp->nextPc << "\n";
    }
    rsp->inDelaySlot = true;
}
void RSP::j(RSP* rsp, uint32_t instruction) {
    rsp->nextPc = (rsp->pc & 0xf0000000) | ((instruction & 0x3ffffff) << 2);
    std::cout << "set nextPc = " << std::hex << rsp->nextPc << "\n";
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
    std::cout << "TODO: lh\n";
    exit(1);
}
void RSP::lhu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lhu\n";
    exit(1);
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

    std::cout << "set register " << std::dec << CPU::getRt(instruction) << " to value " << std::hex << value << "\n";
}
void RSP::lwu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lwu\n";
    exit(1);
}
void RSP::ori(RSP* rsp, uint32_t instruction) {
    std::cout << "or'd " << std::hex << rsp->r[CPU::getRs(instruction)] << " and " << CPU::getImmediate(instruction) << " together\n";
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
    std::cout << "TODO: sh\n";
    exit(1);
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
    std::cout << "TODO: sw\n";
    exit(1);
}
void RSP::xori(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: xori\n";
    exit(1);
}

void RSP::sll(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRt(instruction)] << CPU::shiftAmount(instruction);
}
void RSP::srl(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: srl\n";
    exit(1);
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
    std::cout << "nextPc = " << std::hex << rsp->nextPc << "\n";
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
    std::cout << "TODO: add\n";
    exit(1);
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
    rsp->r[CPU::getRt(instruction)] = rsp->readRegisters(CPU::getRd(instruction));

    rsp->cycleCounter += 4;
    rsp->isRunning = false;
}