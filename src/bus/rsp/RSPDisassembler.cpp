#pragma once
#include <cstdint>
#include <string>
#include <array>
#include "RSP.hpp"
#include "../../cpu/CPU.hpp"

class RSP;

typedef std::string (*RSPDisassemble)(RSP* rsp, uint32_t instruction);

std::string addi(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t immediate = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    return std::format("ADDI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, rsp->r[rs]);
}
std::string addiu(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t immediate = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    return std::format("ADDIU r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, rsp->r[rs]);
}
std::string andi(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t immediate = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    return std::format("ANDI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, rsp->r[rs]);
}
std::string beq(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if (rsp->r[rs] == rsp->r[rt]) {
        taken = "Yes";
    }

    return std::format(
        "BEQ r{}, r{}, 0x{:x} ; r{} = 0x{:x}, r{} = 0x{:x} (Taken: {})",
        rs,
        rt,
        amount,
        rs,
        rsp->r[rs],
        rt,
        rsp->r[rt],
        taken
    );
}
std::string bgtz(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if ((int32_t)rsp->r[rs] > 0) {
        taken = "Yes";
    }

    return std::format(
        "BGTZ r{}, 0x{:x} ; r{} = 0x{:x}, (Taken: {})",
        rs,
        amount,
        rs,
        rsp->r[rs],
        taken
    );
}
std::string blez(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if ((int32_t)rsp->r[rs] <= 0) {
        taken = "Yes";
    }

    return std::format(
        "BLEZ r{}, 0x{:x} ; r{} = 0x{:x}, (Taken: {})",
        rs,
        amount,
        rs,
        rsp->r[rs],
        taken
    );
}
std::string bne(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if (rsp->r[rs] != rsp->r[rt]) {
        taken = "Yes";
    }

    return std::format(
        "BNE r{}, r{}, 0x{:x} ; r{} = 0x{:x}, r{} = 0x{:x} (Taken: {})",
        rs,
        rt,
        amount,
        rs,
        rsp->r[rs],
        rt,
        rsp->r[rt],
        taken
    );
}
std::string j(RSP* rsp, uint32_t instruction) {
    uint32_t amount = (rsp->pc & 0xf0000000) | ((instruction & 0x3ffffff) << 2);

    return std::format("J 0x{:x}", amount);
}
std::string jal(RSP* rsp, uint32_t instruction) {
    uint32_t amount = (rsp->pc & 0xf0000000) | ((instruction & 0x3ffffff) << 2);

    return std::format("JAL 0x{:x}", amount);
}
std::string lb(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LB r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string lbu(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LBU r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string lh(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LH r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string lhu(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LHU r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string lui(RSP* rsp, uint32_t instruction) {
    uint32_t immediate = CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("LUI r{}, 0x{:x} ; r{} = 0x{:x}", rt, immediate, rt, rsp->r[rt]);
}
std::string lw(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LW r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string lwu(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: lwu");
}
std::string ori(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t immediate = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    return std::format("ORI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, rsp->r[rs]);
}
std::string reserved(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("reserved RSP instruction received\n");
}
std::string sb(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("SB r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string sh(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("SH r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);

}
std::string slti(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: slti");
}
std::string sltiu(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: sltiu");
}
std::string sw(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("SW r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, rsp->r[rs]);
}
std::string xori(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t immediate = (int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction);

    return std::format("XORI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, rsp->r[rs]);
}

std::string sll(RSP* rsp, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    return std::format("SLL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, rsp->r[rt]);
}
std::string srl(RSP* rsp, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    return std::format("SRL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, rsp->r[rt]);
}
std::string sra(RSP* rsp, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    int32_t value = (int32_t)rsp->r[rt];

    return std::format("SLL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, value);
}
std::string sllv(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = rsp->r[rs] & 0x1f;

    return std::format("SLLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string srlv(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = rsp->r[rs] & 0x1f;

    return std::format("SRLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string srav(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = rsp->r[rs] & 0x1f;

    int32_t value = (int32_t)rsp->r[rt];

    return std::format("SLLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, rsp->r[rs], rt, value);
}
std::string jr(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);

    return std::format("JR r{} ; r{} = 0x{:x}", rs, rs, rsp->r[rs]);
}
std::string jalr(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);

    return std::format("JR r{} ; r{} = 0x{:x}", rs, rs, rsp->r[rs]);
}
std::string break_(RSP* rsp, uint32_t instruction) {
    return "BREAK";
}
std::string add(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("ADD r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string addu(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("ADDU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string sub(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("SUB r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string subu(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("SUBU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string and_(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("AND r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string or_(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("OR r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string xor_(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("XOR r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}
std::string nor(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("NOR r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt]);
}

std::string slt(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    std::string isTrue = "No";

    int32_t rtVal = (int32_t)rsp->r[rt];
    int32_t rsVal = (int32_t)rsp->r[rs];

    if (rsVal < rtVal) {
        isTrue = "Yes";
    }

    return std::format("SLT r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x} (True: {})", rd, rs, rt, rs, rsVal, rt, rtVal, isTrue);
}
std::string sltu(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    std::string isTrue = "No";

    if (rsp->r[rs] < rsp->r[rt]) {
        isTrue = "Yes";
    }

    return std::format("SLTU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x} (True: {})", rd, rs, rt, rs, rsp->r[rs], rt, rsp->r[rt], isTrue);
}

std::string mtc0(RSP* rsp, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("MTC0 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, rsp->r[rt]);
}

std::string mfc0(RSP* rsp, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("MFC0 r{}, r{}", rt, rd);
}

std::string lbv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LSV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string lsv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LSV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string llv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LLV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string ldv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LDV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string lqv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LQV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string lrv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LRV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string lpv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LPV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);

}
std::string luv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LUV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string lhv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: lhv");
}
std::string lfv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: lfv");
}
std::string lwv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: lwv");
}
std::string ltv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("LTV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}

std::string sbv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SBV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string ssv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SSV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string slv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SLV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string sdv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SDV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string sqv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SQV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string srv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: srv");
}
std::string spv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SPV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string suv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("SUV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}
std::string shv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: shv");
}
std::string sfv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: sfv");
}
std::string swv(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: swv");
}
std::string stv(RSP* rsp, uint32_t instruction) {
    uint32_t offset = (uint32_t)(RSP::getVOffset(instruction) << 1);
    uint32_t rs = CPU::getRs(instruction);
    uint8_t velement = RSP::getVElement(instruction);
    uint8_t vt = RSP::getVt(instruction);

    return std::format("STV v{}[{}], 0x{:x}(r{}) ; r{} = 0x{:x}", vt, velement, offset, rs, rs, rsp->r[rs]);
}

std::string mtc2(RSP* rsp, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint8_t velement = RSP::getVElement(instruction);

    return std::format("MTC2 v{}[{}], r{} ; r{} = 0x{:x}", rd, velement, rt, rt, rsp->r[rt]);
}

std::string ctc2(RSP* rsp, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint16_t value = (uint16_t)rsp->r[CPU::getRt(instruction)];

    return std::format("CTC2 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, value);
}

std::string mfc2(RSP* rsp, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t velement = RSP::getVElement(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint8_t hi = rsp->vpr[rd][velement & 0xf];
    uint8_t lo = rsp->vpr[rd][(velement + 1) & 0xf];

    int16_t result = (int16_t)hi << 8 | (int16_t)lo;

    // rsp->r[CPU::getRt(instruction)] = (int32_t)(uint32_t)result;

    return std::format("MFC2 r{}, v{}[{}] ; v{}[{}] = 0x{:x}", rt, rd, velement, rd, velement, result);
}

std::string cfc2(RSP* rsp, uint32_t instruction) {
    uint32_t value;
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);
    switch (rd & 3) {
        case 0:
            value = (int16_t)(int32_t)(uint32_t)rsp->vco;
            break;
        case 1:
            value = (int16_t)(int32_t)(uint32_t)rsp->vcc;
            break;
        case 2:
        case 3:
            value = (int8_t)(int32_t)(uint32_t)rsp->vce;
            break;
    }

    return std::format("CFC2 r{}, r{} ; r{} = 0x{:x}", rt, rd, rd, value);
}

std::string vmulf(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMULF v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}

std::string vmulu(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMULU v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrndp(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: vrndp");
}
std::string vmulq(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: vmulq");
}
std::string vmudl(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMUDL v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmudm(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMUDM v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmudn(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMUDN v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmudh(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMUDH v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmacf(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMACF v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmacu(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMACU v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrndn(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: vrndn");
}
std::string vmacq(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: vmacq");
}
std::string vmadl(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMADL v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmadm(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMADM v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmadn(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMADN v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmadh(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMADH v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vadd(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VADD v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vsub(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VSUB v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vzero(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: vzero");
}
std::string vabs(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VABS v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vaddc(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VADDC v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vsubc(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VSUBC v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vsar(RSP* rsp, uint32_t instruction) {
    return std::format("VSAR v{}, [{}]", RSP::getVd(instruction), RSP::getVte(instruction));
}
std::string vlt(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VLT v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string veq(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VEQ v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vne(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VNE v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vge(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VGE v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vcl(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VCL v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vch(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VCH v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vcr(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VCR v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmrg(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMRG v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vand(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VAND v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vnand(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VNAND v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vor(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VOR v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vnor(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VNOR v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vxor(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VXOR v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vnxor(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VNXOR v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrcp(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VRCP v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrcpl(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VRCPL v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}

std::string vrcph(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VRCPH v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vmov(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VMOV v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrsq(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VRSQ v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrsql(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VRSQL v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vrsqh(RSP* rsp, uint32_t instruction) {
    uint8_t vte = RSP::getVte(instruction);
    uint8_t vt = RSP::getVt(instruction);
    uint8_t vs = RSP::getVs(instruction);
    uint8_t vd = RSP::getVd(instruction);

    return std::format("VRSQH v{}, v{}, v{}[{}]", vd, vs, vt, vte);
}
std::string vnop(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: vnop");
}
std::string bgez(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if ((int32_t)rsp->r[rs] >= 0) {
        taken = "Yes";
    }

    return std::format(
        "BGEZ r{}, 0x{:x} ; r{} = 0x{:x}, (Taken: {})",
        rs,
        amount,
        rs,
        rsp->r[rs],
        taken
    );
}

std::string bltz(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if ((int32_t)rsp->r[rs] < 0) {
        taken = "Yes";
    }

    return std::format(
        "BLTZ r{}, 0x{:x} ; r{} = 0x{:x}, (Taken: {})",
        rs,
        amount,
        rs,
        rsp->r[rs],
        taken
    );
}

std::string bltzal(RSP* rsp, uint32_t instruction) {
    throw std::runtime_error("TODO: bltzal");
}

std::string bgezal(RSP* rsp, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t amount = ((int16_t)(int32_t)(uint32_t)CPU::getImmediate(instruction) << 2);

    std::string taken = "No";

    if ((int32_t)rsp->r[rs] >= 0) {
        taken = "Yes";
    }

    return std::format(
        "BGEZAL r{}, 0x{:x} ; r{} = 0x{:x}, (Taken: {})",
        rs,
        amount,
        rs,
        rsp->r[rs],
        taken
    );
}

std::array<RSPDisassemble, 64> rspInstructions = {
    reserved,
    reserved,
    j,
    jal,
    beq,
    bne,
    blez,
    bgtz,
    addi,
    addiu,
    slti,
    sltiu,
    andi,
    ori,
    xori,
    lui,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    lb,
    lh,
    reserved,
    lw,
    lbu,
    lhu,
    reserved,
    lwu,
    sb,
    sh,
    reserved,
    sw,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
};

std::array<RSPDisassemble, 64> rspSecondary = {
    sll,
    reserved,
    srl,
    sra,
    sllv,
    reserved,
    srlv,
    srav,
    jr,
    jalr,
    reserved,
    reserved,
    reserved,
    break_,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    add,
    addu,
    sub,
    subu,
    and_,
    or_,
    xor_,
    nor,
    reserved,
    reserved,
    slt,
    sltu,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
};

std::array<RSPDisassemble, 12> lwc2 = {
    lbv,
    lsv,
    llv,
    ldv,
    lqv,
    lrv,
    lpv,
    luv,
    lhv,
    lfv,
    lwv,
    ltv,
};
std::array<RSPDisassemble, 12> swc2 = {
    sbv,
    ssv,
    slv,
    sdv,
    sqv,
    srv,
    spv,
    suv,
    shv,
    sfv,
    swv,
    stv
};

std::array<RSPDisassemble, 64> vecInstructions = {
    vmulf,
    vmulu,
    vrndp,
    vmulq,
    vmudl,
    vmudm,
    vmudn,
    vmudh,
    vmacf,
    vmacu,
    vrndn,
    vmacq,
    vmadl,
    vmadm,
    vmadn,
    vmadh,
    vadd,
    vsub,
    vzero,
    vabs,
    vaddc,
    vsubc,
    vzero,
    vzero,
    vzero,
    vzero,
    vzero,
    vzero,
    vzero,
    vsar,
    vzero,
    vzero,
    vlt,
    veq,
    vne,
    vge,
    vcl,
    vch,
    vcr,
    vmrg,
    vand,
    vnand,
    vor,
    vnor,
    vxor,
    vnxor,
    vzero,
    vzero,
    vrcp,
    vrcpl,
    vrcph,
    vmov,
    vrsq,
    vrsql,
    vrsqh,
    vnop,
    vzero,
    vzero,
    vzero,
    vzero,
    vzero,
    vzero,
    vzero,
    vnop,
};

std::string rspDisassemble(RSP* rsp, uint32_t instruction) {
    uint32_t command = instruction >> 26;

    switch (command) {
        case 0:
            return rspSecondary[instruction & 0x3f](rsp, instruction);
            break;
        case 1:
            switch ((instruction >> 16) & 0x1f) {
                case 0:
                    return bltz(rsp, instruction);
                    break;
                case 1:
                    return bgez(rsp, instruction);
                    break;
                case 16:
                    return bltzal(rsp, instruction);
                    break;
                case 17:
                    return bgezal(rsp, instruction);
                    break;
                default:
                    return reserved(rsp, instruction);
                    break;
            }
            break;
        case 16:
            switch ((instruction >> 21) & 0x1f) {
                case 0:
                    return mfc0(rsp, instruction);
                    break;
                case 4:
                    return mtc0(rsp, instruction);
                    break;
                default:
                    return reserved(rsp, instruction);
            }
            break;
        case 18:
            switch ((instruction >> 21) & 0x1f) {
                case 0:
                    return mfc2(rsp, instruction);
                    break;
                case 2:
                    return cfc2(rsp, instruction);
                    break;
                case 4:
                    return mtc2(rsp, instruction);
                    break;
                case 6:
                    return ctc2(rsp, instruction);
                    break;
                default:
                    if (((instruction >> 21) & 0x1f) > 15) {
                        return vecInstructions[instruction & 0x3f](rsp, instruction);
                    } else {
                        return reserved(rsp, instruction);
                    }
                    break;
            }
            break;
        case 50: {
            uint32_t op = (instruction >> 11) & 0x1f;

            if (op < lwc2.size()) {
                return lwc2[op](rsp, instruction);
            } else {
                return reserved(rsp, instruction);
            }
            break;
        }
        case 58: {
            uint32_t op = (instruction >> 11) & 0x1f;

            if (op < swc2.size()) {
                return swc2[op](rsp, instruction);
            } else {
                return reserved(rsp, instruction);
            }
            break;
        }
        default:
            return rspInstructions[command](rsp, instruction);
            break;
    }

    return "UNKNOWN";
}