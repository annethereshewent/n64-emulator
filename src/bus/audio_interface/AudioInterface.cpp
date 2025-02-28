#pragma once

#include "AudioInterface.hpp"
#include "../Bus.hpp"

void AudioInterface::pushDma() {
    AudioDma dma;
    if (status.dmaBusy) {
        dma.address = dramAddress;
        dma.duration = getDuration();
        dma.length = audioLength;

        fifo[1] = dma;

        status.dmaFull1 = 1;
        status.dmaFull2 = 1;
    } else {
        dma.address = dramAddress;
        dma.duration = getDuration();
        dma.length = audioLength;

        fifo[0] = dma;

        handleDma();

        status.dmaBusy = 1;
    }
}

void AudioInterface::popDma() {
    if (status.dmaFull1) {
        fifo[0] = fifo[1];

        status.dmaFull1 = 0;
        status.dmaFull2 = 0;

        handleDma();
    } else {
        status.dmaBusy = 0;
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