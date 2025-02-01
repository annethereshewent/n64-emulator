#include <cstdint>
#include "SIStatus.hpp"

class SerialInterface {
public:
    SIStatus status;
    uint32_t dramAddress = 0;
};