/*
 * C++ implementation of timsort
 *
 * ported from Python's and OpenJDK's:
 * - http://svn.python.org/projects/python/trunk/Objects/listobject.c
 * - http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/raw_files/new/src/share/classes/java/util/TimSort.java
 *
 * Copyright (c) 2011 Fuji, Goro (gfx) <gfuji@cpan.org>. C++03/move-compliance modifications by Matt Bentley 2017 (mattreecebentley@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef GFX_TIMSORT_HPP
#define GFX_TIMSORT_HPP

#include <memory> // std::allocator
#include <cassert>
#include <algorithm> // std::copy
#include <functional> // std::less

#ifdef ENABLE_TIMSORT_LOG
#include <iostream>
#define GFX_TIMSORT_LOG(expr) (std::clog << "# " << __func__ << ": " << expr << std::endl)
#else
#define GFX_TIMSORT_LOG(expr) ((void)0)
#endif

// The "GFX_TIMSORT_USE_CXX11" define can be used to control whether we
// use C++11 extensions like type-traits and move semantics.  By default
// it is enabled unless we suspect that the compiler or STL is too old
// to support them:
#ifndef GFX_TIMSORT_USE_CXX11
#  define GFX_TIMSORT_USE_CXX11 1
#  ifdef _MSC_VER
#    if _MSC_VER < 1700
#      undef GFX_TIMSORT_USE_CXX11
#    endif
#  else
#    ifdef __cplusplus
#      if __cplusplus < 201103L
#        undef GFX_TIMSORT_USE_CXX11
#      endif
#    else
#      undef GFX_TIMSORT_USE_CXX11
#    endif
#    if defined(__cplusplus) && __cplusplus < 201103L
#      undef GFX_TIMSORT_USE_CXX11
#    endif
#    ifdef _GLIBCXX_RELEASE
       // This setting only got added in gcc 7.1, so its presence always
       // indicates a C++11-ready STL
#    elif defined(__GLIBCXX__)
       // Before 7.1, the only way to test the version of libstdc++ is the
       // __GLIBCXX__ date macro.  However, it's not monotonically increasing
       // releases kepy being made from older branches.  The best we can do is
       // to disallow any version that is definitely before gcc 5.1 (the first
       // version that had enough C++11 support for us) and then blacklist
       // dates that are known to correspond with non-working versions.
       //
       // Note this really only is a problem when mixing compilers and STL (i.e.
       // compiling using clang but using gcc's libstdc++)   Otherwise we'll
       // correctly reject the gcc compiler if it's too old later.
#      if __GLIBCXX__ < 20150422
#        undef GFX_TIMSORT_USE_CXX11
#      endif
#      if __GLIBCXX__ == 20150426
#        undef GFX_TIMSORT_USE_CXX11   // gcc 4.8.4+patches shipped with Ubuntu LTS 14.04
#      endif
#      if __GLIBCXX__ == 20150623
#        undef GFX_TIMSORT_USE_CXX11   // gcc 4.8.5
#      endif
#      if __GLIBCXX__ == 20150626
#        undef GFX_TIMSORT_USE_CXX11   // gcc 4.9.3
#      endif
#      if __GLIBCXX__ == 20160803
#        undef GFX_TIMSORT_USE_CXX11   // gcc 4.9.5
#      endif
#    elif defined(__GLIBCPP__)  // *really* old version of libstdc++
#      undef GFX_TIMSORT_USE_CXX11
#    endif
#    ifdef _LIBCPP_VERSION
#      if defined(_LIBCPP_HAS_NO_RVALUE_REFERENCES) || defined(_LIBCPP_CXX03_LANG)
#        undef GFX_TIMSORT_USE_CXX11
#      endif
#    endif
#    ifdef __clang__
#      ifdef __has_feature
#        if !(__has_feature(cxx_rvalue_references) && __has_feature(is_trivially_copyable))
#          undef GFX_TIMSORT_USE_CXX11
#        endif
#      else
#        undef GFX_TIMSORT_USE_CXX11
#      endif
#    elif defined(__GNUC__)
#      if __GNUC__ < 5
#        undef GFX_TIMSORT_USE_CXX11
#      endif
#    endif
#  endif
#  ifndef GFX_TIMSORT_USE_CXX11
#    define GFX_TIMSORT_USE_CXX11 0
#  endif
#endif

#if GFX_TIMSORT_USE_CXX11
	#include <type_traits>
	#include <iterator> // iterator_traits
	#include <utility> // std::move
	#include <cstring> // std::memcpy

	#define GFX_TIMSORT_MOVE(x) (std::is_move_constructible<value_t>::value && std::is_move_assignable<value_t>::value) ? std::move(x) : (x)
	#define GFX_TIMSORT_MOVE_RANGE(in1, in2, out) \
		if (std::is_move_constructible<value_t>::value && std::is_move_assignable<value_t>::value) \
		{ \
			std::move((in1), (in2), (out)); \
		} \
		else \
		{ \
			std::copy((in1), (in2), (out)); \
		}
	#define GFX_TIMSORT_MOVE_BACKWARD(in1, in2, out) \
		if (std::is_move_constructible<value_t>::value && std::is_move_assignable<value_t>::value) \
		{ \
			std::move_backward((in1), (in2), (out)); \
		} \
		else \
		{ \
			std::copy_backward((in1), (in2), (out)); \
		}
#else
	#define GFX_TIMSORT_MOVE(x) (x)
	#define GFX_TIMSORT_MOVE_RANGE(in1, in2, out) std::copy((in1), (in2), (out)); 
	#define GFX_TIMSORT_MOVE_BACKWARD(in1, in2, out) std::copy_backward((in1), (in2), (out));
#endif



namespace gfx {

// ---------------------------------------
// Declaration
// ---------------------------------------

/**
 * Same as std::stable_sort(first, last).
 */
