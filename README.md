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

    c++ -I. -Wall -Wextra -g  -DNDEBUG -O2 example/bench.cpp -o .bin/bench
    c++ -v
    Apple clang version 4.1 (tags/Apple/clang-421.11.66) (based on LLVM 3.1svn)
    Target: x86_64-apple-darwin12.2.0
    Thread model: posix
    ./.bin/bench
    RANDOMIZED SEQUENCE
    [int]
    size	100000
    std::sort        0.667322
    std::stable_sort 0.895223
    timsort          1.274456
    [boost::rational]
    size	100000
    std::sort        4.152952
    std::stable_sort 5.136133
    timsort          5.781330
    REVERSED SEQUENCE
    [int]
    size	100000
    std::sort        0.087842
    std::stable_sort 0.234953
    timsort          0.017438
    [boost::rational]
    size	100000
    std::sort        2.114911
    std::stable_sort 2.247124
    timsort          0.281315
    SORTED SEQUENCE
    [int]
    size	100000
    std::sort        0.086000
    std::stable_sort 0.151913
    timsort          0.010536
    [boost::rational]
    size	100000
    std::sort        2.102378
    std::stable_sort 2.408591
    timsort          0.258270
