#pragma once

#include "Scheduler.hpp"
#include <iostream>

bool UniqueQueue::empty() {
    return elements.size() == 0;
}

Event UniqueQueue::peek() {
    return elements[0];
}

Event UniqueQueue::pop() {
    Event event = elements[0];

    elements.erase(elements.begin() + 0);

    for (int i = 0; i < elements.size(); i++) {
        events[elements[i].eventType]--;
    }

    events.erase(event.eventType);

    return event;
}

void UniqueQueue::insert(Event event) {
    if (events.contains(event.eventType)) {
        int index = events[event.eventType];

        if (index != -1) {
            elements.erase(elements.begin() + index);

            for (int i = index; i < elements.size(); i++) {
                if (events[elements[i].eventType] != 0) {
                    events[elements[i].eventType]--;
                }
            }
        }
    }

    int low = 0;
    int high = elements.size() - 1;
    int mid = (high - low) / 2;

    int i = -1;

    while (low <= high) {
        if (mid < 0 || mid >= elements.size()) {
            break;
        }

        if (event.cycles < elements[mid].cycles) {
            if (mid - 1 >= 0 && elements[mid-1].cycles < event.cycles) {
                i = mid;
                break;
            }

            high = mid - 1;
        } else if (event.cycles > elements[mid].cycles) {
            if (mid + 1 < elements.size() && elements[mid + 1].cycles > event.cycles) {
                i = mid + 1;
                break;
            }

            low = mid + 1;
        }

        mid = low + (high - low) / 2;
    }

    if (i == -1) {
        i = mid <= 0 ? 0 : elements.size();
    }

    elements.insert(elements.begin() + i, event);
    events.insert({event.eventType, i});


    for (int j = i + 1; j < elements.size(); j++) {
        Event event = elements[j];

        if (events.contains(event.eventType)) {
            int index = events[event.eventType];

            events[elements[j].eventType]++;
        }
    }
}

void UniqueQueue::remove(EventType eventType) {
    if (events.contains(eventType)) {
        int index = events[eventType];

        if (index != -1) {
            elements.erase(elements.begin() + index);
            events.erase(eventType);
        }
    }
}

void Scheduler::addEvent(Event event) {
    queue.insert(event);
}

void Scheduler::removeEvent(EventType eventType) {
    queue.remove(eventType);
}

bool Scheduler::hasNextEvent(uint64_t cycles) {
    if (!queue.empty() && queue.peek().cycles <= cycles) {
        return true;
    }

    return false;
}

Event Scheduler::getNextEvent() {
    return queue.pop();
}

uint64_t Scheduler::getTimeToNext() {
    return queue.peek().cycles;
}

void Scheduler::rebaseEvents(uint64_t oldCycles, uint64_t newCycles) {
    std::vector<Event> events;
    while (!queue.empty()) {
        events.push_back(queue.pop());
    }

    for (Event event: events) {
        event.cycles -= oldCycles;
        event.cycles += newCycles;
        queue.insert(event);
    }
}