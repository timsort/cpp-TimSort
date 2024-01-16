/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019-2024 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <deque>
#include <stdexcept>
#include <utility>
#include <vector>
#include <windows.h>
#include <catch2/catch_test_macros.hpp>
#include <gfx/timsort.hpp>
#include "test_helpers.hpp"

TEST_CASE( "check inclusion of windows.h" ) {
    const int size = 100;

    std::vector<int> vec;
    for (int i = 0; i < size; ++i) {
        vec.push_back(i);
    }

    test_helpers::shuffle(vec);
    gfx::timsort(vec.begin(), vec.end());

    for (int i = 0; i < size; ++i) {
        CHECK(vec[i] == i);
    }
}
