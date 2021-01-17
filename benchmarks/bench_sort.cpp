/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <vector>
#include <gfx/timsort.hpp>
#include "benchmarker.hpp"

template <typename value_t>
struct Bench {
    void operator()(const std::vector<value_t> &a) const {
        {
            std::vector<value_t> b(a);

            std::clock_t start = std::clock();
            for (int i = 0; i < 100; ++i) {
                std::copy(a.begin(), a.end(), b.begin());
                std::sort(b.begin(), b.end());
            }
            std::clock_t stop = std::clock();

            std::cerr << "std::sort        " << (double(stop - start) / CLOCKS_PER_SEC) << std::endl;
        }

        {
            std::vector<value_t> b(a);

            std::clock_t start = std::clock();
            for (int i = 0; i < 100; ++i) {
                std::copy(a.begin(), a.end(), b.begin());
                std::stable_sort(b.begin(), b.end());
            }
            std::clock_t stop = clock();

            std::cerr << "std::stable_sort " << (double(stop - start) / CLOCKS_PER_SEC) << std::endl;
        }

        {
            std::vector<value_t> b(a);

            std::clock_t start = std::clock();
            for (int i = 0; i < 100; ++i) {
                std::copy(a.begin(), a.end(), b.begin());
                gfx::timsort(b.begin(), b.end());
            }
            std::clock_t stop = std::clock();

            std::cerr << "timsort          " << (double(stop - start) / CLOCKS_PER_SEC) << std::endl;
        }
    }
};

int main(int argc, const char *argv[]) {
    const int size = argc > 1 ? std::atoi(argv[1]) : 100 * 1000;
    Benchmarker<Bench> benchmarker(size);
    benchmarker.run();
}
