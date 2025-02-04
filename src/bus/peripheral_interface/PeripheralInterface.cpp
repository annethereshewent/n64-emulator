#pragma once

#include <cstdint>
#include "PeripheralInterface.hpp"
#include <iostream>

uint32_t PeripheralInterface::calculateCycles(int32_t domain, uint32_t length) {
    double cycles = 0.0;

    double pageSize, latency, pulseWidth, release, pages;

    switch (domain) {
        case 1:
            latency = (double)dom1Latch;
            pulseWidth = (double)(dom1Pwd + 1);
            release = (double)(dom1Rls + 1);
            pageSize = powf(2.0, (double)(dom1Pgs + 2));
            break;
        case 2:
            latency = (double)dom2Latch;
            pulseWidth = (double)(dom2Pwd + 1);
            release = (double)(dom2Rls + 1);
            pageSize = powf(2.0, (double)(dom2Pgs + 2));
            break;
        default:
            std::cout << "invalid domain given\n";
            exit(1);
            break;
    }

    pages = ceil((double)length / pageSize);

    cycles = (14.0 + latency) * pages;
    cycles += (pulseWidth + release) * ((double)length / 2.0);
    cycles += 5.0 * pages;
    return (uint64_t)(cycles * 1.5);
}