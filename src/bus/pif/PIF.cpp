#pragma once

#include <iostream>
#include "PIF.hpp"
#include "../Bus.hpp"

class Bus;

const uint32_t CART_CHANNEL = 4;

void PIF::executeCommand() {
    uint8_t commandByte = ram[0x3f];

    uint8_t mask = 0;
    if (commandByte & 0x1) {
        mask |= 0x1;
        // std::cout << "received command 0x1\n";

        setupChannels();
    }
    if (commandByte & 0x2) {
        // TODO: cic challenge
        mask |= 0x2;
        throw std::runtime_error("received command 0x2");
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

void PIF::processCartridge() {
    uint8_t command = ram[channels[CART_CHANNEL].txBuf];

    switch (command) {
        case 0:
        case 0xff: {
            uint16_t saveType;

            if (std::find(bus.saveTypes.begin(), bus.saveTypes.end(), Eeprom16k) != bus.saveTypes.end()) {
                saveType = EEPROM_16K;
            } else if (std::find(bus.saveTypes.begin(), bus.saveTypes.end(), Eeprom4k) != bus.saveTypes.end()) {
                saveType = EEPROM_4K;
            } else {
                ram[channels[CART_CHANNEL].rx] |= 0x80;
                return;
            }

            ram[channels[CART_CHANNEL].rxBuf] = (uint8_t)saveType;
            ram[channels[CART_CHANNEL].rxBuf + 1] = (uint8_t)(saveType >> 8);
            ram[channels[CART_CHANNEL].rxBuf + 2] = 0;
            break;
        }
        case 4:
            readEeprom();
            break;
        case 5:
            writeEeprom();
            break;
        case 6:
            throw std::runtime_error("TODO: RTC_STATUS)");
            break;
        case 7:
            throw std::runtime_error("TODO: RTC_READ");
            break;
        case 8:
            throw std::runtime_error("TODO: RTC_WRITE");
            break;
        default:
            std::cout << "unknown command received for processCartridge: " << std::hex << command << "\n";
            throw std::runtime_error("");
            break;
    }
}

void PIF::processController(int channel) {
    if (channels[channel].txBuf == -1) {
        throw std::runtime_error("found a value of -1 for txBuf, shouldn't happen");
    }

    uint8_t command = ram[channels[channel].txBuf];

    switch (command) {
        case 0x0:
        case 0xff:
            // TODO: make constants for these values
            ram[channels[channel].rxBuf] = 5; // (1 | (1 << 2))
            ram[channels[channel].rxBuf + 1] = 0; // (the above value >> 8)

            ram[channels[channel].rxBuf + 2] = 1;  // controller pak present
            break;
        case 0x1:
            memcpy(&ram[channels[channel].rxBuf], &bus.input, sizeof(uint32_t));
            break;
        case 0x2:

            readPakBlock(
                channels[channel].txBuf + 1,
                channels[channel].rxBuf,
                channels[channel].rxBuf + 32,
                channel
            );
            break;
        case 0x3:
            writePakBlock(
                channels[channel].txBuf + 1,
                channels[channel].txBuf + 3,
                channels[channel].rxBuf,
                channel
            );
            break;
    }
}

uint8_t PIF::getCrc(int channel, uint16_t address, int data) {
    uint8_t crc = 0;

    for (int i = 0; i <= 32; i++) {

        for (int mask = 0x80; mask >= 1; mask >>= 1) {
            uint8_t xorT = (crc & 0x80)!= 0 ? 0x85 : 0;
            crc <<= 1;
            if (i != 32 && (ram[data + i] & mask) != 0) {
                crc |= 1;
            }
            crc ^= xorT;
        }
    }

    return crc;
}

void PIF::readPakBlock(int addrAcrc, int data, int dcrc, int channel) {
    uint16_t address = (uint16_t)ram[addrAcrc] << 8 | (uint16_t)(ram[addrAcrc + 1] & 0xe0);

    uint8_t value = address >= 0x8000 && address <= 0x9000 ? 0x80 : 0;

    for (int i = 0; i < 32; i++) {
        ram[data + i] = value;
    }

    ram[dcrc] = getCrc(channel, address, data);
}

void PIF::writePakBlock(int addrAcrc, int data, int dcrc, int channel) {
    uint16_t address = (uint16_t)ram[addrAcrc] << 8 | (uint16_t)(ram[addrAcrc + 1] & 0xe0);

    // TODO: implement other kinds of paks. but rumble pak should be fine for now
    bus.writeRumblePak(channel, address, data);

    ram[dcrc] = getCrc(channel, address, data);
}

void PIF::readEeprom() {
    bus.formatEeprom();

    uint32_t address = ram[channels[CART_CHANNEL].txBuf + 1] * 8;

    memcpy(&ram[channels[CART_CHANNEL].rxBuf], &bus.eeprom[address], 8);
}

void PIF::writeEeprom() {
    bus.formatEeprom();

    uint32_t address = ram[channels[CART_CHANNEL].txBuf + 1] * 8;

    memcpy(&bus.eeprom[address], &ram[channels[CART_CHANNEL].txBuf + 2], 8);

    ram[channels[CART_CHANNEL].rxBuf] = 0;

    auto p1 = std::chrono::system_clock::now();

    bus.timeSinceSaveWrite = std::chrono::duration_cast<std::chrono::milliseconds>(
        p1.time_since_epoch()).count();
}