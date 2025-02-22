#pragma once

#include <cstdint>
#include <queue>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

enum EventType {
    VideoInterrupt,
    PIFExecuteCommand,
    RspDmaPop,
    RunRspPc,
    PIDma,
    CompareCount,
    SIDma,
    AIDma,
    RDPEvent
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
    void removeEvent(EventType eventType);

    bool hasNextEvent(uint64_t cycles);

    Event getNextEvent();

    void rebaseEvents(uint64_t oldCount, uint64_t newCount);
    uint64_t getTimeToNext();
};