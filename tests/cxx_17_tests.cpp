/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
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

    SECTION( "for comparisons" ) {
        gfx::timsort(vec, &wrapper::compare_to);
        CHECK(std::is_sorted(vec.begin(), vec.end(), [](wrapper const& lhs, wrapper const& rhs) {
            return lhs.value < rhs.value;
        }));
    }

    SECTION( "for projections" ) {
        gfx::timsort(vec, std::less<>{}, &wrapper::value);
        CHECK(std::is_sorted(vec.begin(), vec.end(), [](wrapper const& lhs, wrapper const& rhs) {
            return lhs.value < rhs.value;
        }));
    }
}

#endif // __cpp_lib_invoke
