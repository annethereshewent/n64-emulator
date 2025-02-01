#pragma once

#include "Scheduler.hpp"
#include <iostream>

void Scheduler::addEvent(Event event) {
    queue.push(event);
}

bool Scheduler::hasNextEvent(uint32_t cycles) {
    if (!queue.empty() && queue.top().cycles <= cycles) {
        return true;
    }

    return false;
}

Event Scheduler::getNextEvent() {
    Event event = queue.top();

    queue.pop();

    return event;
}