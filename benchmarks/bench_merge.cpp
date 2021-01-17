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
#include <valarray>
#include <vector>
#include <gfx/timsort.hpp>
#include "benchmarker.hpp"

namespace
{
    std::vector<int> generate_middle_positions(int size) {
        std::vector<int> result = {
            0, 1, 2, 5, 100, size/100, size/20, size/5, size/3, size/2, 3*size/4,
            6*size/7, 24*size/25, 90*size/91, size-85, size-8, size-2, size-1, size
        };

        // The code below can remove or reorder elements if size is small.

        auto logical_end = std::remove_if(result.begin(), result.end(), [size](int middle) {
            return middle < 0 || middle > size;
        });
        result.erase(logical_end, result.end());

        std::sort(result.begin(), result.end());
        logical_end = std::unique(result.begin(), result.end());
        result.erase(logical_end, result.end());

        return result;
    }

    using Result = std::valarray<double>;
    Result zeroResult() { return Result(2); }
}

template <typename value_t>
struct Bench {
    void operator()(const std::vector<value_t> &source) const {
        const int size = static_cast<int>(source.size());
        const auto middle_positions = generate_middle_positions(size);

        int prev_middle = 0;
        auto prev_result = zeroResult();
        auto result_sum = zeroResult();

        std::cerr << "middle\\algorithm:\tstd::inplace_merge\ttimmerge" << std::endl;
        constexpr int width = 10;
        constexpr const char* padding = "        \t";

        std::vector<value_t> a(source.size());
        for (auto middle : middle_positions) {
            std::copy(source.begin(), source.end(), a.begin());
            std::sort(a.begin(), a.begin() + middle);
            std::sort(a.begin() + middle, a.end());
            const auto result = run(a, middle);

            if (middle != prev_middle) {
                // Trapezoidal rule for approximating the definite integral.
                result_sum += 0.5 * (result + prev_result)
                                  * static_cast<double>(middle - prev_middle);
                prev_middle = middle;
            }
            prev_result = result;

            std::cerr << std::setw(width) << middle
                      << "       \t" << std::setw(width) << result[0]
                      << padding << std::setw(width) << result[1]
                      << std::endl;
        }

        if (size != 0) {
            result_sum /= static_cast<double>(size);
            std::cerr << "approx. average"
                      << "  \t" << std::setw(width) << result_sum[0]
                      << padding << std::setw(width) << result_sum[1]
                      << std::endl;
        }
    }

private:
    static Result run(const std::vector<value_t> &a, const int middle) {
        std::vector<value_t> b(a.size());
        const auto assert_is_sorted = [&b] {
            if (!std::is_sorted(b.cbegin(), b.cend())) {
                std::cerr << "Not sorted!" << std::endl;
                std::abort();
            }
        };

        auto result = zeroResult();
        for (auto *total_time_ms : { &result[0], &result[1] }) {
            using Clock = std::chrono::steady_clock;
            decltype(Clock::now() - Clock::now()) total_time{0};

            for (int i = 0; i < 100; ++i) {
                std::copy(a.begin(), a.end(), b.begin());
                const auto time_begin = Clock::now();

                if (total_time_ms == &result[0]) {
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
