/*
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>.
 * Copyright (c) 2019 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef GFX_TIMSORT_CXX_98_TEST_HELPERS_HPP
#define GFX_TIMSORT_CXX_98_TEST_HELPERS_HPP

#include <iterator>
#include <utility>

namespace test_helpers {
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

#endif // GFX_TIMSORT_CXX_98_TEST_HELPERS_HPP
