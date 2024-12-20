#include <cstdint>
#include <vector>
#include "SPStatus.hpp"

class RSP {
public:
    SPStatus spStatus;
    std::vector<uint8_t> imem;

    RSP();
};