template <typename RandomAccessIterator>
inline void timsort(RandomAccessIterator const first, RandomAccessIterator const last);

/**
 * Same as std::stable_sort(first, last, c).
 */
template <typename RandomAccessIterator, typename LessFunction>
inline void timsort(RandomAccessIterator const first, RandomAccessIterator const last, LessFunction compare);

// ---------------------------------------
// Implementation
// ---------------------------------------

template <typename Value, typename LessFunction> class Compare {
  public:
    typedef Value value_type;
    typedef LessFunction func_type;

    Compare(LessFunction f) : less_(f) {
    }
    Compare(const Compare<value_type, func_type> &other) : less_(other.less_) {
    }

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

    func_type &less_function() {
        return less_;
    }

  private:
    func_type less_;
};

// Some details shared between the two different implementations of TimSortMergeSpace<>
template <typename value_t, typename LengthType> struct TimSortMergeSpaceBase : public std::allocator<value_t> {
    value_t *startp_;
    value_t *endp_;
    value_t const *alloc_limitp_;
    TimSortMergeSpaceBase() : startp_(0), endp_(0), alloc_limitp_(0) {
    }
    LengthType next_capacity() const {
        if (startp_ == 0) {
            return (sizeof(*startp_) > 32) ? 1 : (64 / sizeof(*startp_));
        }
        LengthType const old_cap = alloc_limitp_ - startp_;
        assert(old_cap > 0);
        return (old_cap * 3) >> 1;
    }
#if GFX_TIMSORT_USE_CXX11
  private:
    TimSortMergeSpaceBase(const TimSortMergeSpaceBase&) = delete;
    TimSortMergeSpaceBase& operator=(const TimSortMergeSpaceBase&) = delete;
#endif
};

