#include "utils.h"
#include <vector>

std::size_t calculate_int_vector_hash(const std::vector<int> &v) {
    std::size_t seed = v.size();
    for (int idx: v) {
        seed ^= idx + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

