#pragma once

#include "SerialInterface.hpp"
#include <iostream>
#include "../Bus.hpp"

class Bus;

void SerialInterface::handleDma(Bus& bus) {
    if (dir == DmaDirection::Write) {
        for (int i = 0; i < bus.pif.ram.size(); i+= 4) {
            uint32_t data = *(uint32_t*)&bus.rdram[dramAddress + i];

            Bus::writeWord(&bus.pif.ram[i], data);
        }
    } else {
        for (int i = 0; i < bus.pif.ram.size(); i+= 4) {
            uint32_t value = *(uint32_t*)&bus.pif.ram[i];

            Bus::writeWord(&bus.rdram[dramAddress + i], value);
        }
    }
}

uint64_t SerialInterface::getPIFRamCycles(Bus& bus) {
    uint64_t activeChannels = 0;

    for (int i = 0; i < 5; i++) {
        activeChannels += processChannel(i, bus);
    }

    return (24000 + (activeChannels * 30000));
}

uint64_t SerialInterface::processChannel(int channel, Bus& bus) {
    return 0;
}