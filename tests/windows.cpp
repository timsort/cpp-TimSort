/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <deque>
#include <stdexcept>
#include <utility>
#include <vector>
#include <windows.h>
#include <catch2/catch.hpp>
#include <gfx/timsort.hpp>

TEST_CASE( "check inclusion of windows.h" ) {
    const int size = 100;

    std::vector<int> vec;
    for (int i = 0; i < size; ++i) {
        vec.push_back(i);
    }

    std::random_shuffle(vec.begin(), vec.end());
    gfx::timsort(vec.begin(), vec.end());

    for (int i = 0; i < size; ++i) {
        CHECK(vec[i] == i);
    }
}
