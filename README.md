TimSort [![Build Status](https://travis-ci.org/gfx/cpp-TimSort.svg?branch=master)](https://travis-ci.org/gfx/cpp-TimSort)
==================

A C++ implementation of TimSort, O(n log n) in worst case and stable sort algorithm, ported from Python's and OpenJDK's.

This is a bit slower than `std::sort()` on randomized sequences, and much
faster on partially-sorted sequences.

SYNOPSIS
==================

    #include "timsort.hpp"

    std::vector<string> a;

    // initialize a

    gfx::timsort(a.begin(), a.end(), std::less<string>());

TEST
==================

Run `make test` for testing and `make coverage` for test coverage.

SEE ALSO
==================

* http://svn.python.org/projects/python/trunk/Objects/listsort.txt
* http://en.wikipedia.org/wiki/Timsort

BENCHMARK
==================

bench.cpp, invoked by `make bench`, is a simple benchmark.
An example output is as follows (timing scale: sec.):

    c++ -I. -Wall -Wextra -g  -DNDEBUG -O2 -std=c++11 -DENABLE_STD_MOVE example/bench.cpp -o .bin/bench
    c++ -v
    Apple LLVM version 7.0.0 (clang-700.0.72)
    Target: x86_64-apple-darwin14.5.0
    Thread model: posix
    ./.bin/bench
    RANDOMIZED SEQUENCE
    [int]
    size	100000
    std::sort        0.531996
    std::stable_sort 0.645782
    timsort          1.012254
    [boost::rational]
    size	100000
    std::sort        3.466250
    std::stable_sort 5.943234
    timsort          4.456835
    REVERSED SEQUENCE
    [int]
    size	100000
    std::sort        0.023546
    std::stable_sort 0.399995
    timsort          0.014056
    [boost::rational]
    size	100000
    std::sort        0.626102
    std::stable_sort 7.463993
    timsort          0.218232
    SORTED SEQUENCE
    [int]
    size	100000
    std::sort        0.015051
    std::stable_sort 0.074084
    timsort          0.007797
    [boost::rational]
    size	100000
    std::sort        0.371826
    std::stable_sort 1.290227
    timsort          0.216113
