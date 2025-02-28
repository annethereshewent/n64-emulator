#pragma once

#include "VideoInterface.hpp"
#include <cstdint>
#include "../../cpu/Scheduler.hpp"

class Scheduler;

void VideoInterface::updateVCurrent(Scheduler& scheduler, uint64_t currentCount) {
    Event viEvent = scheduler.getEvent(VideoInterrupt);

    if (viEvent.eventType != NoEvent) {
        vcurrent.value = (delay - viEvent.cycles - currentCount) / countPerScanline;

        if (vcurrent.value >= vTotal) {
            vcurrent.value -= vTotal;
        }
    }

    vcurrent.value |= (vcurrent.value & ~1) | field;
}