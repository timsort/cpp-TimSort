/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
 * Copyright (c) 2021 Igor Kushnir <igorkuo@gmail.com>.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GFX_TIMSORT_BENCHMARKER_HPP
#define GFX_TIMSORT_BENCHMARKER_HPP

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace BenchmarkerHelpers {
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
}

template <template<typename> class Bench>
class Benchmarker {
private:
    enum state_t { sorted, randomized, reversed };

    template <typename value_t>
    void bench(state_t const state) {
        thread_local std::mt19937 random_engine(2581470); // fixed seed is enough

        std::vector<value_t> a;
        for (int i = 0; i < size_; ++i) {
            a.push_back(BenchmarkerHelpers::convert_to<value_t>::from((i + 1) * 10));
        }

        switch (state) {
        case randomized:
            std::shuffle(a.begin(), a.end(), random_engine);
            break;
        case reversed:
            std::stable_sort(a.begin(), a.end());
            std::reverse(a.begin(), a.end());
            break;
        case sorted:
            std::stable_sort(a.begin(), a.end());
            break;
        default:
            std::abort(); // unreachable
        }

        Bench<value_t>()(a);
    }

    void doit(state_t const state) {
        std::cerr << "[int]" << std::endl;
        bench<int>(state);

        std::cerr << "[std::string]" << std::endl;
        bench<std::string>(state);
    }

    const int size_;

public:
    explicit Benchmarker(int size) : size_(size) {
    }

    void run() {
        std::cerr << "size\t" << size_ << std::endl;

        std::cerr << std::setprecision(6) << std::setiosflags(std::ios::fixed);

        std::srand(0);

        std::cerr << "RANDOMIZED SEQUENCE" << std::endl;
        doit(randomized);

        std::cerr << "REVERSED SEQUENCE" << std::endl;
        doit(reversed);

        std::cerr << "SORTED SEQUENCE" << std::endl;
        doit(sorted);
    }
};

#endif // GFX_TIMSORT_BENCHMARKER_HPP
