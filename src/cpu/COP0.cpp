#pragma once

#include "CPU.hpp"

COP0::COP0() {
    instructions = {
        COP0::mfc0,     // 0
        COP0::dmfc0,    // 1
        COP0::reserved, // 2
        COP0::reserved, // 3
        COP0::mtc0,     // 4
        COP0::dmtc0,    // 5
        COP0::reserved, // 6
        COP0::reserved, // 7
        COP0::reserved, // 8
        COP0::reserved, // 9
        COP0::reserved, // a
        COP0::reserved, // b
        COP0::reserved, // c
        COP0::reserved, // d
        COP0::reserved, // e
        COP0::reserved, // f
        COP0::cp0,      // 10
        COP0::reserved, // 11
        COP0::reserved, // 12
        COP0::reserved, // 13
        COP0::reserved, // 14
        COP0::reserved, // 15
        COP0::reserved, // 16
        COP0::reserved, // 17
        COP0::reserved, // 18
        COP0::reserved, // 19
        COP0::reserved, // 1a
        COP0::reserved, // 1b
        COP0::reserved, // 1c
        COP0::reserved, // 1d
        COP0::reserved, // 1e
        COP0::reserved, // 1f
    };

    cp0Instructions = {
        COP0::reserved, // 0
        COP0::tlbr,     // 1
        COP0::tlbwi,    // 2
        COP0::reserved, // 3
        COP0::reserved, // 4
        COP0::reserved, // 5
        COP0::tlbwr,    // 6
        COP0::reserved, // 7
        COP0::tlbp,     // 8
        COP0::reserved, // 9
        COP0::reserved, // 10
        COP0::reserved, // 11
        COP0::reserved, // 12
        COP0::reserved, // 13
        COP0::reserved, // 14
        COP0::reserved, // 15
        COP0::reserved, // 16
        COP0::reserved, // 17
        COP0::reserved, // 18
        COP0::reserved, // 19
        COP0::reserved, // 20
        COP0::reserved, // 21
        COP0::reserved, // 22
        COP0::reserved, // 23
        COP0::eret,     // 24
        COP0::reserved, // 25
        COP0::reserved, // 26
        COP0::reserved, // 27
        COP0::reserved, // 28
        COP0::reserved, // 29
        COP0::reserved, // 30
        COP0::reserved, // 31
    };
}

// mask values gotten mostly from https://github.com/gopher64
void COP0::writeRegister(uint32_t index, uint64_t value) {
    latch = value;

    // TODO: find out which of these registers are written to outside of here
    // ie: "cause" is definitely written to outside of here, so don't want to overwrite its values
    switch (index) {
        case 0:
            this->index = (uint32_t)value & 0x8000003f;
            break;
        case 2:
            entryLo0 = value & 0x3fffffff;
            break;
        case 3:
            entryLo1 = value & 0x3fffffff;
            break;
        case 4:
            context = value & 0xffffffffff800000;
            break;
        case 5:
            pageMask = (uint32_t)value & 0x1ffe000;
            break;
        case 6:
            random = 31;
            wired = (uint32_t)value & 0x3f;
            break;
        case 9:
            count = (uint32_t)value;
            count <<= 1;

            // TODO: rebase scheduler cycles after setting count to this value.
            break;
        case 10:
            entryHi = value & 0xc00000ffffffe0ff;
            break;
        case 11:
            compare = (uint32_t)value;
            // TODO: do some additional stuff here related to compare register once scheduler is in place.
            break;
        case 12:
            status = (uint32_t)value & 0xff57ffff;
            // TODO: check if fgr registers need to be set
            break;
        case 13:
            cause = (cause & ~0x300) | ((uint32_t)value & 0x300);
            break;
        case 14:
            epc = value;
            break;
        case 16:
            config = (uint32_t)value & 0xf00800f;
            break;
        case 17:
            llAddress = (uint32_t)value;
            break;
        case 18:
            watchLo = (uint32_t)value & 0xfffffffb;
            break;
        case 19:
            watchHi = (uint32_t)value & 0xf;
            break;
        case 20:
            xContext = value & 0xFFFFFFFE00000000;
            break;
        case 26:
            parityError = (uint32_t)value & 0xff;
            break;
        case 28:
            tagLo = (uint32_t)value & 0xfffffc0;
        case 30:
            errorEpc = value;
            break;
        case 1:
        case 7:
        case 8:
        case 15:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 27:
        case 29:
        case 31:
            // read only or unused
            break;
    }
}

void COP0::addCycles(uint32_t cycles) {
    count += cycles;
}

uint64_t COP0::readRegister(uint32_t index) {
    switch (index) {
        case 0:
            return this->index;
            break;
        case 1: {
            // algorithm gotten from https://github.com/gopher64
            uint64_t maxValue = 0xffffffffffffffff;
            if (wired > 31) {
                return (maxValue - count) & 0x3F;
            }
            return (maxValue - count)
                % (32 - wired)
                + wired;
            break;
        }
        case 2:
            return entryLo0;
            break;
        case 3:
            return entryLo1;
            break;
        case 4:
            return context;
            break;
        case 5:
            return pageMask;
            break;
        case 6:
            return wired;
            break;
        case 8:
            return badVAddress;
            break;
        case 9:
            return count >> 1;
            break;
        case 10:
            return entryHi;
            break;
        case 11:
            return compare;
            break;
        case 12:
            return status;
            break;
        case 13:
            return cause;
            break;
        case 14:
            return epc;
            break;
        case 15:
            return prid;
            break;
        case 16:
            return config;
            break;
        case 17:
            return llAddress;
            break;
        case 18:
            return watchLo;
            break;
        case 19:
            return watchHi;
            break;
        case 20:
            return xContext;
            break;
        case 26:
            return parityError;
            break;
        case 27:
            return cacheError;
            break;
        case 28:
            return tagLo;
            break;
        case 29:
            return tagHi;
            break;
        case 30:
            return errorEpc;
            break;
        case 7:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 31:
            return latch;
            break;
        default:
            std::cout << "invalid register given for cop0: " << index << "\n";
            exit(1);
            break;
    }
}