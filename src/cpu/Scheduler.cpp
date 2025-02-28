#pragma once

#include "Scheduler.hpp"
#include <iostream>

void Scheduler::addEvent(Event event) {
    events[event.eventType] = event;

    if (event.cycles < timeToNextEvent) {
        timeToNextEvent = event.cycles;
        nextEvent = event.eventType;
    }
}

void Scheduler::removeEvent(EventType eventType) {
    events[eventType] = Event();

    if (eventType == nextEvent) {
        nextEvent = EventType::NoEvent;
        timeToNextEvent = 0xffffffffffffffff;
        updateNextEvent();
    }
}

void Scheduler::updateNextEvent() {
    for (Event event: events) {
        if (event.cycles < timeToNextEvent) {
            timeToNextEvent = event.cycles;
            nextEvent = event.eventType;
        }
    }
}

Event Scheduler::getEvent(EventType eventType) {
    return events[eventType];
}

bool Scheduler::hasNextEvent(uint64_t cycles) {
    return cycles >= timeToNextEvent && nextEvent != EventType::NoEvent;
}

Event Scheduler::getNextEvent() {
    Event event = events[nextEvent];

    events[nextEvent] = Event();

    timeToNextEvent = 0xffffffffffffffff;
    nextEvent = EventType::NoEvent;

    updateNextEvent();

    return event;
}

uint64_t Scheduler::getTimeToNext() {
    return timeToNextEvent;
}

void Scheduler::rebaseEvents(uint64_t oldCycles, uint64_t newCycles) {
    for (Event& event: events) {
        event.cycles -= oldCycles;
        event.cycles += newCycles;
    }
}