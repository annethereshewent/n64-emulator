#pragma once
#include "MIPSInterface.hpp"

void MIPSInterface::write(uint32_t value) {
    repeatCount = value & 0x7f;

    if ((value >> 7 & 0x1) == 1) {
        repeatMode = false;
    }
    if ((value >> 8 & 0x1) == 1) {
        repeatMode = true;
    }
    if ((value >> 9 & 0x1) == 1) {
        ebus = false;
    }
    if ((value >> 10 & 0x1) == 1) {
        ebus = true;
    }

     if ((value >> 12 & 0x1) == 1) {
        upperMode = false;
     }
     if ((value >> 13 & 0x1) == 1) {
        upperMode = true;
     }
}

void MIPSInterface::setMask(uint32_t value) {
    if ((value & 0b1) == 1) {
        mipsMask.spInterrupt = 0;
    }
    if (((value >> 1) & 0b1) == 1) {
        mipsMask.spInterrupt = 1;
    }
    if (((value >> 2) & 0b1) == 1) {
        mipsMask.siInterrupt = 0;
    }
    if (((value >> 3) & 0b1) == 1) {
        mipsMask.siInterrupt = 1;
    }
    if (((value >> 4) & 0b1) == 1) {
        mipsMask.aiInterrupt = 0;
    }
    if (((value >> 5) & 0b1) == 1) {
        mipsMask.aiInterrupt = 1;
    }
    if (((value >> 6) & 0b1) == 1) {
        mipsMask.viInterrupt = 0;
    }
    if (((value >> 7) & 0b1) == 1) {
        mipsMask.viInterrupt = 1;
    }
    if (((value >> 8) & 0b1) == 1) {
        mipsMask.piInterrupt = 0;
    }
    if (((value >> 9) & 0b1) == 1) {
        mipsMask.piInterrupt = 1;
    }
    if (((value >> 10) & 0b1) == 1) {
        mipsMask.dpInterrupt = 0;
    }
    if (((value >> 11) & 0b1) == 1) {
        mipsMask.dpInterrupt = 1;
    }
}