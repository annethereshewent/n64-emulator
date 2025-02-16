#pragma once

#include <cstdint>
#include <queue>
#include <vector>
#include <unordered_map>

enum EventType {
    VideoInterrupt,
    PIFExecuteCommand,
    RspDmaPop,
    RunRspPc,
    PIDma,
    CompareCount,
    SIDma,
    AIDma
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

class UniqueQueue {
private:
    std::vector<Event> elements = {};
    std::unordered_map<EventType, int> events;
public:
    void insert(Event event);
    void remove(EventType eventType);
    bool empty();
    Event peek();
    Event pop();
};

class Scheduler {
private:
    UniqueQueue queue;
public:
    void addEvent(Event event);
    void removeEvent(EventType eventType);

    bool hasNextEvent(uint64_t cycles);

    Event getNextEvent();

    void rebaseEvents(uint64_t oldCount, uint64_t newCount);
    uint64_t getTimeToNext();
};