#include "MIPSInterface.hpp"

void MIPSInterface::write(uint32_t value) {
    repeatCount = value & 0x7f;

    if ((value >> 7 & 0x1) == 1) {
        repeatMode = false;
    }
    if ((value >> 8 & 0x1) == 1) {
        repeatMode = true;
        // TODO: clear repeatMode after rambus write
    }
    if ((value >> 9 & 0x1) == 1) {
        ebus = false;
    }
    if ((value >> 10 & 0x1) == 1) {
        ebus = true;
    }
     // TODO: clear dp interrupt

     if ((value >> 12 & 0x1) == 1) {
        upperMode = false;
     }
     if ((value >> 13 & 0x1) == 1) {
        upperMode = true;
     }
}