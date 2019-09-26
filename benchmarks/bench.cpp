/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <gfx/timsort.hpp>

using namespace gfx;

enum state_t { sorted, randomized, reversed };

template <typename T>
struct convert_to
{
    static T from(int value) {
        return T(value);
    }
};

template <>
struct convert_to<std::string>
{
    static std::string from(int value) {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }
};

template <typename value_t>
static void bench(int size, state_t const state) {
    std::cerr << "size\t" << size << std::endl;

    std::vector<value_t> a;
    for (int i = 0; i < size; ++i) {
        a.push_back(convert_to<value_t>::from((i + 1) * 10));
    }

    switch (state) {
    case randomized:
        std::random_shuffle(a.begin(), a.end());
        break;
    case reversed:
        std::stable_sort(a.begin(), a.end());
        std::reverse(a.begin(), a.end());
        break;
    case sorted:
        std::stable_sort(a.begin(), a.end());
        break;
    default:
        assert(!"not reached");
    }

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
            timsort(b.begin(), b.end());
        }
        std::clock_t stop = std::clock();

        std::cerr << "timsort          " << (double(stop - start) / CLOCKS_PER_SEC) << std::endl;
    }
}

static void doit(int const n, state_t const state) {
    std::cerr << "[int]" << std::endl;
    bench<int>(n, state);

    std::cerr << "[std::string]" << std::endl;
    bench<std::string>(n, state);
}

int main(int argc, const char *argv[]) {
    const int N = argc > 1 ? std::atoi(argv[1]) : 100 * 1000;

    std::cerr << std::setprecision(6) << std::setiosflags(std::ios::fixed);

    std::srand(0);

    std::cerr << "RANDOMIZED SEQUENCE" << std::endl;
    doit(N, randomized);

    std::cerr << "REVERSED SEQUENCE" << std::endl;
    doit(N, reversed);

    std::cerr << "SORTED SEQUENCE" << std::endl;
    doit(N, sorted);
}
