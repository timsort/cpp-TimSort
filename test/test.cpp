#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <utility>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TimSortTest
#include <boost/test/unit_test.hpp>

#include "timsort.hpp"

#if ENABLE_STD_MOVE
#warning std::move() enabled
#else
#warning std::move() disabled
#endif

using namespace gfx;

BOOST_AUTO_TEST_CASE(simple0) {
    std::vector<int> a;

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(0));
}

BOOST_AUTO_TEST_CASE(simple1) {
    std::vector<int> a;

    a.push_back(42);

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(1));
    BOOST_CHECK_EQUAL(a[0], 42);
}

BOOST_AUTO_TEST_CASE(simple2) {
    std::vector<int> a;

    a.push_back(10);
    a.push_back(20);

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(2));
    BOOST_CHECK_EQUAL(a[0], 10);
    BOOST_CHECK_EQUAL(a[1], 20);

    a.clear();
    a.push_back(20);
    a.push_back(10);

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(2));
    BOOST_CHECK_EQUAL(a[0], 10);
    BOOST_CHECK_EQUAL(a[1], 20);

    a.clear();
    a.push_back(10);
    a.push_back(10);

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(2));
    BOOST_CHECK_EQUAL(a[0], 10);
    BOOST_CHECK_EQUAL(a[1], 10);
}

BOOST_AUTO_TEST_CASE(simple10) {
    std::vector<int> a;
    a.push_back(60);
    a.push_back(50);
    a.push_back(10);
    a.push_back(40);
    a.push_back(80);
    a.push_back(20);
    a.push_back(30);
    a.push_back(70);
    a.push_back(10);
    a.push_back(90);

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a[0], 10);
    BOOST_CHECK_EQUAL(a[1], 10);
    BOOST_CHECK_EQUAL(a[2], 20);
    BOOST_CHECK_EQUAL(a[3], 30);
    BOOST_CHECK_EQUAL(a[4], 40);
    BOOST_CHECK_EQUAL(a[5], 50);
    BOOST_CHECK_EQUAL(a[6], 60);
    BOOST_CHECK_EQUAL(a[7], 70);
    BOOST_CHECK_EQUAL(a[8], 80);
    BOOST_CHECK_EQUAL(a[9], 90);

    std::reverse(a.begin(), a.end());

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a[0], 10);
    BOOST_CHECK_EQUAL(a[1], 10);
    BOOST_CHECK_EQUAL(a[2], 20);
    BOOST_CHECK_EQUAL(a[3], 30);
    BOOST_CHECK_EQUAL(a[4], 40);
    BOOST_CHECK_EQUAL(a[5], 50);
    BOOST_CHECK_EQUAL(a[6], 60);
    BOOST_CHECK_EQUAL(a[7], 70);
    BOOST_CHECK_EQUAL(a[8], 80);
    BOOST_CHECK_EQUAL(a[9], 90);
}

BOOST_AUTO_TEST_CASE(shuffle30) {
    const int size = 30;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(size));
    for (int i = 0; i < size; ++i) {
        BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
    }
}

BOOST_AUTO_TEST_CASE(shuffle31) {
    const int size = 31;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(size));
    for (int i = 0; i < size; ++i) {
        BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
    }
}

BOOST_AUTO_TEST_CASE(shuffle32) {
    const int size = 32;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(size));
    for (int i = 0; i < size; ++i) {
        BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
    }
}

BOOST_AUTO_TEST_CASE(shuffle128) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end(), std::less<int>());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(size));
    for (int i = 0; i < size; ++i) {
        BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
    }
}

