/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019-2024 Morwenn.
 * Copyright (c) 2021 Igor Kushnir <igorkuo@gmail.com>.
 *
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <utility>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include <gfx/timsort.hpp>
#include "test_helpers.hpp"

using namespace test_helpers;

namespace
{
    template <typename RandomAccessRange, typename Compare>
    void sort_and_merge(RandomAccessRange &range, decltype(std::end(range)) middle, Compare compare) {
        const auto first = std::begin(range);
        const auto last = std::end(range);
        gfx::timsort(first, middle, compare);
        gfx::timsort(middle, last, compare);
        gfx::timmerge(first, middle, last, compare);
    }

    template <typename RandomAccessRange>
    void sort_and_merge(RandomAccessRange &range, decltype(std::end(range)) middle) {
        using value_type = typename std::iterator_traits<decltype(middle)>::value_type;
        sort_and_merge(range, middle, std::less<value_type>());
    }

    inline std::mt19937& random_engine()
    {
        thread_local std::mt19937 res(Catch::rngSeed());
        return res;
    }
}

TEST_CASE( "merge_simple0" ) {
    std::vector<int> a;

    gfx::timmerge(a.begin(), a.end(), a.end());

    CHECK(a.size() == std::size_t(0));
}

TEST_CASE( "merge_simple1" ) {
    std::vector<int> a;

    a.push_back(-54);

    gfx::timmerge(a.begin(), a.end(), a.end(), std::greater<int>());

    CHECK(a.size() == std::size_t(1));
    CHECK(a[0] == -54);
}

TEST_CASE( "merge_simple2" ) {
    std::vector<int> a = { 10, 20 };

    gfx::timmerge(a.begin(), a.begin() + 1, a.end());

    CHECK(a.size() == std::size_t(2));
    CHECK(a[0] == 10);
    CHECK(a[1] == 20);

    a = { 20, 10 };

    gfx::timmerge(a.begin(), a.begin() + 1, a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(2));
    CHECK(a[0] == 10);
    CHECK(a[1] == 20);

    a = { 10, 10 };

    gfx::timmerge(a.begin(), a.begin() + 1, a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(2));
    CHECK(a[0] == 10);
    CHECK(a[1] == 10);
}

TEST_CASE( "merge_simple10" ) {
    std::vector<int> a = { 60, 50, 10, 40, 80, 20, 30, 70, 10, 90 };

    sort_and_merge(a, a.begin() + 5, std::less<int>());

    CHECK(a[0] == 10);
    CHECK(a[1] == 10);
    CHECK(a[2] == 20);
    CHECK(a[3] == 30);
    CHECK(a[4] == 40);
    CHECK(a[5] == 50);
    CHECK(a[6] == 60);
    CHECK(a[7] == 70);
    CHECK(a[8] == 80);
    CHECK(a[9] == 90);

    std::reverse(a.begin(), a.end());

    sort_and_merge(a, a.begin() + 2, std::less<int>());

    CHECK(a[0] == 10);
    CHECK(a[1] == 10);
    CHECK(a[2] == 20);
    CHECK(a[3] == 30);
    CHECK(a[4] == 40);
    CHECK(a[5] == 50);
    CHECK(a[6] == 60);
    CHECK(a[7] == 70);
    CHECK(a[8] == 80);
    CHECK(a[9] == 90);
}

TEST_CASE( "merge_shuffle30" ) {
    const int size = 30;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    test_helpers::shuffle(a);

    sort_and_merge(a, a.begin() + 24);

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "merge_shuffle128" ) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    test_helpers::shuffle(a);

    sort_and_merge(a, a.begin() + 51);

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "merge_shuffle204x" ) {
    for (int size : {2047, 2048}) {
        std::vector<int> a;
        for (int i = 0; i < size; ++i) {
            a.push_back((i + 1) * 10);
        }

        std::uniform_int_distribution<int> random_middle(0, size);
        for (int n = 0; n < 30; ++n) {
            test_helpers::shuffle(a);

            sort_and_merge(a, a.begin() + random_middle(random_engine()));

            for (int i = 0; i < size; ++i) {
                CHECK(a[i] == (i + 1) * 10);
            }
        }
    }
}

TEST_CASE( "merge_partial_shuffle102x" ) {
    for (int size : {1023, 1024}) {
        std::vector<int> a;
        for (int i = 0; i < size; ++i) {
            a.push_back((i + 1) * 10);
        }

        std::uniform_int_distribution<int> random_middle(0, size);

        // sorted-shuffled-sorted pattern
        for (int n = 0; n < 100; ++n) {
            test_helpers::shuffle(a.begin() + (size / 3 * 1), a.begin() + (size / 3 * 2));

            sort_and_merge(a, a.begin() + random_middle(random_engine()), std::less<int>());

            for (int i = 0; i < size; ++i) {
                CHECK(a[i] == (i + 1) * 10);
            }
        }

        // shuffled-sorted-shuffled pattern
        for (int n = 0; n < 100; ++n) {
            test_helpers::shuffle(a.begin(), a.begin() + (size / 3 * 1));
            test_helpers::shuffle(a.begin() + (size / 3 * 2), a.end());

            sort_and_merge(a, a.begin() + random_middle(random_engine()));

            for (int i = 0; i < size; ++i) {
                CHECK(a[i] == (i + 1) * 10);
            }
        }
    }
}