// This is a generic memory buffer for temporary holding space during TimSort
// merge operations.  This implementation will work for any supported "value_t"
// type (one that is at least move-constructable and move-copyable)
//
// This just provides a temporary buffer which can have elements moved
// into it via the move_in() method and then iterated using begin()/size()
template <typename value_t, typename RandomAccessIterator, typename LengthType, bool ValueTypeIsTriviallyConpyable> class TimSortMergeSpace {
  public:
    typedef value_t *iterator;

    iterator begin() {
        return base_.startp_;
    }
    LengthType size() const {
        return base_.endp_ - base_.startp_;
    }
  private:
    TimSortMergeSpaceBase<value_t, LengthType> base_;
    value_t *ctor_limitp_;

    void destruct() {
        if (base_.startp_ != 0) {
            iterator const e = ctor_limitp_;
            iterator p = begin();
            do {
                p->~value_t();
            } while (++p < e);
            base_.deallocate(base_.startp_, base_.alloc_limitp_ - base_.startp_);
        }
    }
  public:
    TimSortMergeSpace() : ctor_limitp_(0) {
    }
    ~TimSortMergeSpace() {
        destruct();
    }
    void move_in(RandomAccessIterator const in_begin, LengthType const len) {
        assert(len > 0);
        value_t *nend = base_.startp_ + len;
        RandomAccessIterator const in_end = in_begin + len;
        if (nend <= ctor_limitp_) {
            GFX_TIMSORT_MOVE_RANGE(in_begin, in_end, base_.startp_);
        } else {
            // We'll have to construct at least one new element; s..in_end
            // represents the range of source elements that will need
            // that treatment
            RandomAccessIterator s;
            if (nend <= base_.alloc_limitp_) {
                // We don't need to allocate new memory, but we do need
                // to call the constructor on some of our elements since
                // we haven't been this size before.  The others we can
                // just use move_range() on:
                s = in_begin + (ctor_limitp_ - base_.startp_);
                GFX_TIMSORT_MOVE_RANGE(in_begin, s, base_.startp_);
            } else {
                // Our current allocation is too small so allocate a
                // new array entirely
                LengthType const new_cap = std::max(base_.next_capacity(), len);
                destruct();
                value_t *nv;
                try {
                    nv = base_.allocate(new_cap);
                } catch (...) {
                    base_.startp_ = 0;
                    base_.endp_ = 0;
                    base_.alloc_limitp_ = 0;
                    ctor_limitp_ = 0;
                    throw;
                }
                base_.startp_ = nv;
                base_.alloc_limitp_ = nv + new_cap;
                ctor_limitp_ = nv;
                nend = nv + len;
                s = in_begin;
            }
            do {
                new(ctor_limitp_) value_t(GFX_TIMSORT_MOVE(*s));
                ++ctor_limitp_;
            } while (++s < in_end);
            assert(nend <= ctor_limitp_);
            assert(ctor_limitp_ <= base_.alloc_limitp_);
        }
        base_.endp_ = nend;
    }
};

#if GFX_TIMSORT_USE_CXX11
// Simpler specialization of TimSortMergeSpace<> for trivially_constructable
// value_t's.  Here we don't need to bother calling the constructor/destructors
// on each element so we can use move_range().  This is significantly faster
// because it lets us use the well optimized memcpy() instead of a per-element loop.
template <typename value_t, typename RandomAccessIterator, typename LengthType> class TimSortMergeSpace<value_t, RandomAccessIterator, LengthType, true> {
  public:
    typedef value_t *iterator;

    iterator begin() {
        return base_.startp_;
    }
    LengthType size() const {
        return base_.endp_ - base_.startp_;
    }
  private:
    TimSortMergeSpaceBase<value_t, LengthType> base_;

    void destruct() {
        if (base_.startp_ != 0) {
            base_.deallocate(base_.startp_, base_.alloc_limitp_ - base_.startp_);
        }
    }
  public:
    TimSortMergeSpace() {
    }
    ~TimSortMergeSpace() {
        destruct();
    }
    void move_in(RandomAccessIterator const in_begin, LengthType const len) {
        assert(len > 0);
        value_t *nend = base_.startp_ + len;
        if (nend > base_.alloc_limitp_) {
            // Our current allocation is too small so allocate a
            // new array entirely
            LengthType const new_cap = std::max(base_.next_capacity(), len);
            destruct();
            value_t *nv;
            try {
                nv = base_.allocate(new_cap);
            } catch (...) {
                base_.startp_ = 0;
                base_.endp_ = 0;
                base_.alloc_limitp_ = 0;
                throw;
            }
            base_.startp_ = nv;
            base_.alloc_limitp_ = nv + new_cap;
            nend = nv + len;
            assert(nend <= base_.alloc_limitp_);
        }
        // The most correct way to copy these trivially-constructable elements
        // into base_.startp_ would be to do:
        //   GFX_TIMSORT_MOVE_RANGE(in_begin, in_begin + len, base_.startp_);
        // Since the types are default-constructable the STL will be able to
        // reduce this to a call to std::memmove()   However, we know that we're
        // never dealing with overlapping memory here, so it's a tiny bit
        // faster to use std::memcpy() instead.
        std::memcpy(base_.startp_, &*in_begin, len * sizeof(base_.startp_[0]));
        base_.endp_ = nend;
    }
};
#endif

