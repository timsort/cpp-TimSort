/*
 * Copyright (c) 2024 Morwenn.
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <numeric>
#include <random>
#include <span>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include <gfx/timsort.hpp>
#include "test_helpers.hpp"

TEST_CASE( "support for temporary types" ) {
    SECTION( "timsort over std::span" ) {
        std::vector<int> vec(50);
        std::iota(vec.begin(), vec.end(), -25);
        test_helpers::shuffle(vec);

        gfx::timsort(std::span(vec));
        CHECK(std::ranges::is_sorted(vec));
    }
}
