#pragma once

#include <cstdint>
#include "SIStatus.hpp"
#include "../DmaDirection.hpp"

class SerialInterface {
public:
    SIStatus status;
    uint32_t dramAddress = 0;
    DmaDirection dir = DmaDirection::None;

    void handleDma(Bus& bus);
    uint64_t processRam(Bus& bus);
    uint64_t processChannel(int channel, Bus& bus);
};