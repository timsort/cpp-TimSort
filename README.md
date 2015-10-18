TimSort [![Build Status](https://travis-ci.org/gfx/cpp-TimSort.svg?branch=master)](https://travis-ci.org/gfx/cpp-TimSort)
==================

A C++ implementation of TimSort, O(n log n) in worst case and stable sort algorithm, ported from Python's and OpenJDK's.

According to benchmark, this is a bit slower than `std::sort()` on randomized sequences, but much faster on partially-sorted sequences.

SYNOPSIS
==================

    #include "timsort.hpp"

    std::vector<string> a;

    // initialize a

    gfx::timsort(a.begin(), a.end(), std::less<string>());

TEST
==================

Run `make test` for testing and `make coverage` for test coverage.

COMPATIBILITY
==================

This library is compatible with C++98, but if you give compile it with C++11 or later, this library uses `std::move()` instead of value copy and thus you can sort move-only types (see [#9](https://github.com/gfx/cpp-TimSort/pull/9) for details).

You can disable use of `std::move()` by passing the macro '-DDISABLE_STD_MOVE'.

SEE ALSO
==================

* http://svn.python.org/projects/python/trunk/Objects/listsort.txt
* http://en.wikipedia.org/wiki/Timsort

BENCHMARK
==================

bench.cpp, invoked by `make bench`, is a simple benchmark.
An example output is as follows (timing scale: sec.):

    c++ -v
    Apple LLVM version 7.0.0 (clang-700.0.72)
    Target: x86_64-apple-darwin14.5.0
    Thread model: posix
    c++ -I. -Wall -Wextra -g  -DNDEBUG -O2 -std=c++11 example/bench.cpp -o .bin/bench
    ./.bin/bench
    RANDOMIZED SEQUENCE
    [int]
    size	100000
    std::sort        0.695253
    std::stable_sort 0.868916
    timsort          1.255825
    [std::string]
    size	100000
    std::sort        3.438217
    std::stable_sort 4.122629
    timsort          5.791845
    REVERSED SEQUENCE
    [int]
    size	100000
    std::sort        0.045461
    std::stable_sort 0.575431
    timsort          0.019139
    [std::string]
    size	100000
    std::sort        0.586707
    std::stable_sort 2.715778
    timsort          0.345099
    SORTED SEQUENCE
    [int]
    size	100000
    std::sort        0.021876
    std::stable_sort 0.087993
    timsort          0.008042
    [std::string]
    size	100000
    std::sort        0.402458
    std::stable_sort 2.436326
    timsort          0.298639
