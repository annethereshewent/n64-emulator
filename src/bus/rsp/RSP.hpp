#pragma once

#include <cstdint>
#include "SPStatus.hpp"
#include "SPLength.hpp"


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
            RSP::reserved, // SPECIAL
            RSP::reserved, // REGIMM
            RSP::j,        // 2
            RSP::jal,      // 3
            RSP::beq,      // 4
            RSP::bne,      // 5
            RSP::blez,     // 6
            RSP::bgtz,     // 7
            RSP::addi,     // 8
            RSP::addiu,    // 9
            RSP::slti,     // 10
            RSP::sltiu,    // 11
            RSP::andi,     // 12
            RSP::ori,      // 13
            RSP::xori,     // 14
            RSP::lui,      // 15
            RSP::reserved, // COP0
            RSP::reserved, // COP1
            RSP::reserved, // COP2
            RSP::reserved, // 19
            RSP::reserved, // 20
            RSP::reserved, // 21
            RSP::reserved, // 22
            RSP::reserved, // 23
            RSP::reserved, // 24
            RSP::reserved, // 25
            RSP::reserved, // 26
            RSP::reserved, // 27
            RSP::reserved, // 28
            RSP::reserved, // 29
            RSP::reserved, // 30
            RSP::reserved, // 31
            RSP::lb,       // 32
            RSP::lh,       // 33
            RSP::reserved, // 34
            RSP::lw,       // 35
            RSP::lbu,      // 36
            RSP::lhu,      // 37
            RSP::reserved, // 38
            RSP::lwu,      // 39
            RSP::sb,       // 40
            RSP::sh,       // 41
            RSP::reserved, // 42
            RSP::sw,       // 43
            RSP::reserved, // 44
            RSP::reserved, // 45
            RSP::reserved, // 46
            RSP::reserved, // 47
            RSP::reserved, // 48
            RSP::reserved, // 49
            RSP::reserved, // lwc2
            RSP::reserved, // 51
            RSP::reserved, // 52
            RSP::reserved, // 53
            RSP::reserved, // 54
            RSP::reserved, // 55
            RSP::reserved, // 56
            RSP::reserved, // 57
            RSP::reserved, // swc2
            RSP::reserved, // 59
            RSP::reserved, // 60
            RSP::reserved, // 61
            RSP::reserved, // 62
            RSP::reserved, // 63
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

    uint32_t memRead32(uint8_t* ptr);
    void handleDma(SPDma dma);

    void pushDma(DmaDirection direction);
    void popDma();

    void updateStatus(uint32_t value);
    uint64_t runRsp();
    void startRsp();

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
};