// Dynamically-allocated stack of pending "runs" that TimSort needs to merge.
template <typename RandomAccessIterator, typename LengthType> class TimSortRunStack {
  public:
    struct run {
        RandomAccessIterator base;
        LengthType len;
    };
  private:
    struct contents : public std::allocator<struct run> {
        contents() : startp_(0), endp_(0), alloc_limitp_(0) {
        }
        struct run *startp_;
        struct run *endp_;
        struct run const *alloc_limitp_;

        struct run *push_back_grow() {
            assert(endp_ == alloc_limitp_);
            LengthType const old_cap = (endp_ - startp_);
            LengthType const new_cap = (startp_ == 0) ? 16 : ((old_cap * 3) >> 1);
            assert(new_cap > old_cap);
            struct run *nv = this->allocate(new_cap);
            if (startp_ != 0) {
                std::copy(startp_, endp_, nv);
                this->deallocate(startp_, endp_ - startp_);
            }
            startp_ = nv;
            // endp_ is set by our caller
            alloc_limitp_ = nv + new_cap;
            return nv + old_cap + 1;
        }
    };
    struct contents c_;
#if GFX_TIMSORT_USE_CXX11
    TimSortRunStack(const TimSortRunStack&) = delete;
    TimSortRunStack& operator=(const TimSortRunStack&) = delete;
#endif
  public:
    TimSortRunStack() {
    }
    ~TimSortRunStack() {
        c_.deallocate(c_.startp_, c_.alloc_limitp_ - c_.startp_);
    }
    void push_back(RandomAccessIterator const runBase, LengthType const runLen) {
        struct run *nend = c_.endp_ + 1;
        if (nend > c_.alloc_limitp_) {
            nend = c_.push_back_grow();
        }
        c_.endp_ = nend;
        nend[-1].base = runBase;
        nend[-1].len = runLen;
    }
    void pop_back() {
        assert(c_.endp_ > c_.startp_);
        --c_.endp_;
    }
    LengthType size() const {
        return c_.endp_ - c_.startp_;
    }
    struct run& operator[](LengthType const i) {
        return c_.startp_[i];
    }
};

namespace timsort_constants {
static const int MIN_GALLOP = 7;
static const int MIN_MERGE = 32;
} // namespace

// This holds all of the TimSort state that is invariant with respect to
// LessFunction.  In other words, this template expansion can be shared
// by the compiler when sorting the same type in different orderings.
template <typename RandomAccessIterator> class TimSortState {
    typedef RandomAccessIterator iter_t;
    typedef typename std::iterator_traits<iter_t>::value_type value_t;
    typedef typename std::iterator_traits<iter_t>::difference_type diff_t;

    TimSortRunStack<iter_t, diff_t> pending_;

#if GFX_TIMSORT_USE_CXX11
    typedef TimSortMergeSpace<value_t, iter_t, diff_t, std::is_trivially_copyable<value_t>::value> merge_space;
#else
    typedef TimSortMergeSpace<value_t, iter_t, diff_t, false> merge_space;
#endif
    merge_space tmp_; // temp storage for merges

    int minGallop_; // default to MIN_GALLOP

    TimSortState() : minGallop_(timsort_constants::MIN_GALLOP) {
    }
    ~TimSortState() {
    }

    static diff_t minRunLength(diff_t n) {
        using namespace timsort_constants;
        assert(n >= 0);

        diff_t r = 0;
        while (n >= MIN_MERGE) {
            r |= (n & 1);
            n >>= 1;
        }
        return n + r;
    }
    template <typename RA_Iterator, typename LessFunction> friend class TimSort;
};

