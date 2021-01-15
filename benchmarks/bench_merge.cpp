/*
 * Copyright (c) 2021 Igor Kushnir <igorkuo@gmail.com>.
 *
 * SPDX-License-Identifier: MIT
 */
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <gfx/timsort.hpp>
#include "benchmarker.hpp"

template <typename value_t>
struct Bench {
    void operator()(const std::vector<value_t> &source) const {
        const int size = static_cast<int>(source.size());
        const std::vector<int> middle_positions = {
            0, 1, 2, 5, 100, size/100, size/20, size/5, size/3, size/2, 3*size/4,
            6*size/7, 24*size/25, 90*size/91, size-85, size-8, size-2, size-1, size
        };

        int result_count = 0;
        Result result_sum = {};

        std::cerr << "middle\\algorithm:\tstd::inplace_merge\ttimmerge" << std::endl;
        constexpr int width = 10;
        constexpr const char* padding1 = "       \t";
        constexpr const char* padding2 = "        \t";

        std::vector<value_t> a(source.size());
        for (auto middle : middle_positions) {
            if (middle < 0 || middle > size) {
                continue;
            }

            std::copy(source.begin(), source.end(), a.begin());
            std::sort(a.begin(), a.begin() + middle);
            std::sort(a.begin() + middle, a.end());
            const auto result = run(a, middle);

            ++result_count;
            result_sum.first += result.first;
            result_sum.second += result.second;

            std::cerr << std::setw(width) << middle
                      << padding1 << std::setw(width) << result.first
                      << padding2 << std::setw(width) << result.second
                      << std::endl;
        }

        std::cerr << "   AVERAGE"
                  << padding1 << std::setw(width) << result_sum.first / result_count
                  << padding2 << std::setw(width) << result_sum.second / result_count
                  << std::endl;
    }

private:
    using Result = std::pair<double, double>;

    static Result run(const std::vector<value_t> &a, const int middle) {
        std::vector<value_t> b(a.size());
        const auto assert_is_sorted = [&b] {
            if (!std::is_sorted(b.cbegin(), b.cend())) {
                std::cerr << "Not sorted!" << std::endl;
                std::abort();
            }
        };

        Result result;
        for (auto *total_time_ms : { &result.first, &result.second }) {
            using Clock = std::chrono::steady_clock;
            decltype(Clock::now() - Clock::now()) total_time{0};

            for (int i = 0; i < 100; ++i) {
                std::copy(a.begin(), a.end(), b.begin());
                const auto time_begin = Clock::now();

                if (total_time_ms == &result.first) {
                    std::inplace_merge(b.begin(), b.begin() + middle, b.end());
                } else {
                    gfx::timmerge(b.begin(), b.begin() + middle, b.end());
                }

                const auto time_end = Clock::now();
                total_time += time_end - time_begin;

                // Verifying that b is sorted should prevent the compiler from optimizing anything out.
                assert_is_sorted();
            }

            *total_time_ms = std::chrono::duration_cast<
                                std::chrono::microseconds>(total_time).count() / 1000.0;
        }
        return result;
    }
};

int main(int argc, const char *argv[]) {
    const int size = argc > 1 ? std::stoi(argv[1]) : 100 * 1000;
    Benchmarker<Bench> benchmarker(size);
    benchmarker.run();
}
