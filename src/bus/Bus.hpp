#pragma once

#include <cstdint>
#include <vector>
#include "peripheral_interface/PeripheralInterface.hpp"
#include "rsp/RSP.hpp"
#include "video_interface/VideoInterface.hpp"
#include "audio_interface/AudioInterface.hpp"
#include "serial_interface/SerialInterface.hpp"
#include "rdp/RDP.hpp"
#include "pif/PIF.cpp"
#include "rdram_interface/RDInterface.hpp"
#include "mips_interface/MIPSInterface.hpp"
#include "cache/ICache.hpp"
#include "cache/DCache.hpp"
#include "tlb/TlbEntry.hpp"

const uint32_t SP_INTERRUPT_FLAG = 1;
const uint32_t SI_INTERRUPT_FLAG = 1 << 1;
const uint32_t AI_INTERRUPT_FLAG = 1 << 2;
const uint32_t VI_INTERRUPT_FLAG = 1 << 3;
const uint32_t PI_INTERRUPT_FLAG = 1 << 4;

class Bus {
public:
    std::vector<uint8_t> rdram;
    std::vector<bool> rdram9;

    std::vector<TlbLut> tlbReadLut;
    std::vector<TlbLut> tlbWriteLut;

    std::array<TlbEntry, 32> tlbEntries;

    CPU& cpu;
    RSP rsp;

    Bus(CPU& cpu): cpu(cpu), rsp(*this) {
        rdram.resize(0x800000);
        rdram9.resize(0x800000);
        spdmem.resize(0x1000);

        tlbReadLut.resize(0x100000);
        tlbWriteLut.resize(0x100000);

        rsp.status.value = 1;

        for (int i = 0; i < icache.size(); i++) {
            icache[i].index = (uint16_t)(i << 5) & 0xfe0;
        }
        for (int i = 0; i < dcache.size(); i++) {
            dcache[i].index = (uint16_t)(i << 4) & 0xff0;
        }
    };

    std::vector<uint8_t> spdmem;

    PIF pif;

    std::vector<uint8_t> cartridge;

    std::array<ICache, 512> icache;
    std::array<DCache, 512> dcache;

    PeripheralInterface peripheralInterface;
    RDP rdp;
    AudioInterface audioInterface;
    VideoInterface videoInterface;

    SerialInterface serialInterface;

    RDInterface rdInterface;

    MIPSInterface mips;

    uint64_t memRead64(uint64_t address);
    uint32_t memRead32(uint64_t address, bool ignoreCache = false);
    uint16_t memRead16(uint64_t address);
    uint8_t memRead8(uint64_t address);

    uint32_t readDataCache(uint64_t address);
    void writeDataCache(uint64_t address, uint32_t value, int64_t mask = -1);
    bool dcacheHit(uint32_t lineIndex, uint64_t address);
    void fillDataCache(uint32_t lineIndex, uint64_t address);
    void dcacheWriteback(uint64_t line);

    void memWrite64(uint64_t address, uint64_t value);
    void memWrite32(uint64_t address, uint32_t value, bool ignoreCache = false);
    void memWrite16(uint64_t address, uint16_t value);
    void memWrite8(uint64_t address, uint8_t value);

    void setInterrupt(uint32_t flag);
    void clearInterrupt(uint32_t flag);

    void dmaWrite();
    void handleRspDma(SPDma dma);

    void checkIrqs();

    void tlbRead(uint32_t index);
    void tlbWrite(uint32_t index);
    void tlbMap(uint32_t index);
    void tlbUnmap(uint32_t index);
    void tlbProbe();

    void recalculateDelay();
    void finishPiDma();

    static uint64_t translateAddress(uint64_t address);

    static void writeWord(uint8_t* ptr, uint32_t value);
    static void writeHalf(uint8_t* ptr, uint16_t value);
    static void writeDoubleWord(uint8_t* ptr, uint64_t value);
    static uint32_t calculateRdRamCycles(uint32_t length);
};