template <typename RandomAccessIterator, typename LessFunction> class TimSort
{
    TimSortState<RandomAccessIterator> state_;

    typedef RandomAccessIterator iter_t;
    typedef typename std::iterator_traits<iter_t>::value_type value_t;
    typedef typename std::iterator_traits<iter_t>::difference_type diff_t;
    typedef typename std::iterator_traits<iter_t>::reference ref_t;
    typedef typename TimSortState<RandomAccessIterator>::merge_space::iterator tmp_iter_t;
    typedef Compare<const value_t &, LessFunction> compare_t;

    compare_t comp_;

    static void sort(iter_t const lo, iter_t const hi, compare_t c) {
        using namespace timsort_constants;
        assert(lo <= hi);

        diff_t nRemaining = (hi - lo);
        if (nRemaining < 2) {
            return; // nothing to do
        }

        if (nRemaining < MIN_MERGE) {
            diff_t const initRunLen = countRunAndMakeAscending(lo, hi, c);
            GFX_TIMSORT_LOG("initRunLen: " << initRunLen);
            binarySort(lo, hi, lo + initRunLen, c);
            return;
        }

        TimSort ts(c);
        diff_t const minRun = TimSortState<RandomAccessIterator>::minRunLength(nRemaining);
        iter_t cur = lo;
        do {
            diff_t runLen = countRunAndMakeAscending(cur, hi, c);

            if (runLen < minRun) {
                diff_t const force = std::min(nRemaining, minRun);
                binarySort(cur, cur + force, cur + runLen, c);
                runLen = force;
            }

            ts.state_.pending_.push_back(cur, runLen);
            ts.mergeCollapse();

            cur += runLen;
            nRemaining -= runLen;
        } while (nRemaining != 0);

        assert(cur == hi);
        ts.mergeForceCollapse();
        assert(ts.state_.pending_.size() == 1);

        GFX_TIMSORT_LOG("size: " << (hi - lo) << " tmp_.size(): " << ts.tmp_.size()
                                 << " pending_.size(): " << ts.state_.pending_.size());
    } // sort()

    static void binarySort(iter_t const lo, iter_t const hi, iter_t start, compare_t compare) {
        assert(lo <= start && start <= hi);
        if (start == lo) {
            ++start;
        }
        for (; start < hi; ++start) {
            assert(lo <= start);
            /*const*/ value_t pivot = GFX_TIMSORT_MOVE(*start);

            iter_t const pos = std::upper_bound(lo, start, pivot, compare.less_function());
            for (iter_t p = start; p > pos; --p) {
                *p = GFX_TIMSORT_MOVE(*(p - 1));
            }
            *pos = GFX_TIMSORT_MOVE(pivot);
        }
    }

    static diff_t countRunAndMakeAscending(iter_t const lo, iter_t const hi, compare_t compare) {
        assert(lo < hi);

        iter_t runHi = lo + 1;
        if (runHi == hi) {
            return 1;
        }

        if (compare.lt(*(runHi++), *lo)) { // descending
            while (runHi < hi && compare.lt(*runHi, *(runHi - 1))) {
                ++runHi;
            }
            std::reverse(lo, runHi);
        } else { // ascending
            while (runHi < hi && compare.ge(*runHi, *(runHi - 1))) {
                ++runHi;
            }
        }

        return runHi - lo;
    }

    explicit TimSort(compare_t c) : comp_(c) {
    }

    void mergeCollapse() {
        while (state_.pending_.size() > 1) {
            diff_t n = state_.pending_.size() - 2;

            if ((n > 0 && state_.pending_[n - 1].len <= state_.pending_[n].len + state_.pending_[n + 1].len) ||
                (n > 1 && state_.pending_[n - 2].len <= state_.pending_[n - 1].len + state_.pending_[n].len)) {
                if (state_.pending_[n - 1].len < state_.pending_[n + 1].len) {
                    --n;
                }
                mergeAt(n);
            } else if (state_.pending_[n].len <= state_.pending_[n + 1].len) {
                mergeAt(n);
            } else {
                break;
            }
        }
    }

    void mergeForceCollapse() {
        while (state_.pending_.size() > 1) {
            diff_t n = state_.pending_.size() - 2;

            if (n > 0 && state_.pending_[n - 1].len < state_.pending_[n + 1].len) {
                --n;
            }
            mergeAt(n);
        }
    }

    void mergeAt(diff_t const i) {
        diff_t const stackSize = state_.pending_.size();
        assert(stackSize >= 2);
        assert(i >= 0);
        assert(i == stackSize - 2 || i == stackSize - 3);

        iter_t base1 = state_.pending_[i].base;
        diff_t len1 = state_.pending_[i].len;
        iter_t base2 = state_.pending_[i + 1].base;
        diff_t len2 = state_.pending_[i + 1].len;

        assert(len1 > 0 && len2 > 0);
        assert(base1 + len1 == base2);

        state_.pending_[i].len = len1 + len2;

        if (i == stackSize - 3) {
            state_.pending_[i + 1] = state_.pending_[i + 2];
        }

        state_.pending_.pop_back();

        diff_t const k = gallopRight(*base2, base1, len1, 0);
        assert(k >= 0);

        base1 += k;
        len1 -= k;

        if (len1 == 0) {
            return;
        }

        len2 = gallopLeft(*(base1 + (len1 - 1)), base2, len2, len2 - 1);
        assert(len2 >= 0);
        if (len2 == 0) {
            return;
        }

        if (len1 <= len2) {
            mergeLo(base1, len1, base2, len2);
        } else {
            mergeHi(base1, len1, base2, len2);
        }
    }

    template <typename Iter> diff_t gallopLeft(ref_t key, Iter const base, diff_t const len, diff_t const hint) {
        assert(len > 0 && hint >= 0 && hint < len);

        diff_t lastOfs = 0;
        diff_t ofs = 1;

        if (comp_.gt(key, *(base + hint))) {
            diff_t const maxOfs = len - hint;
            while (ofs < maxOfs && comp_.gt(key, *(base + (hint + ofs)))) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;

                if (ofs <= 0) { // int overflow
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }

            lastOfs += hint;
            ofs += hint;
        } else {
            diff_t const maxOfs = hint + 1;
            while (ofs < maxOfs && comp_.le(key, *(base + (hint - ofs)))) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;

                if (ofs <= 0) {
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }

            diff_t const tmp = lastOfs;
            lastOfs = hint - ofs;
            ofs = hint - tmp;
        }
        assert(-1 <= lastOfs && lastOfs < ofs && ofs <= len);

        return std::lower_bound(base + (lastOfs + 1), base + ofs, key, comp_.less_function()) - base;
    }

    template <typename Iter> diff_t gallopRight(ref_t key, Iter const base, diff_t const len, diff_t const hint) {
        assert(len > 0 && hint >= 0 && hint < len);

        diff_t ofs = 1;
        diff_t lastOfs = 0;

        if (comp_.lt(key, *(base + hint))) {
            diff_t const maxOfs = hint + 1;
            while (ofs < maxOfs && comp_.lt(key, *(base + (hint - ofs)))) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;

                if (ofs <= 0) {
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }

            diff_t const tmp = lastOfs;
            lastOfs = hint - ofs;
            ofs = hint - tmp;
        } else {
            diff_t const maxOfs = len - hint;
            while (ofs < maxOfs && comp_.ge(key, *(base + (hint + ofs)))) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;

                if (ofs <= 0) { // int overflow
                    ofs = maxOfs;
                }
            }
            if (ofs > maxOfs) {
                ofs = maxOfs;
            }

            lastOfs += hint;
            ofs += hint;
        }
        assert(-1 <= lastOfs && lastOfs < ofs && ofs <= len);

        return std::upper_bound(base + (lastOfs + 1), base + ofs, key, comp_.less_function()) - base;
    }

    void mergeLo(iter_t const base1, diff_t len1, iter_t const base2, diff_t len2) {
        using namespace timsort_constants;
        assert(len1 > 0 && len2 > 0 && base1 + len1 == base2);

        state_.tmp_.move_in(base1, len1);

        tmp_iter_t cursor1 = state_.tmp_.begin();
        iter_t cursor2 = base2;
        iter_t dest = base1;

        *(dest++) = GFX_TIMSORT_MOVE(*(cursor2++));
        if (--len2 == 0) {
            GFX_TIMSORT_MOVE_RANGE(cursor1, cursor1 + len1, dest);
            return;
        }
        if (len1 == 1) {
            GFX_TIMSORT_MOVE_RANGE(cursor2, cursor2 + len2, dest);
            *(dest + len2) = GFX_TIMSORT_MOVE(*cursor1);
            return;
        }

        int minGallop(state_.minGallop_);

        // outer:
        while (true) {
            int count1 = 0;
            int count2 = 0;

            bool break_outer = false;
            do {
                assert(len1 > 1 && len2 > 0);

                if (comp_.lt(*cursor2, *cursor1)) {
                    *(dest++) = GFX_TIMSORT_MOVE(*(cursor2++));
                    ++count2;
                    count1 = 0;
                    if (--len2 == 0) {
                        break_outer = true;
                        break;
                    }
                } else {
                    *(dest++) = GFX_TIMSORT_MOVE(*(cursor1++));
                    ++count1;
                    count2 = 0;
                    if (--len1 == 1) {
                        break_outer = true;
                        break;
                    }
                }
            } while ((count1 | count2) < minGallop);
            if (break_outer) {
                break;
            }

            do {
                assert(len1 > 1 && len2 > 0);

                count1 = gallopRight(*cursor2, cursor1, len1, 0);
                if (count1 != 0) {
                    GFX_TIMSORT_MOVE_BACKWARD(cursor1, cursor1 + count1, dest + count1);
                    dest += count1;
                    cursor1 += count1;
                    len1 -= count1;

                    if (len1 <= 1) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest++) = GFX_TIMSORT_MOVE(*(cursor2++));
                if (--len2 == 0) {
                    break_outer = true;
                    break;
                }

                count2 = gallopLeft(*cursor1, cursor2, len2, 0);
                if (count2 != 0) {
                    GFX_TIMSORT_MOVE_RANGE(cursor2, cursor2 + count2, dest);
                    dest += count2;
                    cursor2 += count2;
                    len2 -= count2;
                    if (len2 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest++) = GFX_TIMSORT_MOVE(*(cursor1++));
                if (--len1 == 1) {
                    break_outer = true;
                    break;
                }

                --minGallop;
            } while ((count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP));
            if (break_outer) {
                break;
            }

            if (minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;
        } // end of "outer" loop

        state_.minGallop_ = std::min(minGallop, 1);

        if (len1 == 1) {
            assert(len2 > 0);
            GFX_TIMSORT_MOVE_RANGE(cursor2, cursor2 + len2, dest);
            *(dest + len2) = GFX_TIMSORT_MOVE(*cursor1);
        } else {
            assert(len1 != 0 && "Comparison function violates its general contract");
            assert(len2 == 0);
            assert(len1 > 1);
            GFX_TIMSORT_MOVE_RANGE(cursor1, cursor1 + len1, dest);
        }
    }

    void mergeHi(iter_t const base1, diff_t len1, iter_t const base2, diff_t len2) {
        using namespace timsort_constants;
        assert(len1 > 0 && len2 > 0 && base1 + len1 == base2);

        state_.tmp_.move_in(base2, len2);

        iter_t cursor1 = base1 + (len1 - 1);
        tmp_iter_t cursor2 = state_.tmp_.begin() + (len2 - 1);
        iter_t dest = base2 + (len2 - 1);

        *(dest--) = GFX_TIMSORT_MOVE(*(cursor1--));
        if (--len1 == 0) {
            GFX_TIMSORT_MOVE_RANGE(state_.tmp_.begin(), state_.tmp_.begin() + len2, dest - (len2 - 1));
            return;
        }
        if (len2 == 1) {
            dest -= len1;
            cursor1 -= len1;
            GFX_TIMSORT_MOVE_BACKWARD(cursor1 + 1, cursor1 + (1 + len1), dest + (1 + len1));
            *dest = GFX_TIMSORT_MOVE(*cursor2);
            return;
        }

        int minGallop(state_.minGallop_);

        // outer:
        while (true) {
            int count1 = 0;
            int count2 = 0;

            bool break_outer = false;
            do {
                assert(len1 > 0 && len2 > 1);

                if (comp_.lt(*cursor2, *cursor1)) {
                    *(dest--) = GFX_TIMSORT_MOVE(*(cursor1--));
                    ++count1;
                    count2 = 0;
                    if (--len1 == 0) {
                        break_outer = true;
                        break;
                    }
                } else {
                    *(dest--) = GFX_TIMSORT_MOVE(*(cursor2--));
                    ++count2;
                    count1 = 0;
                    if (--len2 == 1) {
                        break_outer = true;
                        break;
                    }
                }
            } while ((count1 | count2) < minGallop);
            if (break_outer) {
                break;
            }

            do {
                assert(len1 > 0 && len2 > 1);

                count1 = len1 - gallopRight(*cursor2, base1, len1, len1 - 1);
                if (count1 != 0) {
                    dest -= count1;
                    cursor1 -= count1;
                    len1 -= count1;
                    GFX_TIMSORT_MOVE_BACKWARD(cursor1 + 1, cursor1 + (1 + count1), dest + (1 + count1));

                    if (len1 == 0) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest--) = GFX_TIMSORT_MOVE(*(cursor2--));
                if (--len2 == 1) {
                    break_outer = true;
                    break;
                }

                count2 = len2 - gallopLeft(*cursor1, state_.tmp_.begin(), len2, len2 - 1);
                if (count2 != 0) {
                    dest -= count2;
                    cursor2 -= count2;
                    len2 -= count2;
                    GFX_TIMSORT_MOVE_RANGE(cursor2 + 1, cursor2 + (1 + count2), dest + 1);
                    if (len2 <= 1) {
                        break_outer = true;
                        break;
                    }
                }
                *(dest--) = GFX_TIMSORT_MOVE(*(cursor1--));
                if (--len1 == 0) {
                    break_outer = true;
                    break;
                }

                minGallop--;
            } while ((count1 >= MIN_GALLOP) | (count2 >= MIN_GALLOP));
            if (break_outer) {
                break;
            }

            if (minGallop < 0) {
                minGallop = 0;
            }
            minGallop += 2;
        } // end of "outer" loop

        state_.minGallop_ = std::min(minGallop, 1);

        if (len2 == 1) {
            assert(len1 > 0);
            dest -= len1;
            GFX_TIMSORT_MOVE_BACKWARD(cursor1 + (1 - len1), cursor1 + 1, dest + (1 + len1));
            *dest = GFX_TIMSORT_MOVE(*cursor2);
        } else {
            assert(len2 != 0 && "Comparison function violates its general contract");
            assert(len1 == 0);
            assert(len2 > 1);
            GFX_TIMSORT_MOVE_RANGE(state_.tmp_.begin(), state_.tmp_.begin() + len2, dest - (len2 - 1));
        }
    }

    // the only interface is the friend timsort() function
    template <typename IterT, typename LessT> friend void timsort(IterT first, IterT last, LessT c);
};

template <typename RandomAccessIterator>
inline void timsort(RandomAccessIterator const first, RandomAccessIterator const last) {
    typedef typename std::iterator_traits<RandomAccessIterator>::value_type value_type;
    timsort(first, last, std::less<value_type>());
}

template <typename RandomAccessIterator, typename LessFunction>
inline void timsort(RandomAccessIterator const first, RandomAccessIterator const last, LessFunction compare) {
    TimSort<RandomAccessIterator, LessFunction>::sort(first, last, compare);
}

} // namespace gfx

#undef GFX_TIMSORT_LOG
#undef GFX_TIMSORT_MOVE
#undef GFX_TIMSORT_MOVE_RANGE
#undef GFX_TIMSORT_MOVE_BACKWARD
#endif // GFX_TIMSORT_HPP
