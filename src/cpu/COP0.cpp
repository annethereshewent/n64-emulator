#pragma once

#include "CPU.hpp"

COP0::COP0(CPU& cpu): cpu(cpu) {
    instructions = {
        COP0::mfc0,
        COP0::dmfc0,
        COP0::reserved,
        COP0::reserved,
        COP0::mtc0,
        COP0::dmtc0,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::cp0,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
    };

    cp0Instructions = {
        COP0::reserved,
        COP0::tlbr,
        COP0::tlbwi,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::tlbwr,
        COP0::reserved,
        COP0::tlbp,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::eret,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
        COP0::reserved,
    };

    config = 0x7006E463;
}

// mask values gotten mostly from https://github.com/gopher64
void COP0::writeRegister(uint32_t index, uint64_t value, Scheduler* scheduler) {
    latch = value;

    switch (index) {
        case 0:
            this->index = (this->index & ~0x8000003f) | ((uint32_t)value & 0x8000003f);
            break;
        case 2:
            entryLo0 = (entryLo0 & ~0x3fffffff) | (value & 0x3fffffff);
            break;
        case 3:
            entryLo1 = (entryLo1 & ~0x3fffffff) | (value & 0x3fffffff);
            break;
        case 4:
            context = (context & ~0xffffffffff800000) | (value & 0xffffffffff800000);
            break;
        case 5:
            pageMask = (pageMask & ~0x1ffe000) | ((uint32_t)value & 0x1ffe000);
            break;
        case 6:
            random = 31;
            wired = (wired & ~0x3f) | (uint32_t)value & 0x3f;
            break;
        case 9: {
            uint64_t newCount = value & 0xffffffff;
            newCount <<= 1;

            scheduler->rebaseEvents(count, newCount);

            count = newCount;
            break;
        }
        case 10:
            entryHi = (entryHi & ~0xc00000ffffffe0ff) | (value & 0xc00000ffffffe0ff);
            break;
        case 11: {
            compare = (uint32_t)value;
            uint32_t currentCount = count >> 1;

            uint32_t diff = value - currentCount;

            if (diff == 0) {
                diff = 0xffffffff;
            }

            uint64_t newCycles = (uint64_t)diff << 1;

            scheduler->addEvent(Event(CompareCount, count + newCycles));

            cause &= ~(1 << 15);
            pendingInterrupt = false;

            break;
        }
        case 12: {
            uint8_t oldFr = status.fr;
            status.value = (status.value & ~0xff57ffff) | (uint32_t)value & 0xff57ffff;

            if (oldFr != status.fr) {
                cpu.cop1.setCop1Registers(status);
            }
            break;
        }
        case 13:
            cause = (cause & ~0x300) | ((uint32_t)value & 0x300);
            break;
        case 14:
            epc = value;
            break;
        case 16:
            config = (config & ~0xf00800f) | (uint32_t)value & 0xf00800f;
            break;
        case 17:
            llAddress = (uint32_t)value;
            break;
        case 18:
            watchLo = (watchLo & ~0xfffffffb) | (uint32_t)value & 0xfffffffb;
            break;
        case 19:
            watchHi = (watchHi & ~0xf) | (uint32_t)value & 0xf;
            break;
        case 20:
            xContext = (xContext & ~0xFFFFFFFE00000000) | value & 0xFFFFFFFE00000000;
            break;
        case 26:
            parityError = (parityError & ~0xff) | (uint32_t)value & 0xff;
            break;
        case 28:
            tagLo = (tagLo & ~0xfffffc0) | (uint32_t)value & 0xfffffc0;
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
            return status.value;
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
            throw std::runtime_error("");
            break;
    }
}