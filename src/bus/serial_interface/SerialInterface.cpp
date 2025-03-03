#pragma once

#include "SerialInterface.hpp"
#include <iostream>
#include "../Bus.hpp"

class Bus;

void SerialInterface::handleDma(Bus& bus) {
    if (dir == DmaDirection::Write) {
        for (int i = 0; i < bus.pif.ram.size(); i+= 4) {
            uint32_t data = std::byteswap(*(uint32_t*)&bus.rdram[dramAddress + i]);

            // Bus::writeWord(&bus.pif.ram[i], data);
            memcpy(&bus.pif.ram[i], &data, sizeof(uint32_t));
        }
    } else {
        for (int i = 0; i < bus.pif.ram.size(); i+= 4) {
            uint32_t value = std::byteswap(*(uint32_t*)&bus.pif.ram[i]);

            // Bus::writeValueLE(&bus.rdram[dramAddress + i], value, 4);
            memcpy(&bus.rdram[dramAddress + i], &value, sizeof(uint32_t));
        }
    }
}

uint64_t SerialInterface::processRam(Bus& bus) {
    uint64_t activeChannels = 0;

    for (int i = 0; i < bus.pif.channels.size(); i++) {
        activeChannels += processChannel(i, bus);
    }

    return (24000 + (activeChannels * 30000));
}

uint64_t SerialInterface::processChannel(int channelId, Bus& bus) {
    if (bus.pif.channels[channelId].tx == -1
        || bus.pif.channels[channelId].txBuf == -1
        || bus.pif.channels[channelId].rx == -1
        || bus.pif.channels[channelId].rxBuf == -1
    ) {
        return 0;
    }

    bus.pif.ram[bus.pif.channels[channelId].tx] &= 0x3f;
    bus.pif.ram[bus.pif.channels[channelId].rx] &= 0x3f;

    // TODO: process other controllers, but for now just do the first controller
    if (channelId == 0) {
        bus.pif.processController(channelId);
    } else if (channelId == CART_CHANNEL) {
        bus.pif.processCartridge();
    } else {
        bus.pif.ram[bus.pif.channels[channelId].rx] |= 0x80;
        return 0;
    }

    return 1;
}