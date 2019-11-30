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
#include <catch2/catch.hpp>
#include <gfx/timsort.hpp>

namespace {
    // Helper types for the tests

    ////////////////////////////////////////////////////////////
    // Timsort should work with types that are not
    // default-constructible

    struct NonDefaultConstructible {
        int i;

        NonDefaultConstructible(int i_) : i(i_) {
        }

        friend bool operator<(NonDefaultConstructible const &x, NonDefaultConstructible const &y) {
            return x.i < y.i;
        }
    };

    ////////////////////////////////////////////////////////////
    // Tools to test the stability of the sort

    enum id { foo, bar, baz };

    typedef std::pair<int, id> pair_t;

    inline bool less_in_first(pair_t x, pair_t y) {
        return x.first < y.first;
    }

    ////////////////////////////////////////////////////////////
    // Timsort should work with iterators that don't have a
    // post-increment or post-decrement operation

    template<typename Iterator>
    class NoPostIterator
    {
        public:

            ////////////////////////////////////////////////////////////
            // Public types

            typedef typename std::iterator_traits<Iterator>::iterator_category iterator_category;
            typedef Iterator iterator_type;
            typedef typename std::iterator_traits<Iterator>::value_type value_type;
            typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
            typedef typename std::iterator_traits<Iterator>::pointer pointer;
            typedef typename std::iterator_traits<Iterator>::reference reference;

            ////////////////////////////////////////////////////////////
            // Constructors

            NoPostIterator() : _it() {
            }

            explicit NoPostIterator(Iterator it) : _it(it) {
            }

            ////////////////////////////////////////////////////////////
            // Members access

            iterator_type base() const {
                return _it;
            }

            ////////////////////////////////////////////////////////////
            // Element access

            reference operator*() const {
                return *base();
            }

            pointer operator->() const {
                return &(operator*());
            }

            ////////////////////////////////////////////////////////////
            // Increment/decrement operators

            NoPostIterator& operator++() {
                ++_it;
                return *this;
            }

            NoPostIterator& operator--() {
                --_it;
                return *this;
            }

            NoPostIterator& operator+=(difference_type increment) {
                _it += increment;
                return *this;
            }

            NoPostIterator& operator-=(difference_type increment) {
                _it -= increment;
                return *this;
            }

            ////////////////////////////////////////////////////////////
            // Comparison operators

            friend bool operator==(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() == rhs.base();
            }

            friend bool operator!=(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() != rhs.base();
            }

            ////////////////////////////////////////////////////////////
            // Relational operators

            friend bool operator<(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() < rhs.base();
            }

            friend bool operator<=(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() <= rhs.base();
            }

            friend bool operator>(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() > rhs.base();
            }

            friend bool operator>=(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() >= rhs.base();
            }

            ////////////////////////////////////////////////////////////
            // Arithmetic operators

            friend NoPostIterator operator+(NoPostIterator it, difference_type size) {
                return it += size;
            }

            friend NoPostIterator operator+(difference_type size, NoPostIterator it) {
                return it += size;
            }

            friend NoPostIterator operator-(NoPostIterator it, difference_type size) {
                return it -= size;
            }

            friend difference_type operator-(NoPostIterator const& lhs, NoPostIterator const& rhs) {
                return lhs.base() - rhs.base();
            }

        private:

            Iterator _it;
    };

    ////////////////////////////////////////////////////////////
    // Construction function

    template<typename Iterator>
    NoPostIterator<Iterator> make_no_post_iterator(Iterator it) {
        return NoPostIterator<Iterator>(it);
    }

}

TEST_CASE( "simple0" ) {
    std::vector<int> a;

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(0));
}

TEST_CASE( "simple1" ) {
    std::vector<int> a;

    a.push_back(42);

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(1));
    CHECK(a[0] == 42);
}

TEST_CASE( "simple2" ) {
    std::vector<int> a;

    a.push_back(10);
    a.push_back(20);

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(2));
    CHECK(a[0] == 10);
    CHECK(a[1] == 20);

    a.clear();
    a.push_back(20);
    a.push_back(10);

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(2));
    CHECK(a[0] == 10);
    CHECK(a[1] == 20);

    a.clear();
    a.push_back(10);
    a.push_back(10);

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(2));
    CHECK(a[0] == 10);
    CHECK(a[1] == 10);
}

TEST_CASE( "simple10" ) {
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

    gfx::timsort(a.begin(), a.end(), std::less<int>());

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

    gfx::timsort(a.begin(), a.end(), std::less<int>());

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

TEST_CASE( "shuffle30" ) {
    const int size = 30;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "shuffle31" ) {
    const int size = 31;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "shuffle32" ) {
    const int size = 32;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "shuffle128" ) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    gfx::timsort(a.begin(), a.end(), std::less<int>());

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "shuffle1023" ) {
    const int size = 1023; // odd number of elements

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }
}

TEST_CASE( "shuffle1024" ) {
    const int size = 1024; // should be even number of elements

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }
}

TEST_CASE( "partial_shuffle1023" ) {
    const int size = 1023;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    // sorted-shuffled-sorted pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin() + (size / 3 * 1), a.begin() + (size / 3 * 2));

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }

    // shuffled-sorted-shuffled pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.begin() + (size / 3 * 1));
        std::random_shuffle(a.begin() + (size / 3 * 2), a.end());

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }
}

TEST_CASE( "partial_shuffle1024" ) {
    const int size = 1024;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    // sorted-shuffled-sorted pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin() + (size / 3 * 1), a.begin() + (size / 3 * 2));

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }

    // shuffled-sorted-shuffled pattern
    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.begin() + (size / 3 * 1));
        std::random_shuffle(a.begin() + (size / 3 * 2), a.end());

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }
}

