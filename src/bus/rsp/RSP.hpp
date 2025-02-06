#pragma once

#include <cstdint>
#include "SPStatus.hpp"
#include "SPLength.hpp"

const uint32_t NUM_RSP_REGISTERS = 8;

enum DmaDirection {
    Read,
    Write
};

class SPDma {
public:
    DmaDirection direction = Read;
    SPLength length;
    uint32_t dramAddress = 0;
    uint32_t memAddress = 0;
};

class Bus;

class RSP;

typedef void (*RSPInstruction)(RSP* rsp, uint32_t instruction);


class RSP {
public:
    SPStatus status;

    uint32_t previousPc = 0;
    uint32_t pc = 0;
    uint32_t nextPc = 0;

    uint32_t semaphore;

    Bus& bus;

    RSP(Bus& bus) : bus(bus) {
        // gotten from https://github.com/hulkholden/n64js/blob/master/src/rsp.js#L147
        vecSelect = {
            0x76543210, // None
            0x76543210, // None
            0x66442200, // 0q
            0x77553311, // 1q
            0x44440000, // 0h
            0x55551111, // 1h
            0x66662222, // 2h
            0x77773333, // 3h
            0x00000000, // 0
            0x11111111, // 1
            0x22222222, // 2
            0x33333333, // 3
            0x44444444, // 4
            0x55555555, // 5
            0x66666666, // 6
            0x77777777  // 7
        };

        instructions = {
            RSP::reserved,
            RSP::reserved,
            RSP::j,
            RSP::jal,
            RSP::beq,
            RSP::bne,
            RSP::blez,
            RSP::bgtz,
            RSP::addi,
            RSP::addiu,
            RSP::slti,
            RSP::sltiu,
            RSP::andi,
            RSP::ori,
            RSP::xori,
            RSP::lui,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::lb,
            RSP::lh,
            RSP::reserved,
            RSP::lw,
            RSP::lbu,
            RSP::lhu,
            RSP::reserved,
            RSP::lwu,
            RSP::sb,
            RSP::sh,
            RSP::reserved,
            RSP::sw,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
        };

        secondary = {
            RSP::sll,
            RSP::reserved,
            RSP::srl,
            RSP::sra,
            RSP::sllv,
            RSP::reserved,
            RSP::srlv,
            RSP::srav,
            RSP::jr,
            RSP::jalr,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::break_,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::add,
            RSP::addu,
            RSP::sub,
            RSP::subu,
            RSP::and_,
            RSP::or_,
            RSP::xor_,
            RSP::nor,
            RSP::reserved,
            RSP::reserved,
            RSP::slt,
            RSP::sltu,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
            RSP::reserved,
        };
    };

    std::array<uint32_t, 32> r = {};
    std::array<uint8_t, 512> vpr = {};

    uint16_t vuVCO = 0;
    uint16_t vuVCC = 0;
    uint8_t vuVCE = 0;

    bool divDp = false;

    std::array<uint64_t, 8> vAcc = {};

    std::array<uint32_t, 16> vecSelect;

    bool cpuHalted = false;
    bool cpuBroken = false;
    bool isRunning = false;
    bool inDelaySlot = false;

    uint64_t cycleCounter = 0;

    uint16_t dmaMemAddress = 0;
    uint32_t dmaRamAddress = 0;
    SPLength spReadLength;
    SPLength spWriteLength;

    std::array<SPDma, 2> fifo;

    std::array<uint8_t, 0x1000> imem = {};
    std::array<uint8_t, 0x1000> dmem = {};

    std::array<RSPInstruction, 64> instructions = {};
    std::array<RSPInstruction, 64> secondary = {};

    uint32_t memRead32(uint8_t* ptr);
    void handleDma(SPDma dma);

    void pushDma(DmaDirection direction);
    void popDma();

    void updateStatus(uint32_t value);
    uint64_t runRsp();
    void startRsp();
    void restartRsp();

    uint32_t readRegisters(uint32_t offset);
    void writeRegisters(uint32_t offset, uint32_t value);

    static void reserved(RSP* rsp, uint32_t instruction);
    static void j(RSP* rsp, uint32_t instruction);
    static void jal(RSP* rsp, uint32_t instruction);
    static void beq(RSP* rsp, uint32_t instruction);
    static void bne(RSP* rsp, uint32_t instruction);
    static void blez(RSP* rsp, uint32_t instruction);
    static void bgtz(RSP* rsp, uint32_t instruction);
    static void addi(RSP* rsp, uint32_t instruction);
    static void addiu(RSP* rsp, uint32_t instruction);
    static void slti(RSP* rsp, uint32_t instruction);
    static void sltiu(RSP* rsp, uint32_t instruction);
    static void andi(RSP* rsp, uint32_t instruction);
    static void xori(RSP* rsp, uint32_t instruction);
    static void ori(RSP* rsp, uint32_t instruction);
    static void lui(RSP* rsp, uint32_t instruction);
    static void lb(RSP* rsp, uint32_t instruction);
    static void lh(RSP* rsp, uint32_t instruction);
    static void lw(RSP* rsp, uint32_t instruction);
    static void lbu(RSP* rsp, uint32_t instruction);
    static void lhu(RSP* rsp, uint32_t instruction);
    static void lwu(RSP* rsp, uint32_t instruction);
    static void sb(RSP* rsp, uint32_t instruction);
    static void sh(RSP* rsp, uint32_t instruction);
    static void sw(RSP* rsp, uint32_t instruction);

    static void sll(RSP* rsp, uint32_t instruction);
    static void srl(RSP* rsp, uint32_t instruction);
    static void sra(RSP* rsp, uint32_t instruction);
    static void sllv(RSP* rsp, uint32_t instruction);
    static void srlv(RSP* rsp, uint32_t instruction);
    static void srav(RSP* rsp, uint32_t instruction);
    static void jr(RSP* rsp, uint32_t instruction);
    static void jalr(RSP* rsp, uint32_t instruction);
    static void break_(RSP* rsp, uint32_t instruction);
    static void add(RSP* rsp, uint32_t instruction);
    static void addu(RSP* rsp, uint32_t instruction);
    static void sub(RSP* rsp, uint32_t instruction);
    static void subu(RSP* rsp, uint32_t instruction);
    static void and_(RSP* rsp, uint32_t instruction);
    static void or_(RSP* rsp, uint32_t instruction);
    static void xor_(RSP* rsp, uint32_t instruction);
    static void nor(RSP* rsp, uint32_t instruction);
    static void slt(RSP* rsp, uint32_t instruction);
    static void sltu(RSP* rsp, uint32_t instruction);

    static void mfc0(RSP* rsp, uint32_t instruction);
    static void mtc0(RSP* rsp, uint32_t instruction);
};