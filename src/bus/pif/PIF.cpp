#pragma once

#include "PIF.hpp"
#include <iostream>

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
         // TODO: implement controllers command
        std::cout << "received command 0x1\n";
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