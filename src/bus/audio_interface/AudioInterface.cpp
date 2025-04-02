#pragma once

#include "AudioInterface.hpp"
#include "../Bus.hpp"
#include "../../cpu/CPU.hpp"

void AudioInterface::pushDma() {
    AudioDma dma;
    if (((status >> Busy) & 0b1) == 1) {
        dma.address = dramAddress;
        dma.duration = getDuration();
        dma.length = audioLength;

        fifo[1] = dma;

        setBit(&status, Full);
        setBit(&status, Full2);
    } else {
        dma.address = dramAddress;
        dma.duration = getDuration();
        dma.length = audioLength;

        fifo[0] = dma;

        handleDma();

        setBit(&status, Busy);
    }
}

void AudioInterface::popDma() {
    if (((status >> Full) & 0b1) == 1) {
        fifo[0] = fifo[1];

        clearBit(&status, Full);
        clearBit(&status, Full2);

        handleDma();
    } else {
        clearBit(&status, Busy);

        delayedCarry = false;
    }
}

uint64_t AudioInterface::getDuration() {
    uint64_t samplesPerSec = bus.videoInterface.clock  / (1 + dacRate);
    uint64_t length = audioLength & !7;

    return length * (bus.cpu.clock / (4 * samplesPerSec));
}

void AudioInterface::handleDma() {
    AudioDma dma = fifo[0];

    dmaReady = true;

    if (delayedCarry) {
        dma.address += 0x2000;
    }

    delayedCarry = ((dma.address + dma.length) & 0x1FFF) == 0;

    bus.cpu.scheduler.addEvent(Event(AIDma, bus.cpu.cop0.count + dma.duration));

    bus.setInterrupt(AI_INTERRUPT_FLAG);
}