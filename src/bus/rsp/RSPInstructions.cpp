#pragma once

#include "RSP.hpp"
#include <iostream>
#include <algorithm>
#include "../Bus.hpp"
#include <print>
#include <bit>

typedef unsigned __int128 u128;

void RSP::addi(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRt(instruction)] = rsp->r[CPU::getRs(instruction)] + (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
}
void RSP::addiu(RSP* rsp, uint32_t instruction) {
    RSP::addi(rsp, instruction);
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
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint32_t value = (int8_t)(int32_t)(uint32_t)rsp->memRead8(address & 0xfff);

    rsp->r[CPU::getRt(instruction)] = value;
}
void RSP::lbu(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint32_t value = (uint32_t)rsp->memRead8(address & 0xfff);

    rsp->r[CPU::getRt(instruction)] = value;
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
    rsp->r[CPU::getRt(instruction)] = CPU::getImmediate(instruction) << 16;
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
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    rsp->memWrite8(address & 0xfff, (uint8_t)rsp->r[CPU::getRt(instruction)]);
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
    rsp->r[CPU::getRd(instruction)] = (uint32_t)((int32_t)rsp->r[CPU::getRt(instruction)] >> CPU::shiftAmount(instruction));
}
void RSP::sllv(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = rsp->r[rs] & 0x1f;

    rsp->r[rd] = rsp->r[rs] << shift;
}
void RSP::srlv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: srlv\n";
    exit(1);
}
void RSP::srav(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = (uint32_t)((int32_t)rsp->r[CPU::getRt(instruction)] >> (rsp->r[CPU::getRs(instruction)] & 31));
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
    rsp->cpuBroken = true;
}
void RSP::add(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRs(instruction)] + rsp->r[CPU::getRt(instruction)];
}
void RSP::addu(RSP* rsp, uint32_t instruction) {
    RSP::add(rsp, instruction);
}
void RSP::sub(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRs(instruction)] - rsp->r[CPU::getRt(instruction)];
}
void RSP::subu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: subu\n";
    exit(1);
}
void RSP::and_(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRs(instruction)] & rsp->r[CPU::getRt(instruction)];
}
void RSP::or_(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRs(instruction)] | rsp->r[CPU::getRt(instruction)];
}
void RSP::xor_(RSP* rsp, uint32_t instruction) {
    rsp->r[CPU::getRd(instruction)] = rsp->r[CPU::getRs(instruction)] ^ rsp->r[CPU::getRt(instruction)];
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
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    if (rd < NUM_RSP_REGISTERS) {
        rsp->writeRegisters(CPU::getRd(instruction), rsp->r[rt]);
    } else {
        rsp->bus.rdp.writeRegisters(rd - NUM_RSP_REGISTERS, rsp->r[rt]);
    }

    if (rsp->status.halted) {
        rsp->status.halted = 0;
        rsp->cpuHalted = true;
    }
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
}

