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

void Scheduler::rebaseEvents(uint32_t oldCycles, uint32_t newCycles) {
    std::vector<Event> events;
    while (!queue.empty()) {
        events.push_back(queue.top());
        queue.pop();
    }

    for (Event event: events) {
        event.cycles -= oldCycles;
        event.cycles += newCycles;
        queue.push(event);
    }
}