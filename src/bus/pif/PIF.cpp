#pragma once

#include "PIF.hpp"
#include <iostream>

void PIF::executeCommand() {
    uint8_t commandByte = ram[0x3f];

    if (commandByte & 0x1) {
        std::cout << "received command 0x1\n";
    }
    if (commandByte & 0x2) {
        std::cout << "received command 0x2\n";
    }
    if (commandByte & 0x8) {
        std::cout << "received command 0x8\n";
    }
    if (commandByte & 0x10) {
        // TODO: implement controllers command
        std::cout << "received command 0x10\n";
    }
    if (commandByte & 0x20) {
        std::cout << "received command 0x20\n";
        ram[0x3f] = 0x80;
    }
}