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

class RSP {
public:
    SPStatus status;

    uint32_t previousPc = 0;
    uint32_t pc = 0;
    uint32_t nextPc = 0;

    Bus& bus;

    RSP(Bus& bus) : bus(bus) {
        // gotten from https://github.com/hulkholden/n64js/blob/master/src/rsp.js#L249
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
    };

    std::array<uint32_t, 32> r;
    std::array<uint8_t, 512> vpr;

    uint16_t vuVCO = 0;
    uint16_t vuVCC = 0;
    uint8_t vuVCE = 0;

    bool divDp = false;

    std::array<uint64_t, 8> vAcc = {};

    std::array<uint32_t, 16> vecSelect;

    bool cpuHalted = false;
    bool cpuBroken = false;
    bool isRunning = false;

    uint64_t cycleCounter = 0;

    uint16_t dmaMemAddress = 0;
    uint32_t dmaRamAddress = 0;
    SPLength spReadLength;
    SPLength spWriteLength;

    std::array<SPDma, 2> fifo;

    std::array<uint8_t, 0x1000> imem = {};
    std::array<uint8_t, 0x1000> dmem = {};

    uint32_t imemRead32(uint32_t address);
    void handleDma(SPDma dma);

    void pushDma(DmaDirection direction);
    void popDma();

    void updateStatus(uint32_t value);
    uint64_t runRsp();
    void startRsp();
};