TEST_CASE( "merge_shuffle1025r" ) {
    const int size = 1025;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    std::uniform_int_distribution<int> random_middle(0, size);
    for (int n = 0; n < 100; ++n) {
        test_helpers::shuffle(a);

        sort_and_merge(a, a.begin() + random_middle(random_engine()), std::greater<int>());

        int j = size;
        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (--j + 1) * 10);
        }
    }
}

TEST_CASE( "merge_partial_reversed307x" ) {
    for (int size : {3071, 3072}) {
        std::vector<int> a;
        for (int i = 0; i < size; ++i) {
            a.push_back((i + 1) * 10);
        }

        std::uniform_int_distribution<int> random_middle(0, size);
        for (int n = 0; n < 20; ++n) {
            std::reverse(a.begin(), a.begin() + (size / 2)); // partial reversed

            sort_and_merge(a, a.begin() + random_middle(random_engine()));

            for (int i = 0; i < size; ++i) {
                CHECK(a[i] == (i + 1) * 10);
            }
        }
    }
}

TEST_CASE( "merge_c_array" ) {
    int a[] = {7, 1, 5, 3, 9};

    sort_and_merge(a, a + 2);

    CHECK(a[0] == 1);
    CHECK(a[1] == 3);
    CHECK(a[2] == 5);
    CHECK(a[3] == 7);
    CHECK(a[4] == 9);
}

TEST_CASE( "merge_string_array" ) {
    std::string a[] = {"7", "1", "5", "3", "9"};

    sort_and_merge(a, a + 3);

    CHECK(a[0] == "1");
    CHECK(a[1] == "3");
    CHECK(a[2] == "5");
    CHECK(a[3] == "7");
    CHECK(a[4] == "9");
}

TEST_CASE( "merge_non_default_constructible" ) {
    NonDefaultConstructible a[] = {7, 1, 5, 3, 9};

    sort_and_merge(a, a + 1);

    CHECK(a[0].i == 1);
    CHECK(a[1].i == 3);
    CHECK(a[2].i == 5);
    CHECK(a[3].i == 7);
    CHECK(a[4].i == 9);
}

TEST_CASE( "merge_default_compare_function" ) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    test_helpers::shuffle(a);

    const auto middle = a.begin() + a.size() / 3;
    gfx::timsort(a.begin(), middle);
    gfx::timsort(middle, a.end());
    gfx::timmerge(a.begin(), middle, a.end());

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "merge_stability" ) {
    std::vector<pair_t> a;

    for (int i = 100; i >= 0; --i) {
        a.push_back(std::make_pair(i, foo));
        a.push_back(std::make_pair(i, bar));
        a.push_back(std::make_pair(i, baz));
    }

    sort_and_merge(a, a.begin() + 117, &less_in_first);

    CHECK(a[0].first == 0);
    CHECK(a[0].second == foo);
    CHECK(a[1].first == 0);
    CHECK(a[1].second == bar);
    CHECK(a[2].first == 0);
    CHECK(a[2].second == baz);

    CHECK(a[3].first == 1);
    CHECK(a[3].second == foo);
    CHECK(a[4].first == 1);
    CHECK(a[4].second == bar);
    CHECK(a[5].first == 1);
    CHECK(a[5].second == baz);

    CHECK(a[6].first == 2);
    CHECK(a[6].second == foo);
    CHECK(a[7].first == 2);
    CHECK(a[7].second == bar);
    CHECK(a[8].first == 2);
    CHECK(a[8].second == baz);

    CHECK(a[9].first == 3);
    CHECK(a[9].second == foo);
    CHECK(a[10].first == 3);
    CHECK(a[10].second == bar);
    CHECK(a[11].first == 3);
    CHECK(a[11].second == baz);
}

TEST_CASE( "merge_issue2_duplication" ) {
    std::vector<std::pair<int, int> > a;

    for (int i = 0; i < 10000; ++i) {
        int first = static_cast<unsigned short>(rand());
        int second = static_cast<unsigned short>(rand());

        a.push_back(std::make_pair(first, second));
    }

    std::vector<std::pair<int, int> > expected(a);

    std::sort(expected.begin(), expected.end());
    sort_and_merge(a, a.begin() + 824);

    CHECK(a == expected);
}

TEST_CASE( "merge_projection" ) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back(i - 40);
    }
    test_helpers::shuffle(a);

    const auto middle = a.begin() + 43;
    gfx::timsort(a.begin(), middle, std::greater<int>(), std::negate<int>());
    gfx::timsort(middle, a.end(), std::greater<int>(), std::negate<int>());
    gfx::timmerge(a.begin(), middle, a.end(), std::greater<int>(), std::negate<int>());

    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == i - 40);
    }
}
