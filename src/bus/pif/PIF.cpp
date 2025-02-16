#pragma once

#include "PIF.hpp"
#include <iostream>

class Bus;

PIF::PIF() {
    // TODO: actually calculate these. it's hardcoded to get super mario 64 booting for now.
    ram = {};
    ram[0x26] = 0x3f;
    ram[0x27] = 0x3f;
}

void PIF::executeCommand() {
    uint8_t commandByte = ram[0x3f];

    uint8_t mask = 0;
    if (commandByte & 0x1) {
        mask |= 0x1;
        std::cout << "received command 0x1\n";

        setupChannels();
    }
    if (commandByte & 0x2) {
        mask |= 0x2;
        std::cout << "received command 0x2\n";
    }
    if (commandByte & 0x8) {
        mask |= 0x8;
        std::cout << "received command 0x8\n";
    }
    if (commandByte & 0x10) {
        for (uint8_t& byte: rom) {
            byte = 0;
        }

        std::cout << "received command 0x10\n";
    }
    if (commandByte & 0x20) {
        std::cout << "received command 0x20\n";
        ram[0x3f] = 0x80;
    }

    ram[0x3f] &= ~mask;
}

void PIF::setupChannels() {
    int i = 0;
    int j = 0;

    while (i < ram.size() && j < channels.size()) {
        switch (ram[i]) {
            case 0:
                disablePIFChannel(j);
                i++;
                j++;
                break;
            case 0xff:
                i++;
                break;
            case 0xfe:
                while (j < channels.size()) {
                    disablePIFChannel(j);
                    j++;
                }
                break;
            case 0xfd:
                disablePIFChannel(j);
                j++;
                i++;
                break;
            default:
                if (i + 1 < ram.size() && ram[i + 1] == 0xfe) {
                    i++;
                    continue;
                }

                if (i + 2 >= ram.size()) {
                    i = ram.size();
                    continue;
                }

                i += setupPIFChannel(j, i);
                j++;

                break;
        }
    }
}

int PIF::setupPIFChannel(int channel, int buf) {
    uint8_t tx = ram[buf] & 0x3f;
    uint8_t rx = ram[buf + 1] & 0x3f;

    channels[channel].tx = buf;
    channels[channel].rx = buf + 1;
    channels[channel].txBuf = buf + 2;
    channels[channel].rxBuf = buf + 2 + tx;

    return 2 + tx + rx;
}

void PIF::disablePIFChannel(int channel) {
    channels[channel].tx = -1;
    channels[channel].txBuf = -1;
    channels[channel].rx = -1;
    channels[channel].rxBuf = -1;
}

void PIF::processCartridge(Bus& bus) {
    uint32_t command = ram[channels[4].txBuf];


    switch (command) {
        case 0:
        case 0xff: {
            if (bus.saveType != -1) {
                ram[channels[4].rxBuf] = (uint8_t)bus.saveType;
                ram[channels[4].rxBuf + 1] = (uint8_t)(bus.saveType >> 8);
                ram[channels[4].rxBuf + 2] = 0;
            } else {
                ram[channels[4].rxBuf] |= 0x80;
            }
            break;
        }
        case 4:
            Bus::writeWord(&ram[channels[4].rxBuf + 1], 0xffffffff);
            break;
        case 5:
            std::cout << "TODO: EEPROM_WRITE\n";
            exit(1);
            break;
        case 6:
            std::cout << "TODO: RTC_STATUS\n";
            exit(1);
            break;
        case 7:
            std::cout << "TODO: RTC_READ\n";
            exit(1);
            break;
        case 8:
            std::cout << "TODO: RTC_WRITE\n";
            exit(1);
            break;
        default:
            std::cout << "unknown command received for processCartridge: " << std::hex << command << "\n";
            exit(1);
            break;
    }
}

void PIF::processController(int channel, Bus& bus) {
    if (channels[channel].txBuf == -1) {
        std::cout << "found a value of -1 for txBuf, shouldn't happen\n";
        exit(1);
    }
    uint8_t command = ram[channels[channel].txBuf];

    switch (command) {
        case 0x0:
        case 0xff:
            ram[channels[channel].rxBuf] = 5; // (1 | (1 << 2))
            ram[channels[channel].rxBuf + 1] = 0; // (the above value >> 8)

            ram[channels[channel].rxBuf + 2] = 1;  // controller pak present
            break;
        case 0x1:
            std::cout << "controller command not implemented: 0x1\n";
            exit(1);
            break;
        case 0x2:
            std::cout << "controller command not implemented: 0x2\n";
            exit(1);
            break;
        case 0x3:
            std::cout << "controller command not implemented: 0x3\n";
            exit(1);
            break;
    }
}