BOOST_AUTO_TEST_CASE(shuffle1023) {
    const int size = 1023; // odd number of elements

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(shuffle1024) {
    const int size = 1024; // should be even number of elements

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(partial_shuffle1023) {
    const int size = 1023;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    // sorted-shuffled-sorted pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin() + (size / 3 * 1), a.begin() + (size / 3 * 2));

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }

    // shuffled-sorted-shuffled pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.begin() + (size / 3 * 1));
        std::random_shuffle(a.begin() + (size / 3 * 2), a.end());

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(partial_shuffle1024) {
    const int size = 1024;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    // sorted-shuffled-sorted pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin() + (size / 3 * 1), a.begin() + (size / 3 * 2));

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }

    // shuffled-sorted-shuffled pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.begin() + (size / 3 * 1));
        std::random_shuffle(a.begin() + (size / 3 * 2), a.end());

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(shuffle1024r) {
    const int size = 1024;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        timsort(a.begin(), a.end(), std::greater<int>());

        int j = size;
        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (--j + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(partial_reversed1023) {
    const int size = 1023;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::reverse(a.begin(), a.begin() + (size / 2)); // partial reversed

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(partial_reversed1024) {
    const int size = 1024;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::reverse(a.begin(), a.begin() + (size / 2)); // partial reversed

        timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(c_array) {
    int a[] = {7, 1, 5, 3, 9};

    timsort(a, a + sizeof(a) / sizeof(int), std::less<int>());

    BOOST_CHECK_EQUAL(a[0], 1);
    BOOST_CHECK_EQUAL(a[1], 3);
    BOOST_CHECK_EQUAL(a[2], 5);
    BOOST_CHECK_EQUAL(a[3], 7);
    BOOST_CHECK_EQUAL(a[4], 9);
}

BOOST_AUTO_TEST_CASE(string_array) {
    std::string a[] = {"7", "1", "5", "3", "9"};

    timsort(a, a + sizeof(a) / sizeof(std::string), std::less<std::string>());

    BOOST_CHECK_EQUAL(a[0], "1");
    BOOST_CHECK_EQUAL(a[1], "3");
    BOOST_CHECK_EQUAL(a[2], "5");
    BOOST_CHECK_EQUAL(a[3], "7");
    BOOST_CHECK_EQUAL(a[4], "9");
}

struct NonDefaultConstructible {
    int i;

    NonDefaultConstructible(int i_) : i(i_) {
    }

    friend bool operator<(NonDefaultConstructible const &x, NonDefaultConstructible const &y) {
        return x.i < y.i;
    }
};

BOOST_AUTO_TEST_CASE(non_default_constructible) {
    NonDefaultConstructible a[] = {7, 1, 5, 3, 9};

    timsort(a, a + sizeof(a) / sizeof(a[0]), std::less<NonDefaultConstructible>());

    BOOST_CHECK_EQUAL(a[0].i, 1);
    BOOST_CHECK_EQUAL(a[1].i, 3);
    BOOST_CHECK_EQUAL(a[2].i, 5);
    BOOST_CHECK_EQUAL(a[3].i, 7);
    BOOST_CHECK_EQUAL(a[4].i, 9);
}

BOOST_AUTO_TEST_CASE(default_compare_function) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    timsort(a.begin(), a.end());

    BOOST_CHECK_EQUAL(a.size(), std::size_t(size));
    for (int i = 0; i < size; ++i) {
        BOOST_CHECK_EQUAL(a[i], (i + 1) * 10);
    }
}

enum id { foo, bar, baz };
typedef std::pair<int, id> pair_t;
bool less_in_first(pair_t x, pair_t y) {
    return x.first < y.first;
}

BOOST_AUTO_TEST_CASE(stability) {
    std::vector<pair_t> a;

    for (int i = 100; i >= 0; --i) {
        a.push_back(std::make_pair(i, foo));
        a.push_back(std::make_pair(i, bar));
        a.push_back(std::make_pair(i, baz));
    }

    timsort(a.begin(), a.end(), &less_in_first);

    BOOST_CHECK_EQUAL(a[0].first, 0);
    BOOST_CHECK_EQUAL(a[0].second, foo);
    BOOST_CHECK_EQUAL(a[1].first, 0);
    BOOST_CHECK_EQUAL(a[1].second, bar);
    BOOST_CHECK_EQUAL(a[2].first, 0);
    BOOST_CHECK_EQUAL(a[2].second, baz);

    BOOST_CHECK_EQUAL(a[3].first, 1);
    BOOST_CHECK_EQUAL(a[3].second, foo);
    BOOST_CHECK_EQUAL(a[4].first, 1);
    BOOST_CHECK_EQUAL(a[4].second, bar);
    BOOST_CHECK_EQUAL(a[5].first, 1);
    BOOST_CHECK_EQUAL(a[5].second, baz);

    BOOST_CHECK_EQUAL(a[6].first, 2);
    BOOST_CHECK_EQUAL(a[6].second, foo);
    BOOST_CHECK_EQUAL(a[7].first, 2);
    BOOST_CHECK_EQUAL(a[7].second, bar);
    BOOST_CHECK_EQUAL(a[8].first, 2);
    BOOST_CHECK_EQUAL(a[8].second, baz);

    BOOST_CHECK_EQUAL(a[9].first, 3);
    BOOST_CHECK_EQUAL(a[9].second, foo);
    BOOST_CHECK_EQUAL(a[10].first, 3);
    BOOST_CHECK_EQUAL(a[10].second, bar);
    BOOST_CHECK_EQUAL(a[11].first, 3);
    BOOST_CHECK_EQUAL(a[11].second, baz);
}

bool less_in_pair(const std::pair<int, int> &x, const std::pair<int, int> &y) {
    if (x.first == y.first) {
        return x.second < y.second;
    }
    return x.first < y.first;
}

BOOST_AUTO_TEST_CASE(issue2_compare) {
    typedef std::pair<int, int> p;
    gfx::Compare<p, bool (*)(const p &, const p &)> c(&less_in_pair);

    BOOST_CHECK_EQUAL(c.lt(std::make_pair(10, 10), std::make_pair(10, 9)), false);
    BOOST_CHECK_EQUAL(c.lt(std::make_pair(10, 10), std::make_pair(10, 10)), false);
    BOOST_CHECK_EQUAL(c.lt(std::make_pair(10, 10), std::make_pair(10, 11)), true);

    BOOST_CHECK_EQUAL(c.le(std::make_pair(10, 10), std::make_pair(10, 9)), false);
    BOOST_CHECK_EQUAL(c.le(std::make_pair(10, 10), std::make_pair(10, 10)), true);
    BOOST_CHECK_EQUAL(c.le(std::make_pair(10, 10), std::make_pair(10, 11)), true);

    BOOST_CHECK_EQUAL(c.gt(std::make_pair(10, 10), std::make_pair(10, 9)), true);
    BOOST_CHECK_EQUAL(c.gt(std::make_pair(10, 10), std::make_pair(10, 10)), false);
    BOOST_CHECK_EQUAL(c.gt(std::make_pair(10, 10), std::make_pair(10, 11)), false);

    BOOST_CHECK_EQUAL(c.ge(std::make_pair(10, 10), std::make_pair(10, 9)), true);
    BOOST_CHECK_EQUAL(c.ge(std::make_pair(10, 10), std::make_pair(10, 10)), true);
    BOOST_CHECK_EQUAL(c.ge(std::make_pair(10, 10), std::make_pair(10, 11)), false);
}

BOOST_AUTO_TEST_CASE(issue2_duplication) {
    std::vector<std::pair<int, int> > a;

    for (int i = 0; i < 10000; ++i) {
        int first = static_cast<unsigned short>(rand());
        int second = static_cast<unsigned short>(rand());

        a.push_back(std::make_pair(first, second));
    }

    std::vector<std::pair<int, int> > expected(a);

    std::sort(expected.begin(), expected.end(), &less_in_pair);
    timsort(a.begin(), a.end(), &less_in_pair);

    if (false) {
        for (std::size_t i = 0; i < a.size(); ++i) {
            std::clog << i << " ";
            std::clog << "(" << a[i].first << ", " << a[i].second << ")";
            std::clog << " ";
            std::clog << "(" << expected[i].first << ", " << expected[i].second << ") ";
            std::clog << "\n";
        }
        return;
    }

    BOOST_CHECK_EQUAL(a.size(), expected.size());

    // test some points

    BOOST_CHECK_EQUAL(a[0].first, expected[0].first);
    BOOST_CHECK_EQUAL(a[0].second, expected[0].second);

    BOOST_CHECK_EQUAL(a[1].first, expected[1].first);
    BOOST_CHECK_EQUAL(a[1].second, expected[1].second);

    BOOST_CHECK_EQUAL(a[10].first, expected[10].first);
    BOOST_CHECK_EQUAL(a[10].second, expected[10].second);

    BOOST_CHECK_EQUAL(a[11].first, expected[11].first);
    BOOST_CHECK_EQUAL(a[11].second, expected[11].second);

    BOOST_CHECK_EQUAL(a[100].first, expected[100].first);
    BOOST_CHECK_EQUAL(a[100].second, expected[100].second);

    BOOST_CHECK_EQUAL(a[101].first, expected[101].first);
    BOOST_CHECK_EQUAL(a[101].second, expected[101].second);

    BOOST_CHECK_EQUAL(a[111].first, expected[111].first);
    BOOST_CHECK_EQUAL(a[111].second, expected[111].second);

    BOOST_CHECK_EQUAL(a[112].first, expected[112].first);
    BOOST_CHECK_EQUAL(a[112].second, expected[112].second);

    BOOST_CHECK_EQUAL(a[999].first, expected[999].first);
    BOOST_CHECK_EQUAL(a[999].second, expected[999].second);

    BOOST_CHECK_EQUAL(a[1000].first, expected[1000].first);
    BOOST_CHECK_EQUAL(a[1000].second, expected[1000].second);

    BOOST_CHECK_EQUAL(a[1001].first, expected[1001].first);
    BOOST_CHECK_EQUAL(a[1001].second, expected[1001].second);
}

#if ENABLE_STD_MOVE && __cplusplus >= 201103L
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

BOOST_AUTO_TEST_CASE(shuffle10k_for_move_only_types) {
    const int size = 1024 * 10; // should be even number of elements

    std::vector<move_only<int> > a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        timsort(a.begin(), a.end(), [](const move_only<int> &x, const move_only<int> &y) { return x.value < y.value; });

        for (int i = 0; i < size; ++i) {
            BOOST_CHECK_EQUAL(a[i].value, (i + 1) * 10);
        }
    }
}

BOOST_AUTO_TEST_CASE(issue14) {
    int a[] = {15, 7,  16, 20, 25, 28, 13, 27, 34, 24, 19, 1, 6,  30, 32, 29, 10, 9,
               3,  31, 21, 26, 8,  2,  22, 14, 4,  12, 5,  0, 23, 33, 11, 17, 18};
    std::deque<int> c(std::begin(a), std::end(a));

    gfx::timsort(std::begin(c), std::end(c));
    BOOST_CHECK(std::is_sorted(std::begin(c), std::end(c)));
}

#endif // if std::move available
