#pragma once
#include "MIPSInterface.hpp"
#include "../../util/BitUtils.cpp"

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
        // mipsMask &= ~(1 << SP);
        clearBit(&mipsMask, SP);
    } else if (((value >> 1) & 0b1) == 1) {
        setBit(&mipsMask, SP);
    }
    if (((value >> 2) & 0b1) == 1) {
        clearBit(&mipsMask, SI);
    } else if (((value >> 3) & 0b1) == 1) {
        setBit(&mipsMask, SI);
    }
    if (((value >> 4) & 0b1) == 1) {
        clearBit(&mipsMask, AI);
    } else if (((value >> 5) & 0b1) == 1) {
        setBit(&mipsMask, AI);
    }
    if (((value >> 6) & 0b1) == 1) {
        clearBit(&mipsMask, VI);
    } else if (((value >> 7) & 0b1) == 1) {
        setBit(&mipsMask, VI);
    }
    if (((value >> 8) & 0b1) == 1) {
        clearBit(&mipsMask, PI);
    } else if (((value >> 9) & 0b1) == 1) {
        setBit(&mipsMask, PI);
    }
    if (((value >> 10) & 0b1) == 1) {
        clearBit(&mipsMask, DP);
    } else if (((value >> 11) & 0b1) == 1) {
        setBit(&mipsMask, DP);
    }
}