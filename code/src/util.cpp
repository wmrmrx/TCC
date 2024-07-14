#include "util.hpp"

uint64_t ceil_div(uint64_t a, uint64_t b) {
    return (a + b - 1) / b;
}

int64_t ceil_div(int64_t a, int64_t b) {
    return (a + b - 1) / b;
}
