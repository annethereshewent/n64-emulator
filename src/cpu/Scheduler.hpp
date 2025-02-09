#pragma once

#include <cstdint>
#include <queue>
#include <vector>
#include <unordered_set>

enum EventType {
    VideoInterrupt,
    PIFExecuteCommand,
    RspDmaPop,
    RunRspPc,
    PIDma,
    CompareCount
};

class Event {
public:
    uint64_t cycles;
    EventType eventType;

    Event(EventType eventType, uint64_t cycles) {
        this->eventType = eventType;
        this->cycles = cycles;
    };
};

class Comparator {
public:
    bool operator() (Event l, Event r) {
        return l.cycles > r.cycles;
    }
};

class Scheduler {
private:
    std::priority_queue<Event, std::vector<Event>, Comparator> queue;
public:
    std::unordered_set<EventType> currentEvents;
    void addEvent(Event event);

    bool hasNextEvent(uint64_t cycles);

    Event getNextEvent();

    void rebaseEvents(uint64_t oldCount, uint64_t newCount);
    uint64_t getTimeToNext();
};