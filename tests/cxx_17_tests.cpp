/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019-2022 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <functional>
#include <numeric>
#include <random>
#include <vector>
#include <catch2/catch.hpp>
#include <gfx/timsort.hpp>
#include "test_helpers.hpp"

namespace
{
    struct wrapper {
        wrapper() = default;
        wrapper(wrapper const&) = default;
        wrapper(wrapper&&) = default;

        wrapper(int val) : value(val) {
        }

        wrapper& operator=(wrapper const&) = default;
        wrapper& operator=(wrapper&&) = default;

        wrapper& operator=(int val) {
            value = val;
            return *this;
        }

        bool compare_to(wrapper const& other) const {
            return value < other.value;
        }

        int value = 0;
    };
}

#ifdef __cpp_lib_invoke

TEST_CASE( "generalized callables" ) {
    std::vector<wrapper> vec(50);
    std::iota(vec.begin(), vec.end(), -25);
    std::mt19937 gen(123456); // fixed seed is enough
    std::shuffle(vec.begin(), vec.end(), gen);

    const auto is_vec_sorted = [&vec] {
        return std::is_sorted(vec.begin(), vec.end(), [](wrapper const& lhs, wrapper const& rhs) {
            return lhs.value < rhs.value;
        });
    };

    SECTION( "timsort for comparisons" ) {
        GFX_TIMSORT_TEST_SORT(vec, &wrapper::compare_to);
        CHECK(is_vec_sorted());
    }

    SECTION( "timsort for projections" ) {
        GFX_TIMSORT_TEST_SORT(vec, std::less<>{}, &wrapper::value);
        CHECK(is_vec_sorted());
    }

    std::uniform_int_distribution<int> random_middle(0, vec.size());

    SECTION( "timmerge for comparisons" ) {
        const auto middle = vec.begin() + random_middle(gen);
        GFX_TIMSORT_TEST_SORT(vec.begin(), middle, &wrapper::compare_to);
        GFX_TIMSORT_TEST_SORT(middle, vec.end(), &wrapper::compare_to);
        GFX_TIMSORT_TEST_MERGE(vec.begin(), middle, vec.end(), &wrapper::compare_to);
        CHECK(is_vec_sorted());
    }

    SECTION( "timmerge for projections" ) {
        const auto middle = vec.begin() + random_middle(gen);
        GFX_TIMSORT_TEST_SORT(vec.begin(), middle, std::less<>{}, &wrapper::value);
        GFX_TIMSORT_TEST_SORT(middle, vec.end(), std::less<>{}, &wrapper::value);
        GFX_TIMSORT_TEST_MERGE(vec.begin(), middle, vec.end(), std::less<>{}, &wrapper::value);
        CHECK(is_vec_sorted());
    }
}

#endif // __cpp_lib_invoke
