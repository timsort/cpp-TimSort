/*
 * Copyright (c) 2024 Morwenn.
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <iterator>
#include <numeric>
#include <random>
#include <span>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include <gfx/timsort.hpp>
#include "test_helpers.hpp"

TEST_CASE( "support for temporary types" ) {
    SECTION( "timmerge over std::span" ) {
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        auto middle = vec.begin() + 38;
        gfx::timsort(vec.begin(), middle);
        gfx::timsort(middle, vec.end());

        auto view = std::span(vec);
        gfx::timmerge(std::span(vec), view.begin() + 38);
        CHECK(std::ranges::is_sorted(vec));
    }

    SECTION( "timsort over std::span" ) {
        std::vector<int> vec(50);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        gfx::timsort(std::span(vec));
        CHECK(std::ranges::is_sorted(vec));
    }
}

TEST_CASE( "support for sentinels" )
{
    SECTION( "timmerge with sentinel" ) {
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        auto middle = vec.begin() + 38;
        gfx::timsort(vec.begin(), middle);
        gfx::timsort(middle, vec.end());

        gfx::timmerge(std::counted_iterator(vec.begin(), 85),
                      std::counted_iterator(middle, 85 - 38),
                      std::default_sentinel);
        CHECK(std::is_sorted(vec.begin(), vec.begin() + 85));
    }

    SECTION( "timsort with sentinel" ) {
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        gfx::timsort(std::counted_iterator(vec.begin(), 85),
                     std::default_sentinel);
        CHECK(std::is_sorted(vec.begin(), vec.begin() + 85));
    }
}
