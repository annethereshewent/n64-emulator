#pragma once

#include "Scheduler.hpp"
#include <iostream>

void Scheduler::addEvent(Event event) {
    if (!currentEvents.contains(event.eventType)) {
        queue.push(event);
        currentEvents.insert(event.eventType);
    } else {
        // TODO: optimize this, probably need to use
        // something other than priority queue

        std::vector<Event> events;
        while (!queue.empty()) {
            Event compare = queue.top();
            events.push_back(compare);
            queue.pop();

            if (compare.eventType == event.eventType) {
                break;
            }
        }

        queue.push(event);

        for (Event oldEvent: events) {
            queue.push(oldEvent);
        }
    }
}

void Scheduler::removeEvent(EventType eventType) {
    // TODO: optimize this, probably need to use
    // something other than priority queue

    if (currentEvents.contains(eventType)) {
        std::vector<Event> events;
        while (!queue.empty()) {
            Event compare = queue.top();
            events.push_back(compare);
            queue.pop();

            if (compare.eventType == eventType) {
                break;
            }
        }

        for (Event oldEvent: events) {
            queue.push(oldEvent);
        }

        currentEvents.erase(eventType);
    }


}

bool Scheduler::hasNextEvent(uint64_t cycles) {
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

uint64_t Scheduler::getTimeToNext() {
    return queue.top().cycles;
}

void Scheduler::rebaseEvents(uint64_t oldCycles, uint64_t newCycles) {
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