void RSP::lbv(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: lbv\n";
    exit(1);
}
void RSP::lsv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 1);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    uint32_t scale = std::min(16 - velement, 2);

    for (int i = 0; i < scale; i++) {
        rsp->setVec8(vt, (velement + i), rsp->memRead8(address + i));
    }
}
void RSP::llv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 2);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    uint32_t scale = std::min(16 - velement, 4);

    for (int i = 0; i < scale; i++) {
        rsp->setVec8(vt, (velement + i), rsp->memRead8(address + i));
    }
}
void RSP::ldv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 3);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    uint32_t scale = std::min(16 - velement, 8);

    for (int i = 0; i < scale; i++) {
        rsp->setVec8(vt, (velement + i), rsp->memRead8(address + i));
    }
}
void RSP::lqv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 4);

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
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 4);

    uint32_t end = rsp->r[CPU::getRs(instruction)] + offset;
    uint32_t address = end & 0xff0;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    uint8_t elOffset = end & 0xf;

    uint8_t startElement = (16 - elOffset) + velement;

    uint8_t length = std::min(elOffset, (uint8_t)(16 - startElement));

    for (int i = 0; i < length; i++) {
        rsp->setVec8(vt, velement + i, rsp->memRead8(address + i));
    }
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
    uint32_t offset = (uint32_t)getVOffset(instruction);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);

    rsp->memWrite8(address, rsp->getVec8(getVt(instruction), velement));
}
void RSP::ssv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 1);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    for (int i = 0; i < 2; i++) {
        rsp->memWrite8(address + i, rsp->getVec8(vt, (velement + i) & 0xf));
    }
}
void RSP::slv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 2);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    for (int i = 0; i < 4; i++) {
        rsp->memWrite8(address + i, rsp->getVec8(vt, (velement + i) & 0xf));
    }
}
void RSP::sdv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 3);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    for (int i = 0; i < 8; i++) {
        rsp->memWrite8(address + i, rsp->getVec8(vt, (velement + i) & 0xf));
    }
}
void RSP::sqv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(getVOffset(instruction) << 4);

    uint32_t address = rsp->r[CPU::getRs(instruction)] + offset;

    uint8_t velement = getVElement(instruction);
    uint8_t vt = getVt(instruction);

    uint32_t end = (address & 0xff0) + 16;

    uint32_t len = end - address;

    for (int i = 0; i < len; i++) {
        rsp->memWrite8(address + i, rsp->getVec8(vt, (velement + i) & 0xf));
    }
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
    uint32_t value;
    switch (CPU::getRd(instruction)) {
        case 0:
            std::println("it's vco");
            value = (int16_t)(int32_t)(uint32_t)rsp->vco;
            break;
        case 1:
            std::println("it's vcc");
            value = (int16_t)(int32_t)(uint32_t)rsp->vcc;
            break;
        case 2:
        case 3:
            std::println("it's vce");
            value = (int8_t)(int32_t)(uint32_t)rsp->vce;
            break;
    }

    std::println("setting register {} to {:x}", CPU::getRt(instruction), value);

    rsp->r[CPU::getRt(instruction)] = value;
}

void RSP::vmulf(RSP* rsp, uint32_t instruction) {
    vectorMultiplyFractions(rsp, instruction, false, 0x8000);
    rsp->setVecFromAccSignedMid(getVd(instruction));
}

void RSP::vectorLogicalOp(RSP* rsp, uint32_t instruction, auto fn) {

    uint8_t vte = getVte(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vs = getVs(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int16_t result = fn(s, t);

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);
    }

    rsp->setVecFromAccLow(getVd(instruction));

    u128 value = std::byteswap(*(u128*)&rsp->vpr[getVd(instruction)]);

    std::println("v{} is now = {:x}", getVd(instruction), value);
}

