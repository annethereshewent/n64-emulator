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
            uint32_t value = std::byteswap(*(uint32_t*)&bus.pif.ram[i]);

            Bus::writeWord(&bus.rdram[dramAddress + i], value);
        }
    }
}

uint64_t SerialInterface::getPIFRamCycles(Bus& bus) {
    uint64_t activeChannels = 0;

    for (int i = 0; i < bus.pif.channels.size(); i++) {
        activeChannels += processChannel(i, bus);
    }

    return (24000 + (activeChannels * 30000));
}

uint64_t SerialInterface::processChannel(int channelId, Bus& bus) {
    if (bus.pif.channels[channelId].tx == -1) {
        return 0;
    }

    bus.pif.ram[bus.pif.channels[channelId].tx] &= 0x3f;
    bus.pif.ram[bus.pif.channels[channelId].rx] &= 0x3f;

    if (channelId == 0) {
        bus.pif.processController(channelId, bus);
    } else if (channelId == 4) {
       bus.pif.processCartridge(bus);
    } else {
        bus.pif.ram[bus.pif.channels[channelId].rx] |= 0x80;
        return 0;
    }

    return 1;
}