#pragma once

#include "VideoInterface.hpp"
#include <cstdint>
#include "../../cpu/Scheduler.hpp"

class Scheduler;

void VideoInterface::updateVCurrent(Scheduler& scheduler, uint64_t currentCount) {
    Event viEvent = scheduler.getEvent(VideoInterrupt);

    if (viEvent.eventType != NoEvent) {
        vcurrent = (delay - (viEvent.cycles - currentCount)) / countPerScanline;

        if (vcurrent >= vTotal) {
            vcurrent -= vTotal;
        }

        vcurrent |= (vcurrent & ~1) | field;
        vcurrent &= 0x3ff;
    } else {
        vcurrent = 0;
    }
}