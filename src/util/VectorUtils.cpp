#include <cstdint>
#include <vector>
#include <algorithm>

bool contains(std::vector<uint32_t> vec, uint32_t element) {
    return std::find(vec.begin(), vec.end(), element) != vec.end();
}