void RSP::vectorMultiplyFractions(RSP* rsp, uint32_t instruction, bool accumulate, int32_t round) {
    uint8_t vte = getVte(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vs = getVs(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int32_t result = (int32_t)s * (int32_t)t * 2 + round;

        rsp->updateAccumulatorMid32(el, result, accumulate);
    }
}

void RSP::vectorMulPartialMidM(RSP* rsp, uint32_t instruction, bool accumulate) {
    uint8_t vte = getVte(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vs = getVs(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int32_t result = (int32_t)s * (int32_t)t;

        rsp->updateAccumulatorMid32(el, result, accumulate);
    }
}

void RSP::vectorMultiplyPartialMidN(RSP* rsp, uint32_t instruction, bool accumulate) {
    uint8_t vte = getVte(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vs = getVs(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        uint16_t s = rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int32_t result = (int32_t)s * (int32_t)t;

        rsp->updateAccumulatorMid32(el, result, accumulate);
    }
}

void RSP::vectorMultiplyPartialLow(RSP* rsp, uint32_t instruction, bool accumulate) {
    uint8_t vte = getVte(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vs = getVs(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        uint16_t s = rsp->getVec16(vs, el);
        uint16_t t = rsp->getVec16(vt, select & 0x7);

        uint32_t result = ((uint32_t)s * (uint32_t)t) >> 16;

        rsp->updateAccumulatorLow32(el, (int32_t)result, accumulate);
    }
}

void RSP::vectorMultiplyPartialHigh(RSP* rsp, uint32_t instruction, bool accumulate) {
    uint8_t vte = getVte(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vs = getVs(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int32_t result = (int32_t)s * (int32_t)t;

        if (rsp->previousPc == 0x34) {
            std::println("multiplied {:x} and {:x} together", s, t);
        }

        rsp->updateAccumulatorHigh32(el, result, accumulate);
    }
}

void RSP::vectorSetAccumulatorFromRegister(RSP* rsp, uint32_t instruction) {
    uint8_t vt = getVt(instruction);
    uint8_t vte = getVte(instruction);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t result = rsp->getVec16(vt, select & 0x7);

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);
    }
}

void RSP::vmulu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vmulu\n";
    exit(1);
}
void RSP::vrndp(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vrndp\n";
    exit(1);
}
void RSP::vmulq(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vmulq\n";
    exit(1);
}
void RSP::vmudl(RSP* rsp, uint32_t instruction) {
    vectorMultiplyPartialLow(rsp, instruction, false);
    rsp->setVecFromAccLow(getVd(instruction));
}
void RSP::vmudm(RSP* rsp, uint32_t instruction) {
    vectorMulPartialMidM(rsp, instruction, false);
    rsp->setVecFromAccMid(getVd(instruction));
}
void RSP::vmudn(RSP* rsp, uint32_t instruction) {
    vectorMultiplyPartialMidN(rsp, instruction, false);
    rsp->setVecFromAccLow(getVd(instruction));
}
void RSP::vmudh(RSP* rsp, uint32_t instruction) {
    vectorMultiplyPartialHigh(rsp, instruction, false);
    rsp->setVecFromAccSignedMid(getVd(instruction));
}
void RSP::vmacf(RSP* rsp, uint32_t instruction) {
    vectorMultiplyFractions(rsp, instruction, true, 0);
    rsp->setVecFromAccSignedMid(getVd(instruction));
}
void RSP::vmacu(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vmacu\n";
    exit(1);
}
void RSP::vrndn(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vrndn\n";
    exit(1);
}
void RSP::vmacq(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vmacq\n";
    exit(1);
}
void RSP::vmadl(RSP* rsp, uint32_t instruction) {
    vectorMultiplyPartialLow(rsp, instruction, true);
    rsp->setVecFromAccSignedLow(getVd(instruction));
}
void RSP::vmadm(RSP* rsp, uint32_t instruction) {
    vectorMulPartialMidM(rsp, instruction, true);
    rsp->setVecFromAccSignedMid(getVd(instruction));
}
void RSP::vmadn(RSP* rsp, uint32_t instruction) {
    vectorMultiplyPartialMidN(rsp, instruction, true);
    rsp->setVecFromAccSignedLow(getVd(instruction));
}
void RSP::vmadh(RSP* rsp, uint32_t instruction) {
    vectorMultiplyPartialHigh(rsp, instruction, true);
    rsp->setVecFromAccSignedMid(getVd(instruction));
}
void RSP::vadd(RSP* rsp, uint32_t instruction) {
    uint8_t vs = getVs(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vte = getVte(instruction);
    uint8_t vd = getVd(instruction);

    uint16_t vco = rsp->vco;

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = rsp->getVec16(vs, el);
        int16_t t = rsp->getVec16(vt, select & 0x7);
        int16_t c = (vco >> el) & 0b1;
        int16_t result = s + t + c;

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);

        int32_t clamped = std::clamp((int32_t)s + (int32_t)t + (int32_t)c, -0x8000, 0x7fff);

        rsp->setVec16(vd, el, (int16_t)clamped);
    }

    u128 value = std::byteswap(*(u128*)&rsp->vpr[getVd(instruction)]);

    std::println("(vadd)set v{} = {:x}", getVd(instruction), value);

    rsp->vco = 0;
}
void RSP::vsub(RSP* rsp, uint32_t instruction) {
    uint8_t vs = getVs(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vte = getVte(instruction);
    uint8_t vd = getVd(instruction);

    uint16_t vco = rsp->vco;

    std::println("vco = {:x}", vco);

    u128 vecVs = std::byteswap(*(u128*)&rsp->vpr[vs]);
    u128 vecVt = std::byteswap(*(u128*)&rsp->vpr[vt]);

    std::println("subtracting {:x} and {:x}", vecVs, vecVt);

    u128 oldValue = std::byteswap(*(u128*)&rsp->vpr[getVd(instruction)]);

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);
        int16_t c = (vco >> el) & 0b1;

        int16_t result = s - (t + c);

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);

        int32_t clamped = std::clamp((int32_t)s - ((int32_t)t + (int32_t)c), -0x8000, 0x7fff);

        rsp->setVec16(vd, el, (int16_t)clamped);
    }

    u128 value = std::byteswap(*(u128*)&rsp->vpr[vd]);

    std::println("(vsub)set v{} = {:x} from {:x}", getVd(instruction), value, oldValue);

    rsp->vco = 0;
}
void RSP::vzero(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vzero\n";
    exit(1);
}
void RSP::vabs(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vabs\n";
    exit(1);
}
void RSP::vaddc(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vaddc\n";
    exit(1);
}
void RSP::vsubc(RSP* rsp, uint32_t instruction) {
    uint8_t vs = getVs(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vte = getVte(instruction);

    uint16_t vco = 0;

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = rsp->getVec16(vs, el);
        int16_t t = rsp->getVec16(vt, select & 0x7);

        int16_t result = s - t;

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);

        vco |= (result != 0) ? (1 << (el + 8)) : 0;
        vco |= (result < 0) ? (1 << el) : 0;
    }
    rsp->setVecFromAccLow(getVd(instruction));
    rsp->vco = vco;
}
void RSP::vsar(RSP* rsp, uint32_t instruction) {
    switch (getVte(instruction)) {
        case 8:
            memcpy(&rsp->vpr[getVd(instruction)], &rsp->accHi, sizeof(rsp->accHi));
            break;
        case 9:
            memcpy(&rsp->vpr[getVd(instruction)], &rsp->accMid, sizeof(rsp->accMid));
            break;
        case 10:
            memcpy(&rsp->vpr[getVd(instruction)], &rsp->accLo, sizeof(rsp->accLo));
            break;
        default:
            std::println("invalid option for vsar given: {}", getVte(instruction));
            exit(1);
            break;
    }
}
void RSP::vlt(RSP* rsp, uint32_t instruction) {
    uint8_t vs = getVs(instruction);
    uint8_t vt = getVt(instruction);

    uint8_t vte = getVte(instruction);

    uint16_t vco = rsp->vco;
    uint16_t vcc = 0;
    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int8_t currBit = 1 << el;

        bool vcoLo = (vco & currBit) != 0;
        bool vcoHi = (vco & (currBit << 8)) != 0;

        bool condition = (s < t) || (s == t && vcoLo && vcoHi);

        int16_t result = condition ? s : t;

        vcc |= condition ? currBit : 0;

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);
    }
    rsp->setVecFromAccLow(getVd(instruction));
    rsp->vcc = vcc;
    rsp->vco = 0;
}
void RSP::veq(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: veq\n";
    exit(1);
}
void RSP::vne(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vne\n";
    exit(1);
}
void RSP::vge(RSP* rsp, uint32_t instruction) {
    uint8_t vs = getVs(instruction);
    uint8_t vt = getVt(instruction);

    uint8_t vte = getVte(instruction);

    uint16_t vco = rsp->vco;
    uint16_t vcc = 0;
    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        int8_t currBit = 1 << el;

        bool vcoLo = (vco & currBit) != 0;
        bool vcoHi = (vco & (currBit << 8)) != 0;

        bool bothEqual = vcoLo && vcoHi;

        bool condition = (s > t) || (s == t && !bothEqual);

        int16_t result = condition ? s : t;

        vcc |= condition ? currBit : 0;

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);
    }
    rsp->setVecFromAccLow(getVd(instruction));
    rsp->vcc = vcc;
    rsp->vco = 0;
}
void RSP::vcl(RSP* rsp, uint32_t instruction) {
    uint8_t vccHi = (uint8_t)(rsp->vcc >> 8);
    uint8_t vccLo = (uint8_t)rsp->vcc;

    uint8_t vce = rsp->vce;

    uint8_t vcoHi = (uint8_t)(rsp->vco >> 8);
    uint8_t vcoLo = (uint8_t)rsp->vco;

    uint8_t outLo = 0;
    uint8_t outHi = 0;

    for (int el = 0, select = rsp->vecSelect[getVte(instruction)]; el < 8; el++, select >>= 4) {
        uint16_t s = rsp->getVec16(getVs(instruction), el);
        uint16_t t = rsp->getVec16(getVt(instruction), select & 0x7);

        uint8_t le = (vccLo >> el) & 1;
        uint8_t ge = (vccHi >> el) & 1;
        uint8_t ce = (vce >> el) & 1;

        uint8_t sign = (vcoLo >> el) & 1;
        uint8_t eq = (~(vcoHi >> el)) & 1;

        uint16_t result;

        if (sign) {
            if (eq) {
                uint16_t sum = s + t;
                uint32_t sumCarry = (uint32_t)s + (uint32_t)t;
                bool carry = sum < sumCarry;

                le = (ce == 0 && (sum == 0 && !carry)) || (ce == 1 && (sum == 0 || !carry));
            }
            result = le ? -t : s;
        } else {
            if (eq) {
                ge = ((int16_t)s - (int16_t)t) >= 0;
            }
            result = ge ? t : s;
        }

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);

        outHi |= (uint8_t)ge << el;
        outLo |= (uint8_t)le << el;
    }

    rsp->setVecFromAccLow(getVd(instruction));

    rsp->vcc = (uint16_t)outHi << 8 | (uint16_t)outLo;

    rsp->vco = 0;
    rsp->vce = 0;
}
void RSP::vch(RSP* rsp, uint32_t instruction) {
    uint8_t vs = getVs(instruction);
    uint8_t vt = getVt(instruction);
    uint8_t vte = getVte(instruction);

    // wtf this wasn't working
    // uint8_t vccHi, vccLo, vcoHi, vcoLo, vce = 0;

    uint8_t vccHi = 0;
    uint8_t vccLo = 0;
    uint8_t vcoHi = 0;
    uint8_t vcoLo = 0;
    uint8_t vce = 0;

    for (int el = 0, select = rsp->vecSelect[vte]; el < 8; el++, select >>= 4) {
        int16_t s = (int16_t)rsp->getVec16(vs, el);
        int16_t t = (int16_t)rsp->getVec16(vt, select & 0x7);

        bool sign = (s ^ t) < 0;

        bool le, ge, ce, ne;
        uint32_t result;
        if (sign) {
            int32_t sum = ((int32_t)(s + t) << 16) >> 16;

            ge = t < 0;
            le = sum <= 0;
            ce = sum == -1;
            ne = sum != 0 && (s != ~t);
            result = le ? -t : s;
        } else {
            int32_t diff = ((int32_t)(s - t) << 16) >> 16;

            le = t < 0;
            ge = diff >= 0;
            ce = false;
            ne = diff != 0;
            result = ge ? t : s;
        }
        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);

        vccHi |= (uint8_t)ge << el;
        vccLo |= (uint8_t)le << el;

        vcoHi |= (uint8_t)ne << el;
        vcoLo |= (uint8_t)sign << el;
        vce |= (uint8_t)ce << el;
    }

    rsp->setVecFromAccLow(getVd(instruction));

    rsp->vcc = (uint16_t)vccHi << 8 | (uint16_t)vccLo;
    rsp->vco = (uint16_t)vcoHi << 8 | (uint16_t)vcoLo;

    rsp->vce = vce;
}
void RSP::vcr(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vcr\n";
    exit(1);
}
void RSP::vmrg(RSP* rsp, uint32_t instruction) {
    for (int el = 0, select = rsp->vecSelect[getVte(instruction)]; el < 8; el++, select >>= 4) {
        uint16_t s = rsp->getVec16(getVs(instruction), el);
        uint16_t t = rsp->getVec16(getVt(instruction), select & 0x7);

        uint16_t result = rsp->vcc & 0x1 ? s : t;

        rsp->accLo[el * 2] = (uint8_t)result;
        rsp->accLo[el * 2 + 1] = (uint8_t)(result >> 8);
    }
    rsp->vco = 0;
    rsp->setVecFromAccLow(getVd(instruction));
}
void RSP::vand(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vand\n";
    exit(1);
}
void RSP::vnand(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vnand\n";
    exit(1);
}
void RSP::vor(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vor\n";
    exit(1);
}
void RSP::vnor(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vnor\n";
    exit(1);
}
void RSP::vxor(RSP* rsp, uint32_t instruction) {
    RSP::vectorLogicalOp(rsp, instruction, [](int16_t s, int16_t t) -> int16_t { return s ^ t; });
}
void RSP::vnxor(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vnxor\n";
    exit(1);
}
void RSP::vrcp(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vrcp\n";
    exit(1);
}
void RSP::vrcpl(RSP* rsp, uint32_t instruction) {
    int16_t val = (int16_t)rsp->getVec16(getVt(instruction), getVte(instruction) & 0x7);

    u128 value = std::byteswap(*(u128*)&rsp->vpr[getVt(instruction)]);

    std::println("val = {:x}, vt = {}, vte = {}, vt reg = {:x}", val, getVt(instruction), getVte(instruction) & 0x7, value);

    int32_t input = rsp->divDp ? (int32_t)rsp->divIn << 16 | (int32_t)val : (int32_t)val;

    std::println("got back input {:x}", input);

    int32_t mask = input >> 31;
    int32_t data = input ^ mask;

    std::println("data = {:x}", data);

    if (input > -0x8000) {
        data -= mask;
    }

    uint32_t result;

    if (data == 0) {
        result = 0x7fffffff;
    } else if (data == -0x8000) {
        result = 0xffff0000;
    } else {
        uint32_t shift = std::countl_zero((uint32_t)data);
        uint32_t index = ((((uint64_t)data) << shift) & 0x7fc00000) >> 22;

        result = (uint32_t)rsp->reciprocals[index];

        result = (0x10000 | result) << 14;
        result = (result >> (31 - shift)) ^ (uint32_t)mask;

    }

    rsp->divDp = false;
    rsp->divOut = (int16_t)(result >> 16);

    std::println("got back result {:x}", result);

    rsp->setVec16(getVd(instruction), getDe(instruction), (uint16_t)result);
}
void RSP::vrcph(RSP* rsp, uint32_t instruction) {
    vectorSetAccumulatorFromRegister(rsp, instruction);

    rsp->divDp = true;
    rsp->divIn = (int16_t)rsp->getVec16(getVt(instruction), getVte(instruction) & 0x7);

    std::println("setting divIn to {:x}", rsp->divIn);

    rsp->setVec16(getVd(instruction), getDe(instruction), rsp->divOut);
}
void RSP::vmov(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vmov\n";
    exit(1);
}
void RSP::vrsq(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vrsq\n";
    exit(1);
}
void RSP::vrsql(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vrsql\n";
    exit(1);
}
void RSP::vrsqh(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vrsqh\n";
    exit(1);
}
void RSP::vnop(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: vnop\n";
    exit(1);
}
void RSP::bgez(RSP* rsp, uint32_t instruction) {
    if ((int32_t)rsp->r[CPU::getRs(instruction)] >= 0) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);
    }

    rsp->inDelaySlot = true;
}

void RSP::bltz(RSP* rsp, uint32_t instruction) {
    if ((int32_t)rsp->r[CPU::getRs(instruction)] < 0) {
        rsp->nextPc = rsp->pc + ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);
    }

    rsp->inDelaySlot = true;
}

void RSP::bltzal(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: bltzal\n";
    exit(1);
}

void RSP::bgezal(RSP* rsp, uint32_t instruction) {
    std::cout << "TODO: bgezal\n";
    exit(1);
}