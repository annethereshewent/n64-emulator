#pragma once

#include <cstdint>
#include <array>
#include "AIStatus.hpp"

class Bus;

class AudioDma {
public:
    uint64_t address = 0;
    uint64_t length = 0;
    uint64_t duration = 0;
};

class AudioInterface {
public:

    Bus& bus;

    bool dmaReady = false;
    bool delayedCarry = false;
    AudioInterface(Bus& bus) : bus(bus) {};

    uint32_t dramAddress = 0;
    uint32_t audioLength = 0;
    uint32_t dacRate = 0;
    uint32_t bitRate = 0;

    uint32_t frequency = 33600;

    bool dmaEnable = false;

    AIStatus status;

    std::array<AudioDma, 2> fifo = {};

    void pushDma();
    void popDma();
    void handleDma();
    uint64_t getDuration();
};