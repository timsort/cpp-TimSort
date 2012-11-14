/*
 * C++ implementation of timsort
 *
 * ported from http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/raw_files/new/src/share/classes/java/util/TimSort.java
 *
 * Copyright 2011 Fuji, Goro <gfuji@cpan.org>. All rights reserved.
 *
 * You can use and/or redistribute in the same term of the original license.
 */


/* Here is the original license in TimSort.java:
 *
 *
 * Copyright 2009 Google Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

#ifndef GFX_TIMSORT_HPP
#define GFX_TIMSORT_HPP

#include <vector>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <utility>
#include <stdexcept>

#ifdef ENABLE_TIMSORT_LOG
#include <iostream>
#define LOG(expr) (std::clog << "# " << __func__ << ": " << expr << std::endl)
#else
#define LOG(expr) ((void)0)
#endif

#if ENABLE_STD_MOVE && __cplusplus >= 201103L
#define MOVE(x) std::move(x)
#else
#define MOVE(x) (x)
#endif

namespace gfx {

template <typename Value, typename LessFunction> class Compare {
    public:
        typedef Value        value_type;
        typedef LessFunction func_type;

        Compare(LessFunction f)
            : less_(f) { }
        Compare(const Compare<value_type, func_type>& other)
            : less_(other.less_) { }

        bool lt(value_type x, value_type y) {
            return less_(x, y);
        }
        bool le(value_type x, value_type y) {
            return less_(x, y) || !less_(y, x);
        }
        bool gt(value_type x, value_type y) {
            return !less_(x, y) && less_(y, x);
        }
        bool ge(value_type x, value_type y) {
            return !less_(x, y);
        }

        func_type& less_function() {
            return less_;
        }
    private:
        func_type less_;
};

template <typename RandomAccessIterator, typename LessFunction>
class TimSort {
    typedef RandomAccessIterator iter_t;
    typedef typename std::iterator_traits<iter_t>::value_type value_t;
    typedef typename std::iterator_traits<iter_t>::reference ref_t;
    typedef typename std::iterator_traits<iter_t>::difference_type diff_t;
    typedef Compare<const value_t&, LessFunction> compare_t;

    static const int MIN_MERGE = 32;

    compare_t comp_;

    static const int MIN_GALLOP = 7;

    int minGallop_; // default to MIN_GALLOP

    static const int INITIAL_TMP_STORAGE_LENGTH = 256;

    std::vector<value_t> tmp_; // temp storage for merges
    typedef typename std::vector<value_t>::iterator tmp_iter_t;

    std::vector<iter_t> runBase_;
    std::vector<diff_t> runLen_;

    static
    void sort(iter_t const lo, iter_t const hi, compare_t c) {
        assert( lo <= hi );

        diff_t nRemaining = (hi - lo);
        if(nRemaining < 2) {
            return; // nothing to do
        }

        if(nRemaining < MIN_MERGE) {
            diff_t const initRunLen = countRunAndMakeAscending(lo, hi, c);
            LOG("initRunLen: " << initRunLen);
            binarySort(lo, hi, lo + initRunLen, c);
            return;
        }

        TimSort ts(lo, hi, c);
        diff_t const minRun = minRunLength(nRemaining);
        iter_t cur          = lo;
        do {
            diff_t runLen = countRunAndMakeAscending(cur, hi, c);

            if(runLen < minRun) {
                diff_t const force  = std::min(nRemaining, minRun);
                binarySort(cur, cur + force, cur + runLen, c);
                runLen = force;
            }

            ts.pushRun(cur, runLen);
            ts.mergeCollapse();

            cur        += runLen;
            nRemaining -= runLen;
        } while(nRemaining != 0);

        assert( cur == hi );
        ts.mergeForceCollapse();
        assert( ts.runBase_.size() == 1 );
        assert( ts.runLen_.size()  == 1 );
    } // sort()

    static
    void binarySort(iter_t const lo, iter_t const hi, iter_t start, compare_t compare) {
        assert( lo <= start && start <= hi );
        if(start == lo) {
            ++start;
        }
        for( ; start < hi; ++start ) {
            assert(lo <= start);
            /*const*/ value_t pivot = MOVE(*start);

            iter_t const pos = std::upper_bound(lo, start, pivot, compare.less_function());
            for(iter_t p = start; p > pos; --p) {
                *p = MOVE(*(p - 1));
            }
            *pos = MOVE(pivot);
        }
    }

    static
    diff_t countRunAndMakeAscending(iter_t const lo, iter_t const hi, compare_t compare) {
        assert( lo < hi );

        iter_t runHi = lo + 1;
        if( runHi == hi ) {
            return 1;
        }

        if(compare.lt(*(runHi++), *lo)) { // descending
            LOG("descending");
            while(runHi < hi && compare.lt(*runHi, *(runHi - 1))) {
                ++runHi;
            }
            std::reverse(lo, runHi);
        }
        else { // ascending
            LOG("ascending");
            while(runHi < hi && compare.ge(*runHi, *(runHi - 1))) {
                ++runHi;
            }
        }

        return runHi - lo;
    }

    static
    diff_t minRunLength(diff_t n) {
        assert( n >= 0 );

        diff_t r = 0;
        while(n >= MIN_MERGE) {
            r |= (n & 1);
            n >>= 1;
        }
        return n + r;
    }

    TimSort(iter_t const lo, iter_t const hi, compare_t c)
            : comp_(c), minGallop_(MIN_GALLOP) {
        assert( lo <= hi );
        diff_t const len = (hi - lo);
        tmp_.reserve(
                len < 2 * INITIAL_TMP_STORAGE_LENGTH
                    ? len >> 1
                    : INITIAL_TMP_STORAGE_LENGTH );

        runBase_.reserve(40);
        runLen_.reserve(40);
    }

    void pushRun(iter_t const runBase, diff_t const runLen) {
        runBase_.push_back(runBase);
        runLen_.push_back(runLen);
    }

    void mergeCollapse() {
        while( runBase_.size() > 1 ) {
            diff_t n = runBase_.size() - 2;

            if(n > 0 && runLen_[n - 1] <= runLen_[n] + runLen_[n + 1]) {
                if(runLen_[n - 1] < runLen_[n + 1]) {
                    --n;
                }
                mergeAt(n);
            }
            else if(runLen_[n] <= runLen_[n + 1]) {
                mergeAt(n);
            }
            else {
                break;
            }
        }
    }

    void mergeForceCollapse() {
        while( runBase_.size() > 1 ) {
            diff_t n = runBase_.size() - 2;

            if(n > 0 && runLen_[n - 1] <= runLen_[n + 1]) {
                --n;
            }
            mergeAt(n);
        }

    }

    void mergeAt(diff_t const i) {
        diff_t const stackSize = runBase_.size();
        assert( stackSize >= 2 );
        assert( i >= 0 );
        assert( i == stackSize - 2 || i == stackSize - 3 );

        iter_t base1 = runBase_[i];
        diff_t len1  = runLen_ [i];
        iter_t base2 = runBase_[i + 1];
        diff_t len2  = runLen_ [i + 1];

        assert( len1 > 0 && len2 > 0 );
        assert( base1 + len1 == base2 );

        runLen_[i] = len1 + len2;

        if(i == stackSize - 3) {
            runBase_[i + 1] = runBase_[i + 2];
            runLen_ [i + 1] = runLen_ [i + 2];
        }

        runBase_.pop_back();
        runLen_.pop_back();

        diff_t const k = gallopRight(*base2, base1, len1, 0, comp_);
        assert( k >= 0 );

        base1 += k;
        len1  -= k;

        if(len1 == 0) {
            return;
        }

        len2 = gallopLeft(*(base1 + (len1 - 1)), base2, len2, len2 - 1, comp_);
        assert( len2 >= 0 );
        if(len2 == 0) {
            return;
        }

        if(len1 <= len2) {
            mergeLo(base1, len1, base2, len2);
        }
        else {
            mergeHi(base1, len1, base2, len2);
        }
    }

    template <typename Iter>
    static
    diff_t gallopLeft(ref_t key, Iter const base, diff_t const len, diff_t const hint, compare_t compare) {
        assert( len > 0 && hint >= 0 && hint < len );

        diff_t lastOfs = 0;
        diff_t ofs = 1;

        if(compare.gt(key, *(base + hint))) {
            diff_t const maxOfs = len - hint;
            while(ofs < maxOfs && compare.gt(key, *(base + (hint + ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) { // int overflow
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            lastOfs += hint;
            ofs     += hint;
        }
        else {
            diff_t const maxOfs = hint + 1;
            while(ofs < maxOfs && compare.le(key, *(base + (hint - ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) {
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            diff_t const tmp = lastOfs;
            lastOfs          = hint - ofs;
            ofs              = hint - tmp;
        }
        assert( -1 <= lastOfs && lastOfs < ofs && ofs <= len );

        return std::lower_bound(base+(lastOfs+1), base+ofs, key, compare.less_function()) - base;
    }

    template <typename Iter>
    static
    diff_t gallopRight(ref_t key, Iter const base, diff_t const len, diff_t const hint, compare_t compare) {
        assert( len > 0 && hint >= 0 && hint < len );

        diff_t ofs = 1;
        diff_t lastOfs = 0;

        if(compare.lt(key, *(base + hint))) {
            diff_t const maxOfs = hint + 1;
            while(ofs < maxOfs && compare.lt(key, *(base + (hint - ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) {
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            diff_t const tmp = lastOfs;
            lastOfs          = hint - ofs;
            ofs              = hint - tmp;
        }
        else {
            diff_t const maxOfs = len - hint;
            while(ofs < maxOfs && compare.ge(key, *(base + (hint + ofs)))) {
                lastOfs = ofs;
                ofs     = (ofs << 1) + 1;

                if(ofs <= 0) { // int overflow
                    ofs = maxOfs;
                }
            }
            if(ofs > maxOfs) {
                ofs = maxOfs;
            }

            lastOfs += hint;
            ofs     += hint;
        }
        assert( -1 <= lastOfs && lastOfs < ofs && ofs <= len );

        return std::upper_bound(base+(lastOfs+1), base+ofs, key, compare.less_function()) - base;
    }

    void mergeLo(iter_t const base1, diff_t len1, iter_t const base2, diff_t len2) {
        assert( len1 > 0 && len2 > 0 && base1 + len1 == base2 );

        tmp_.clear();
        tmp_.reserve(len1);
        std::copy(base1, base1 + len1, std::back_inserter(tmp_));

        tmp_iter_t cursor1 = tmp_.begin();
        iter_t cursor2     = base2;
        iter_t dest        = base1;

        *(dest++) = *(cursor2++);
        if(--len2 == 0) {
            std::copy(cursor1, cursor1 + len1, dest);
            return;
        }
        if(len1 == 1) {
            std::copy(cursor2, cursor2 + len2, dest);
            *(dest + len2) = *cursor1;
            return;
        }

        compare_t compare(comp_);
        int minGallop(minGallop_);

        // outer:
        while(true) {
            int count1 = 0;
            int count2 = 0;

            bool break_outer = false;
            do {
                assert( len1 > 1 && len2 > 0 );

                if(compare.lt(*cursor2, *cursor1)) {
                    *(dest++) = *(cursor2++);
                    ++count2;
                    count1 = 0;
                    if(--len2 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                else {
                    *(dest++) = *(cursor1++);
                    ++count1;
                    count2 = 0;
                    if(--len1 == 1) {
                        break_outer = true;
                        break;
                    }
                }
            } while( (count1 | count2) < minGallop );
            if(break_outer) {
                break;
            }

            do {
                assert( len1 > 1 && len2 > 0 );

                count1 = gallopRight(*cursor2, cursor1, len1, 0, compare);
                if(count1 != 0) {
                    std::copy(cursor1, cursor1 + count1, dest);
                    dest    += count1;
                    cursor1 += count1;
                    len1    -= count1;

                    if(len1 <= 1) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest++) = *(cursor2++);
                if(--len2 == 0) {
                    break_outer = true;
                    break;
                }

                count2 = gallopLeft(*cursor1, cursor2, len2, 0, compare);
                if(count2 != 0) {
                    std::copy(cursor2, cursor2 + count2, dest);
                    dest    += count2;
                    cursor2 += count2;
                    len2    -= count2;
                    if(len2 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest++) = *(cursor1++);
                if(--len1 == 1) {
                    break_outer = true;
                    break;
                }

                --minGallop;
            } while( (count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP) );
            if(break_outer) {
                break;
            }

            if(minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;
        } // end of "outer" loop

        minGallop_ = std::min(minGallop, 1);

        if(len1 == 1) {
            assert( len2 > 0 );
            std::copy(cursor2, cursor2 + len2, dest);
            *(dest + len2) = *cursor1;
        }
        else if(len1 == 0) {
            throw std::invalid_argument("Comparision method violates its general contract");
        }
        else {
            assert( len2 == 0 );
            assert( len1 > 1 );
            std::copy(cursor1, cursor1 + len1, dest);
        }
    }

    void mergeHi(iter_t const base1, diff_t len1, iter_t const base2, diff_t len2) {
        assert( len1 > 0 && len2 > 0 && base1 + len1 == base2 );

        tmp_.clear();
        tmp_.reserve(len2);
        std::copy(base2, base2 + len2, std::back_inserter(tmp_));

        iter_t cursor1     = base1 + (len1 - 1);
        tmp_iter_t cursor2 = tmp_.begin() + (len2 - 1);
        iter_t dest        = base2 + (len2 - 1);

        *(dest--) = *(cursor1--);
        if(--len1 == 0) {
            std::copy(tmp_.begin(), tmp_.begin() + len2, dest - (len2 - 1));
            return;
        }
        if(len2 == 1) {
            dest    -= len1;
            cursor1 -= len1;
            std::copy(cursor1 + 1, cursor1 + (1 + len1), dest + 1);
            *dest = *cursor2;
            return;
        }

        compare_t compare( comp_ );
        int minGallop( minGallop_ );

        // outer:
        while(true) {
            int count1 = 0;
            int count2 = 0;

            bool break_outer = false;
            do {
                assert( len1 > 0 && len2 > 1 );

                if(compare.lt(*cursor2, *cursor1)) {
                    *(dest--) = *(cursor1--);
                    ++count1;
                    count2 = 0;
                    if(--len1 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                else {
                    *(dest--) = *(cursor2--);
                    ++count2;
                    count1 = 0;
                    if(--len2 == 1) {
                        break_outer = true;
                        break;
                    }
                }
            } while( (count1 | count2) < minGallop );
            if(break_outer) {
                break;
            }

            do {
                assert( len1 > 0 && len2 > 1 );

                count1 = len1 - gallopRight(*cursor2, base1, len1, len1 - 1, compare);
                if(count1 != 0) {
                    dest    -= count1;
                    cursor1 -= count1;
                    len1    -= count1;
                    std::copy(cursor1 + 1, cursor1 + (1 + count1), dest + 1);

                    if(len1 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest--) = *(cursor2--);
                if(--len2 == 1) {
                    break_outer = true;
                    break;
                }

                count2 = len2 - gallopLeft(*cursor1, tmp_.begin(), len2, len2 - 1, compare);
                if(count2 != 0) {
                    dest    -= count2;
                    cursor2 -= count2;
                    len2    -= count2;
                    std::copy(cursor2 + 1, cursor2 + (1 + count2), dest + 1);
                    if(len2 <= 1) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest--) = *(cursor1--);
                if(--len1 == 0) {
                    break_outer = true;
                    break;
                }

                minGallop--;
            } while( (count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP) );
            if(break_outer) {
                break;
            }

            if(minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;
        } // end of "outer" loop

        minGallop_ = std::min(minGallop, 1);

        if(len2 == 1) {
            assert( len1 > 0 );
            dest    -= len1;
            cursor1 -= len1;
            std::copy(cursor1 + 1, cursor1 + (1 + len1), dest + 1);
            *dest = *cursor2;
        }
        else if(len2 == 0) {
            throw std::invalid_argument("Comparision method violates its general contract");
        }
        else {
            assert( len1 == 0 );
            assert( len2 > 1 );
            std::copy(tmp_.begin(), tmp_.begin() + len2, dest - (len2 - 1));
        }
    }

    // the only interface is the friend timsort() function
    template <typename IterT, typename LessT>
    friend void timsort(IterT first, IterT last, LessT c);
};

template<typename IterT, typename LessT>
inline void timsort(IterT const first, IterT const last, LessT c) {
    TimSort<IterT, LessT>::sort(first, last, c);
}

} // namespace gfx

#undef LOG
#undef MOVE
#endif // GFX_TIMSORT_HPP

