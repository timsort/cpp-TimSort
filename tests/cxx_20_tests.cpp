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
#include <utility>
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
        auto last_it = gfx::timmerge(std::span(vec), view.begin() + 38);
        CHECK(std::ranges::is_sorted(vec));
        CHECK(last_it == view.end());
    }

    SECTION( "timsort over std::span" ) {
        std::vector<int> vec(50);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        auto last_it = gfx::timsort(std::span(vec));
        CHECK(std::ranges::is_sorted(vec));
        CHECK(last_it == std::span(vec).end());
    }
}

TEST_CASE( "dangling return type" ) {
    SECTION( "timmerge over temporary std::vector" ) {
        std::vector<int> vec(30, 5);
        auto last_it = gfx::timmerge(std::move(vec), vec.begin() + 14);
        STATIC_CHECK(std::is_same_v<decltype(last_it), std::ranges::dangling>);
    }

    SECTION( "timsort over temporary std::vector" ) {
        std::vector<int> vec(50, 8);
        auto last_it = gfx::timsort(std::move(vec));
        STATIC_CHECK(std::is_same_v<decltype(last_it), std::ranges::dangling>);
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

        auto last_it = gfx::timmerge(std::counted_iterator(vec.begin(), 85),
                                     std::counted_iterator(middle, 85 - 38),
                                     std::default_sentinel);
        CHECK(std::is_sorted(vec.begin(), vec.begin() + 85));
        CHECK(last_it == std::counted_iterator(vec.begin() + 85, 0));
        CHECK(last_it == std::default_sentinel);
    }

    SECTION( "timsort with sentinel" ) {
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        auto last_it = gfx::timsort(std::counted_iterator(vec.begin(), 85),
                                    std::default_sentinel);
        CHECK(std::is_sorted(vec.begin(), vec.begin() + 85));
        CHECK(last_it == std::counted_iterator(vec.begin() + 85, 0));
        CHECK(last_it == std::default_sentinel);
    }
}
