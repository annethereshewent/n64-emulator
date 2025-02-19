#pragma once

#include "DPCStatus.hpp"

class Bus;

class RDPInterface {
public:
    DPCStatus status;

    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t current = 0;

    uint32_t clockCounter = 0;
    bool isFrozen = false;
    bool frameFinished = false;

    RDPInterface(Bus& bus): bus(bus) {};

    Bus& bus;

    uint32_t readRegisters(uint32_t offset);
    void writeRegisters(uint32_t offset, uint32_t value);
    void updateStatus(uint32_t value);
};