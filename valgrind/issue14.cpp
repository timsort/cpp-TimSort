#include "timsort.hpp"
#include <deque>
#include <cstdlib>

int main() {
    std::deque<int> collection = {15, 7,  16, 20, 25, 28, 13, 27, 34, 24, 19, 1, 6,  30, 32, 29, 10, 9,
                                  3,  31, 21, 26, 8,  2,  22, 14, 4,  12, 5,  0, 23, 33, 11, 17, 18};

    gfx::timsort(std::begin(collection), std::end(collection));
    assert(std::is_sorted(std::begin(collection), std::end(collection)));

    return 0;
}
