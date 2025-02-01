#pragma once

#include <cstdint>
#include <queue>
#include <vector>

enum EventType {
    VideoInterrupt
};

class Event {
public:
    uint32_t cycles;
    EventType eventType;

    Event(EventType eventType, uint32_t cycles) {
        this->eventType = eventType;
        this->cycles = cycles;
    };
};

class Comparator {
public:
    bool operator() (Event l, Event r) {
        return l.cycles < r.cycles;
    }
};

class Scheduler {
private:
    std::priority_queue<Event, std::vector<Event>, Comparator> queue;
public:
    void addEvent(Event event);

    bool hasNextEvent(uint32_t cycles);

    Event getNextEvent();
};