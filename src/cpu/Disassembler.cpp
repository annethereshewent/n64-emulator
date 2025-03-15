#pragma once

#include <string>
#include <cstdint>
#include <array>
#include "CPU.hpp"

class CPU;

typedef std::string (*DisassembleInstruction)(CPU* cpu, uint32_t instruction);

std::string reserved(CPU* cpu, uint32_t instruction) {
    return "RESERVED";
}

std::string j(CPU* cpu, uint32_t instruction) {
    uint32_t offset = (instruction & 0x3ffffff) << 2;

    uint32_t address = (cpu->pc & 0xfffffffff0000000) | offset;

    return std::format("J 0x{:x}", address);
}

std::string mfc0(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("MFC0 r{}, r{}", rt, rd);
}
std::string dmfc0(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("DMFC0 r{}, r{}", rt, rd);
}

std::string mtc0(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("MTC0 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, cpu->r[rt]);
}

std::string dmtc0(CPU* cpu, uint32_t instruction) {
    uint32_t rd = CPU::getRd(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("DMTC0 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, cpu->r[rt]);
}

std::string tlbr(CPU* cpu, uint32_t instruction) {
    return "TLBR";
}

std::string tlbwi(CPU* cpu, uint32_t instruction) {
    return "TLBWI";
}

std::string tlbwr(CPU* cpu, uint32_t instruction) {
    return "TLBWR";
}

std::string tlbp(CPU* cpu, uint32_t instruction) {
    return "TLBP";
}

std::string eret(CPU* cpu, uint32_t instruction) {
    return "ERET";
}

std::string jal(CPU* cpu, uint32_t instruction) {
    uint32_t offset = (instruction & 0x3ffffff) << 2;

    uint32_t address = (cpu->pc & 0xfffffffff0000000) | offset;

    return std::format("JAL 0x{:x}", address);
}
std::string beq(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint32_t immediate = CPU::getImmediate(instruction);

    uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

    std::string taken = "No";

    if (cpu->r[rs] == cpu->r[rt]) {
        taken = "Yes";
    }

    return std::format(
        "BEQ r{}, r{}, 0x{:x} ; r{} = 0x{:x}, r{} = 0x{:x} (Taken: {})",
        rs,
        rt,
        amount,
        rs,
        cpu->r[rs],
        rt,
        cpu->r[rt],
        taken
    );
}
std::string bne(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint32_t immediate = CPU::getImmediate(instruction);
    uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

    std::string taken = "No";

    if (cpu->r[rs] != cpu->r[rt]) {
        taken = "Yes";
    }

    return std::format(
        "BNE r{}, r{}, 0x{:x} ; r{} = 0x{:x}, r{} = 0x{:x} (Taken: {})",
        rs,
        rt,
        amount,
        rs,
        cpu->r[rs],
        rt,
        cpu->r[rt],
        taken
    );
}
std::string blez(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] <= 0) {
        taken = "Yes";
    }

    return std::format("BLEZ r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bgtz(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] > 0) {
        taken = "Yes";
    }

    return std::format("BGTZ r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string addi(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);

    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("ADDI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, cpu->r[rs]);
}
std::string addiu(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);

    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("ADDIU r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, cpu->r[rs]);
}
std::string slti(CPU* cpu, uint32_t instruction) {
    std::string isTrue = "No";

    uint64_t rs = CPU::getRs(instruction);
    int64_t immediate = (int16_t)(int64_t)CPU::getImmediate(immediate);

    int64_t value = (int64_t)cpu->r[rs];

    if (value < immediate) {
        isTrue = "Yes";
    }

    return std::format("SLTI r{}, 0x{:x} ; r{} = 0x{:x} (True: {})", rs, immediate, rs, value, isTrue);
}
std::string sltiu(CPU* cpu, uint32_t instruction) {
    std::string isTrue = "No";

    uint64_t rs = CPU::getRs(instruction);
    int64_t immediate = CPU::getSignedImmediate(immediate);

    if (cpu->r[rs] < immediate) {
        isTrue = "Yes";
    }

    return std::format("SLTI r{}, 0x{:x} ; r{} = 0x{:x} (True: {})", rs, immediate, rs, cpu->r[rs], isTrue);
}
std::string andi(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = CPU::getImmediate(instruction);

    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("ANDI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, cpu->r[rs]);
}
std::string ori(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = CPU::getImmediate(instruction);

    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("ORI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, cpu->r[rs]);
}
std::string xori(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = CPU::getImmediate(instruction);

    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("XORI r{}, r{}, 0x{:x} ; r{} = 0x{:x}", rt, rs, immediate, rs, cpu->r[rs]);
}
std::string lui(CPU* cpu, uint32_t instruction) {
    uint32_t immediate = CPU::getImmediate(instruction);
    int rt = CPU::getRt(instruction);

    return std::format("LUI r{}, 0x{:x} ; r{} = 0x{:x}", rt, immediate, rt, cpu->r[rt]);
}
std::string beql(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint32_t immediate = CPU::getImmediate(instruction);

    uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

    std::string taken = "No";

    if (cpu->r[rs] == cpu->r[rt]) {
        taken = "Yes";
    }

    return std::format(
        "BEQL r{}, r{}, 0x{:x} ; r{} = 0x{:x}, r{} = 0x{:x} (Taken: {})",
        rs,
        rt,
        amount,
        rs,
        cpu->r[rs],
        rt,
        cpu->r[rt],
        taken
    );
}
std::string bnel(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    uint32_t immediate = CPU::getImmediate(instruction);
    uint64_t amount = (int16_t)(int64_t)(uint64_t)(immediate << 2);

    std::string taken = "No";

    if (cpu->r[rs] != cpu->r[rt]) {
        taken = "Yes";
    }

    return std::format(
        "BNEL r{}, r{}, 0x{:x} ; r{} = 0x{:x}, r{} = 0x{:x} (Taken: {})",
        rs,
        rt,
        amount,
        rs,
        cpu->r[rs],
        rt,
        cpu->r[rt],
        taken
    );
}
std::string blezl(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] <= 0) {
        taken = "Yes";
    }

    return std::format("BLEZL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bgtzl(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] > 0) {
        taken = "Yes";
    }

    return std::format("BGTZL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string daddi(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t immediate = CPU::getSignedImmediate(instruction);

    return std::format(
        "DADDI r{}, r{}, 0x{:x} ; r{} = 0x{:x}",
        rt,
        rs,
        immediate,
        rs,
        cpu->r[rs]
    );
}
std::string daddiu(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t immediate = CPU::getSignedImmediate(instruction);

    return std::format(
        "DADDIU r{}, r{}, 0x{:x} ; r{} = 0x{:x}",
        rt,
        rs,
        immediate,
        rs,
        cpu->r[rs]
    );
}
std::string ldl(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LDL r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string ldr(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LDR r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lb(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LB r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lh(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LH r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lwl(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LWL r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lw(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LW r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lbu(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LBU r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lhu(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LHU r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lwr(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LWR r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string lwu(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LWU r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string sb(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SB r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string sh(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SH r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string swl(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SWL r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string sw(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SW r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string sdl(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SDL r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string sdr(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SDR r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string swr(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SWR r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string cache(CPU* cpu, uint32_t instruction) {
    uint32_t cacheOp = CPU::getRt(instruction);

    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format(
        "CACHE 0x{:x}, 0x{:x}(r{}) ; r{} = 0x{:x}",
        cacheOp,
        offset,
        rs,
        rs,
        cpu->r[rs]
    );
}
std::string ll(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("LL r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, immediate, rs, rs, cpu->r[rs]);
}
std::string lwc1(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rd = CPU::getRt(instruction);

    return std::format("LWC1 r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rd, immediate, rs, rs, cpu->r[rs]);
}
std::string lld(CPU* cpu, uint32_t instruction) {
    return "TODO: LLD";
}
std::string ldc1(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rd = CPU::getRt(instruction);

    return std::format("LDC1 r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rd, immediate, rs, rs, cpu->r[rs]);
}
std::string ld(CPU* cpu, uint32_t instruction) {
    uint64_t offset = CPU::getSignedImmediate(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rs = CPU::getRs(instruction);

    return std::format("LD r{}, 0x{:x}(r{}) ; r{} = 0x{:x}", rt, offset, rs, rs, cpu->r[rs]);
}
std::string sc(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SC r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}
std::string swc1(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("SWC1 r{}, 0x{:x}(r{})", rt, immediate, baseReg);
}
std::string scd(CPU* cpu, uint32_t instruction) {
    return "TODO: SCD";
}
std::string sdc1(CPU* cpu, uint32_t instruction) {
    uint64_t immediate = CPU::getSignedImmediate(instruction);
    uint32_t baseReg = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("SDC1 r{}, 0x{:x}(r{})", rt, immediate, baseReg);
}
std::string sd(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);
    uint64_t rt = CPU::getRt(instruction);
    uint64_t offset = CPU::getSignedImmediate(instruction);

    return std::format(
        "SD r{}, 0x{:x}(r{}) ; r{} = 0x{:x}, r{} = 0x{:x}",
        rt,
        offset,
        rs,
        rt,
        cpu->r[rt],
        rs,
        cpu->r[rs]
    );
}

std::string sll(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    return std::format("SLL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, cpu->r[rt]);
}
std::string srl(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    return std::format("SRL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, cpu->r[rt]);
}
std::string sra(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    int64_t value = (int64_t)cpu->r[rt];

    return std::format("SRA r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, value);
}
std::string sllv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x1f;

    return std::format("SLLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string srlv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x1f;

    return std::format("SRLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string srav(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    int64_t value = (int64_t)cpu->r[rt];

    uint32_t shift = cpu->r[rs] & 0x1f;

    return std::format("SLLV r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, cpu->r[rs], rt, value);
}
std::string jr(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);

    return std::format("JR r{} ; r{} = 0x{:x}", rs, rs, cpu->r[rs]);
}
std::string jalr(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);

    return std::format("JALR r{} ; r{} = 0x{:x}", rs, rs, cpu->r[rs]);
}
std::string syscall(CPU* cpu, uint32_t instruction) {
    return "SYSCALL";
}
std::string break_(CPU* cpu, uint32_t instruction) {
    return "BREAK";
}
std::string sync(CPU* cpu, uint32_t instruction) {
    return "SYNC";
}
std::string mfhi(CPU* cpu, uint32_t instruction) {
    uint64_t rd = CPU::getRd(instruction);

    return std::format("MFHI r{} ; hi = 0x{:x}", rd, cpu->hi);
}
std::string mthi(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    return std::format("MTHI r{}, r{} = 0x{:x}", rs, rs, cpu->r[rs]);
}
std::string mflo(CPU* cpu, uint32_t instruction) {
    uint64_t rd = CPU::getRd(instruction);

    return std::format("MFLO r{} ; lo = 0x{:x}", rd, cpu->lo);
}
std::string mtlo(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    return std::format("MTLO r{}, r{} = 0x{:x}", rs, rs, cpu->r[rs]);
}
std::string dsllv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x3f;

    return std::format("DSLLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string dsrlv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x3f;

    return std::format("DSRLV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string dsrav(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t shift = cpu->r[rs] & 0x3f;

    int64_t value = (int64_t)cpu->r[rt];

    return std::format("DSRAV r{}, r{}, {} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rt, shift, rs, cpu->r[rs], rt, value);
}
std::string mult(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    int64_t rsVal = (int64_t)cpu->r[rs];
    int64_t rtVal = (int64_t)cpu->r[rt];

    return std::format("MULT r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, rsVal, rt, rtVal);
}
std::string multu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("MULTU r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string div(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    int64_t rsVal = (int64_t)cpu->r[rs];
    int64_t rtVal = (int64_t)cpu->r[rt];

    return std::format("DIV r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, rsVal, rt, rtVal);
}
std::string divu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("DIVU r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string dmult(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    int64_t rsVal = (int64_t)cpu->r[rs];
    int64_t rtVal = (int64_t)cpu->r[rt];

    return std::format("DMULT r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, rsVal, rt, rtVal);
}
std::string dmultu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("DMULTU r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string ddiv(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    int64_t rsVal = (int64_t)cpu->r[rs];
    int64_t rtVal = (int64_t)cpu->r[rt];

    return std::format("DDIV r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, rsVal, rt, rtVal);
}
std::string ddivu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);

    return std::format("DDIVU r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string add(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("ADD r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string addu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("ADDU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string sub(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("SUB r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string subu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("SUBU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string and_(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("AND r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string or_(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("OR r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string xor_(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("XOR r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string nor(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("NOR r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string slt(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    std::string isTrue = "No";

    int64_t rtVal = (int64_t)cpu->r[rt];
    int64_t rsVal = (int64_t)cpu->r[rs];

    if (rsVal < rtVal) {
        isTrue = "Yes";
    }

    return std::format("ADD r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x} (True: {})", rd, rs, rt, rs, rsVal, rt, rtVal, isTrue);
}
std::string sltu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    std::string isTrue = "No";

    if (cpu->r[rs] < cpu->r[rt]) {
        isTrue = "Yes";
    }

    return std::format("ADD r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x} (True: {})", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt], isTrue);
}
std::string dadd(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DADD r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string daddu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DADDU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
 }
std::string dsub(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DSUB r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string dsubu(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DSUBU r{}, r{}, r{} ; r{} = 0x{:x}, r{} = 0x{:x}", rd, rs, rt, rs, cpu->r[rs], rt, cpu->r[rt]);
}
std::string tge(CPU* cpu, uint32_t instruction) {
    return "TODO: TGE";
}
std::string tgeu(CPU* cpu, uint32_t instruction) {
    return "TODO: TGEU";
}
std::string tlt(CPU* cpu, uint32_t instruction) {
    return "TODO: TLT";
}
std::string tltu(CPU* cpu, uint32_t instruction) {
    return "TODO: TLTU";
}
std::string teq(CPU* cpu, uint32_t instruction) {
    return "TODO: TEQ";
}
std::string tne(CPU* cpu, uint32_t instruction) {
    return "TODO: TNE";
}
std::string dsll(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    return std::format("DSLL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, cpu->r[rt]);
}
std::string dsrl(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    return std::format("DSRL r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, cpu->r[rt]);
}
std::string dsra(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction);

    int64_t value = (int64_t)cpu->r[rt];

    return std::format("DSRA r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, value);
}
std::string dsll32(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction) + 32;

    return std::format("DSLL32 r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, cpu->r[rt]);
}
std::string dsrl32(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction) + 32;

    return std::format("DSRL32 r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, cpu->r[rt]);
}
std::string dsra32(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);
    uint32_t shift = CPU::shiftAmount(instruction) + 32;

    int64_t value = (int64_t)cpu->r[rt];

    return std::format("DSRA32 r{}, r{}, {} ; r{} = 0x{:x}", rd, rt, shift, rt, value);
}
std::string bltz(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] < 0) {
        taken = "Yes";
    }

    return std::format("BLTZ r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bgez(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] >= 0) {
        taken = "Yes";
    }

    return std::format("BGEZ r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bltzl(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] < 0) {
        taken = "Yes";
    }

    return std::format("BLTZL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bgezl(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] >= 0) {
        taken = "Yes";
    }

    return std::format("BGEZL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string tgei(CPU* cpu, uint32_t instruction) {
    uint32_t rs = CPU::getRs(instruction);

    int64_t immediate = (int64_t)CPU::getSignedImmediate(instruction);

    return std::format("TGEI, r{}, 0x{:x} ; r{} = 0x{:x}", rs, immediate, rs, cpu->r[rs]);
}
std::string tgeiu(CPU* cpu, uint32_t instruction) {
    return "TODO: TGEUI";
}
std::string tlti(CPU* cpu, uint32_t instruction) {
    return "TODO: TLTI";
}
std::string tltiu(CPU* cpu, uint32_t instruction) {
    return "TODO: TLTIU";
}
std::string teqi(CPU* cpu, uint32_t instruction) {
    return "TODO: TEQI";
}
std::string tnei(CPU* cpu, uint32_t instruction) {
    return "TODO: TNEI";
}
std::string bltzal(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] < 0) {
        taken = "Yes";
    }

    return std::format("BLTZAL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bgezal(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] >= 0) {
        taken = "Yes";
    }

    return std::format("BGEZAL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bltzall(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] < 0) {
        taken = "Yes";
    }

    return std::format("BLTZALL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string bgezall(CPU* cpu, uint32_t instruction) {
    uint64_t rs = CPU::getRs(instruction);

    std::string taken = "No";

    if ((int64_t)cpu->r[rs] >= 0) {
        taken = "Yes";
    }

    return std::format("BGEZALL r{} ; r{} = 0x{:x} (Taken: {})", rs, rs, cpu->r[rs], taken);
}
std::string mfc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint32_t returnVal;

    if (!cpu->cop0.status.fr) {
        returnVal = cpu->cop1.fgr32[rd];
    } else {
        returnVal = (uint32_t)cpu->cop1.fgr64[rd];
    }

    return std::format("MFC1 r{}, r{} ; r{} = 0x{:x}", rt, rd, rd, returnVal);
}
std::string dmfc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    uint64_t returnVal;

    if (!cpu->cop0.status.fr) {
        returnVal = (uint64_t)cpu->cop1.fgr32[rd] | (uint64_t)cpu->cop1.fgr32[rd + 1] << 32;
    } else {
        returnVal = cpu->cop1.fgr64[rd];
    }

    return std::format("MFC1 r{}, r{} ; r{} = 0x{:x}", rt, rd, rd, returnVal);
}
std::string cfc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("CFC1 r{}, r{}", rt, rd);
}
std::string dcfc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DCFC1 r{}, r{}", rt, rd);
}
std::string mtc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("MTC1 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, cpu->r[rt]);
}
std::string dmtc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DMTC1 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, cpu->r[rt]);
}
std::string ctc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("CTC1 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, cpu->r[rt]);
}
std::string dctc1(CPU* cpu, uint32_t instruction) {
    uint32_t rt = CPU::getRt(instruction);
    uint32_t rd = CPU::getRd(instruction);

    return std::format("DCTC1 r{}, r{} ; r{} = 0x{:x}", rd, rt, rt, cpu->r[rt]);
}
std::string cop1_b_instrs(CPU* cpu, uint32_t instruction) {
    return "COP1_B";
}
std::string cop1_s_instrs(CPU* cpu, uint32_t instruction) {
    return "COP1_S";
}
std::string cop1_d_instrs(CPU* cpu, uint32_t instruction) {
    return "COP1_D";
}
std::string cop1_w_instrs(CPU* cpu, uint32_t instruction) {
    return "COP1_W";
}
std::string cop1_l_instrs(CPU* cpu, uint32_t instruction) {
    return "COP1_L";
}

std::array<DisassembleInstruction, 64> instructions = {
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
    beql,
    bnel,
    blezl,
    bgtzl,
    daddi,
    daddiu,
    ldl,
    ldr,
    reserved,
    reserved,
    reserved,
    reserved,
    lb,
    lh,
    lwl,
    lw,
    lbu,
    lhu,
    lwr,
    lwu,
    sb,
    sh,
    swl,
    sw,
    sdl,
    sdr,
    swr,
    cache,
    ll,
    lwc1,
    reserved,
    reserved,
    lld,
    ldc1,
    reserved,
    ld,
    sc,
    swc1,
    reserved,
    reserved,
    scd,
    sdc1,
    reserved,
    sd,
};

std::array<DisassembleInstruction, 64> secondary = {
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
    syscall,
    break_,
    reserved,
    sync,
    mfhi,
    mthi,
    mflo,
    mtlo,
    dsllv,
    reserved,
    dsrlv,
    dsrav,
    mult,
    multu,
    div,
    divu,
    dmult,
    dmultu,
    ddiv,
    ddivu,
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
    dadd,
    daddu,
    dsub,
    dsubu,
    tge,
    tgeu,
    tlt,
    tltu,
    teq,
    reserved,
    tne,
    reserved,
    dsll,
    reserved,
    dsrl,
    dsra,
    dsll32,
    reserved,
    dsrl32,
    dsra32,
};

std::array<DisassembleInstruction, 32> registerInstructions = {
    bltz,
    bgez,
    bltzl,
    bgezl,
    reserved,
    reserved,
    reserved,
    reserved,
    tgei,
    tgeiu,
    tlti,
    tltiu,
    teqi,
    reserved,
    tnei,
    reserved,
    bltzal,
    bgezal,
    bltzall,
    bgezall,
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

std::array<DisassembleInstruction, 32> cop1Instructions = {
    mfc1,
    dmfc1,
    cfc1,
    dcfc1,
    mtc1,
    dmtc1,
    ctc1,
    dctc1,
    cop1_b_instrs,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    reserved,
    cop1_s_instrs,
    cop1_d_instrs,
    reserved,
    reserved,
    cop1_w_instrs,
    cop1_l_instrs,
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

std::string disassemble(CPU* cpu, uint32_t instruction) {
    uint32_t command = instruction >> 26;

    switch (command) {
        case 0:
            return secondary[instruction & 0x3f](cpu, instruction);
            break;
        case 1:
            return registerInstructions[(instruction >> 16) & 0x1f](cpu, instruction);
            break;
        case 16:
            switch ((instruction >> 21) & 0x1f) {
                case 0:
                    return mfc0(cpu, instruction);
                    break;
                case 1:
                    return dmfc0(cpu, instruction);
                    break;
                case 4:
                    return mtc0(cpu, instruction);
                    break;
                case 5:
                    return mfc0(cpu, instruction);
                    break;
                case 16:
                    switch (instruction & 0x3f) {
                        case 1:
                            return tlbr(cpu, instruction);
                            break;
                        case 2:
                            return tlbwi(cpu, instruction);
                            break;
                        case 6:
                            return tlbwr(cpu, instruction);
                            break;
                        case 8:
                            return tlbp(cpu, instruction);
                            break;
                        case 24:
                            return eret(cpu, instruction);
                        default:
                            return "RESERVED";
                            break;
                    }
                    break;
                default:
                    return "RESERVED";
                    break;
            }
            break;
        case 17:
            return cop1Instructions[(instruction >> 21) & 0x1f](cpu, instruction);
            break;
        case 18:
            throw std::runtime_error("not yet implemented: 18");
            break;
        default:
            return instructions[command](cpu, instruction);
            break;
    }

    return "";
}