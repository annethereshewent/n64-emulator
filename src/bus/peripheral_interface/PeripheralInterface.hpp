#include "PIStatus.hpp"

class PeripheralInterface {
public:
    PIStatus piStatus;
    uint8_t dom1Latch;
    uint8_t dom2Latch;

    uint8_t dom1Pwd;
    uint8_t dom2Pwd;

    uint8_t dom1Pgs;
    uint8_t dom2Pgs;

    uint8_t dom1Rls;
    uint8_t dom2Rls;

    uint32_t dramAddress;
};