#pragma once

#include "SerialInterface.hpp"
#include <iostream>
#include "../Bus.hpp"

class Bus;

void SerialInterface::handleDma(Bus& bus) {
    if (dir == DmaDirection::Write) {
        for (int i = 0; i < bus.pif.ram.size(); i+= 4) {
            uint32_t data = std::byteswap(*(uint32_t*)&bus.rdram[dramAddress + i]);

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

    activeChannels += processChannel(0, bus);

    return (24000 + (activeChannels * 30000));
}

uint64_t SerialInterface::processChannel(int _channelId, Bus& bus) {
    if (bus.pif.channel.tx == -1) {
        return 0;
    }


    return 1;
}