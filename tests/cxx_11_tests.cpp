/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <algorithm>
#include <deque>
#include <iostream>
#include <utility>
#include <vector>
#include <catch.hpp>
#include <gfx/timsort.hpp>

template <typename T> struct move_only {
    // Constructors

    move_only() = delete;
    move_only(const move_only &) = delete;

    move_only(const T &value) : can_read(true), value(value) {
    }

    move_only(move_only &&other) : can_read(true), value(std::move(other.value)) {
        if (not exchange(other.can_read, false)) {
            std::cerr << "illegal read from a moved-from value\n";
            assert(false);
        }
    }

    // Assignment operators

    move_only &operator=(const move_only &) = delete;

    auto operator=(move_only &&other) -> move_only & {
        if (&other != this) {
            if (not exchange(other.can_read, false)) {
                std::cerr << "illegal read from a moved-from value\n";
                assert(false);
            }
            can_read = true;
            value = std::move(other.value);
        }
        return *this;
    }

    // A C++11 backport of std::exchange()

    template <typename U> U exchange(U &obj, U &&new_val) {
        U old_val = std::move(obj);
        obj = std::forward<U>(new_val);
        return old_val;
    }

    // Whether the value can be read
    bool can_read;
    // Actual value
    T value;
};

TEST_CASE( "shuffle10k_for_move_only_types" ) {
    const int size = 1024 * 10; // should be even number of elements

    std::vector<move_only<int> > a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        gfx::timsort(a.begin(), a.end(), [](const move_only<int> &x, const move_only<int> &y) { return x.value < y.value; });

        for (int i = 0; i < size; ++i) {
            CHECK(a[i].value == (i + 1) * 10);
        }
    }
}

TEST_CASE( "issue14" ) {
    int a[] = {15, 7,  16, 20, 25, 28, 13, 27, 34, 24, 19, 1, 6,  30, 32, 29, 10, 9,
               3,  31, 21, 26, 8,  2,  22, 14, 4,  12, 5,  0, 23, 33, 11, 17, 18};
    std::deque<int> c(std::begin(a), std::end(a));

    gfx::timsort(std::begin(c), std::end(c));
    CHECK(std::is_sorted(std::begin(c), std::end(c)));
}