TEST_CASE( "shuffle1024r" ) {
    const int size = 1024;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::random_shuffle(a.begin(), a.end());

        gfx::timsort(a.begin(), a.end(), std::greater<int>());

        int j = size;
        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (--j + 1) * 10);
        }
    }
}

TEST_CASE( "partial_reversed1023" ) {
    const int size = 1023;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::reverse(a.begin(), a.begin() + (size / 2)); // partial reversed

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }
}

TEST_CASE( "partial_reversed1024" ) {
    const int size = 1024;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }

    for (int n = 0; n < 100; ++n) {
        std::reverse(a.begin(), a.begin() + (size / 2)); // partial reversed

        gfx::timsort(a.begin(), a.end(), std::less<int>());

        for (int i = 0; i < size; ++i) {
            CHECK(a[i] == (i + 1) * 10);
        }
    }
}

TEST_CASE( "c_array" ) {
    int a[] = {7, 1, 5, 3, 9};

    gfx::timsort(a, a + sizeof(a) / sizeof(int), std::less<int>());

    CHECK(a[0] == 1);
    CHECK(a[1] == 3);
    CHECK(a[2] == 5);
    CHECK(a[3] == 7);
    CHECK(a[4] == 9);
}

TEST_CASE( "string_array" ) {
    std::string a[] = {"7", "1", "5", "3", "9"};

    gfx::timsort(a, a + sizeof(a) / sizeof(std::string), std::less<std::string>());

    CHECK(a[0] == "1");
    CHECK(a[1] == "3");
    CHECK(a[2] == "5");
    CHECK(a[3] == "7");
    CHECK(a[4] == "9");
}

TEST_CASE( "non_default_constructible" ) {
    NonDefaultConstructible a[] = {7, 1, 5, 3, 9};

    gfx::timsort(a, a + sizeof(a) / sizeof(a[0]), std::less<NonDefaultConstructible>());

    CHECK(a[0].i == 1);
    CHECK(a[1].i == 3);
    CHECK(a[2].i == 5);
    CHECK(a[3].i == 7);
    CHECK(a[4].i == 9);
}

TEST_CASE( "default_compare_function" ) {
    const int size = 128;

    std::vector<int> a;
    for (int i = 0; i < size; ++i) {
        a.push_back((i + 1) * 10);
    }
    std::random_shuffle(a.begin(), a.end());

    gfx::timsort(a.begin(), a.end());

    CHECK(a.size() == std::size_t(size));
    for (int i = 0; i < size; ++i) {
        CHECK(a[i] == (i + 1) * 10);
    }
}

TEST_CASE( "stability" ) {
    std::vector<pair_t> a;

    for (int i = 100; i >= 0; --i) {
        a.push_back(std::make_pair(i, foo));
        a.push_back(std::make_pair(i, bar));
        a.push_back(std::make_pair(i, baz));
    }

    gfx::timsort(a.begin(), a.end(), &less_in_first);

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

TEST_CASE( "issue2_duplication" ) {
    std::vector<std::pair<int, int> > a;

    for (int i = 0; i < 10000; ++i) {
        int first = static_cast<unsigned short>(rand());
        int second = static_cast<unsigned short>(rand());

        a.push_back(std::make_pair(first, second));
    }

    std::vector<std::pair<int, int> > expected(a);

    std::sort(expected.begin(), expected.end());
    gfx::timsort(a.begin(), a.end());

#if 0
        for (std::size_t i = 0; i < a.size(); ++i) {
            std::clog << i << " ";
            std::clog << "(" << a[i].first << ", " << a[i].second << ")";
            std::clog << " ";
            std::clog << "(" << expected[i].first << ", " << expected[i].second << ") ";
            std::clog << "\n";
        }
        return;
#endif

    CHECK(a.size() == expected.size());

    // test some points

    CHECK(a[0].first == expected[0].first);
    CHECK(a[0].second == expected[0].second);

    CHECK(a[1].first == expected[1].first);
    CHECK(a[1].second == expected[1].second);

    CHECK(a[10].first == expected[10].first);
    CHECK(a[10].second == expected[10].second);

    CHECK(a[11].first == expected[11].first);
    CHECK(a[11].second == expected[11].second);

    CHECK(a[100].first == expected[100].first);
    CHECK(a[100].second == expected[100].second);

    CHECK(a[101].first == expected[101].first);
    CHECK(a[101].second == expected[101].second);

    CHECK(a[111].first == expected[111].first);
    CHECK(a[111].second == expected[111].second);

    CHECK(a[112].first == expected[112].first);
    CHECK(a[112].second == expected[112].second);

    CHECK(a[999].first == expected[999].first);
    CHECK(a[999].second == expected[999].second);

    CHECK(a[1000].first == expected[1000].first);
    CHECK(a[1000].second == expected[1000].second);

    CHECK(a[1001].first == expected[1001].first);
    CHECK(a[1001].second == expected[1001].second);
}

TEST_CASE( "projection" ) {
    const int size = 128;

    std::vector<int> vec;
    for (int i = 0; i < size; ++i) {
        vec.push_back(i - 40);
    }
    std::random_shuffle(vec.begin(), vec.end());

    gfx::timsort(vec.begin(), vec.end(), std::greater<int>(), std::negate<int>());
    for (int i = 0; i < size; ++i) {
        CHECK(vec[i] == i - 40);
    }
}

TEST_CASE( "iterator without post-increment or post-decrement" ) {
    std::vector<int> a;

    gfx::timsort(make_no_post_iterator(a.begin()), make_no_post_iterator(a.end()));

    CHECK(a.size() == std::size_t(0));
}
