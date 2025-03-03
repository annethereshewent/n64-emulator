#pragma once

#include <cstdint>
#include <vector>
#include <SDL3/SDL.h>
#include "peripheral_interface/PeripheralInterface.hpp"
#include "rsp/RSP.hpp"
#include "video_interface/VideoInterface.hpp"
#include "audio_interface/AudioInterface.hpp"
#include "serial_interface/SerialInterface.hpp"
#include "rdp/RDPInterface.hpp"
#include "pif/PIF.hpp"
#include "rdram_interface/RDInterface.hpp"
#include "mips_interface/MIPSInterface.hpp"
#include "cache/ICache.hpp"
#include "cache/DCache.hpp"
#include "tlb/TlbEntry.hpp"
#include "../controller/Controller.hpp"
#include <iostream>
#include <fstream>

const uint32_t SP_INTERRUPT_FLAG = 1;
const uint32_t SI_INTERRUPT_FLAG = 1 << 1;
const uint32_t AI_INTERRUPT_FLAG = 1 << 2;
const uint32_t VI_INTERRUPT_FLAG = 1 << 3;
const uint32_t PI_INTERRUPT_FLAG = 1 << 4;
const uint32_t DP_INTERRUPT_FLAG = 1 << 5;

const uint32_t EEPROM_4K = 0x8000;
const uint32_t EEPROM_16K = 0xc000;

const uint32_t SRAM_SIZE = 0x8000;
const uint32_t FLASH_SIZE = 0x20000;

enum SaveType {
    Sram,
    Flash,
    Eeprom4k,
    Eeprom16k,
    Mempak,
    NoSave
};

class Bus {
public:
    std::vector<uint8_t> rdram = {};
    std::vector<bool> rdram9 = {};

    std::vector<TlbLut> tlbReadLut = {};
    std::vector<TlbLut> tlbWriteLut = {};
    std::vector<uint8_t> consoleBuffer = {};

    std::array<TlbEntry, 32> tlbEntries = {};

    std::vector<uint8_t> eeprom = {};
    std::vector<uint8_t> sram = {};
    std::vector<uint8_t> flash = {};

    std::fstream saveFile;

    uint64_t timeSinceSaveWrite = 0;

    char gameId[4];
    SaveType saveType = Eeprom4k;

    SDL_Gamepad* gamepad = nullptr;

    uint32_t input = 0;

    CPU& cpu;
    RSP rsp;

    Bus(CPU& cpu): cpu(cpu), rsp(*this), audioInterface(*this), rdp(*this), pif(*this) {
        rdram.resize(0x800000);
        rdram9.resize(0x800000);
        spdmem.resize(0x1000);

        tlbReadLut.resize(0x100000);
        tlbWriteLut.resize(0x100000);

        consoleBuffer.resize(0x10000);

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

    std::vector<uint8_t> cartridge = {};

    std::array<ICache, 512> icache;
    std::array<DCache, 512> dcache;

    PeripheralInterface peripheralInterface;
    RDPInterface rdp;
    AudioInterface audioInterface;
    VideoInterface videoInterface;

    SerialInterface serialInterface;

    RDInterface rdInterface;

    MIPSInterface mips;

    SDL_AudioStream* stream;

    uint64_t memRead64(uint64_t address);
    uint32_t memRead32(uint64_t address, bool ignoreCache = false, bool ignoreCycles = false);
    uint16_t memRead16(uint64_t address);
    uint8_t memRead8(uint64_t address);

    uint32_t readDataCache(uint64_t address, bool ignoreCycles = false);
    uint32_t readInstructionCache(uint64_t address);

    void formatEeprom();
    void loadRom(std::string filename);
    void openSave(std::string saveName);
    void writeSave();
    std::string getSaveName(std::string filename);

    void writeDataCache(uint64_t address, uint32_t value, int64_t mask = -1);
    bool dcacheHit(uint32_t lineIndex, uint64_t address);
    bool icacheHit(uint32_t lineIndex, uint64_t address);
    void fillDataCache(uint32_t lineIndex, uint64_t address, bool ignoreCycles = false);
    void fillInstructionCache(uint32_t lineIndex, uint64_t address);
    void dcacheWriteback(uint64_t line, bool ignoreCycles = false);

    void memWrite64(uint64_t address, uint64_t value, uint64_t mask = 0xffffffffffffffff);
    void memWrite32(uint64_t address, uint32_t value, bool ignoreCache = false, int64_t mask = -1);
    void memWrite16(uint64_t address, uint16_t value);
    void memWrite8(uint64_t address, uint8_t value);

    void pushSamples(uint64_t length, uint32_t dramAddress);

    void setInterrupt(uint32_t flag);
    void clearInterrupt(uint32_t flag);

    void dmaCartWrite();
    void handleRspDma(SPDma dma);

    void checkIrqs();

    void tlbRead(uint32_t index);
    void tlbWrite(uint32_t index);
    void tlbMap(uint32_t index);
    void tlbUnmap(uint32_t index);
    void tlbProbe();

    void recalculateDelay();
    void finishPiDma();

    void initAudio();
    void restartAudio();

    void updateButton(JoypadButton button, bool state);
    void updateAxis(uint8_t xAxis, uint8_t yAxis);

    uint64_t translateAddress(uint64_t address, bool isWrite = false);
    uint64_t getTlbAddress(uint64_t address, bool isWrite = false);

    std::string generateHash();
    void setCic();

    void writeRumblePak(int channel, uint16_t address, int data);

    static void writeValueLE(uint8_t* ptr, uint32_t value, int size);
    static void writeWord(uint8_t* ptr, uint32_t value);
    static void writeHalf(uint8_t* ptr, uint16_t value);
    static void writeDoubleWord(uint8_t* ptr, uint64_t value);
    static uint32_t calculateRdRamCycles(uint32_t length);

    static void writeWithMask32(uint32_t* oldVal, uint32_t newVal, uint32_t mask);
};