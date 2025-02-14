#pragma once

#include <cstdint>

class PIFChannel {
public:
    int64_t tx = -1;
    int64_t txBuf = -1;
    int64_t rx = -1;
    int64_t rxBuf = -1;
};