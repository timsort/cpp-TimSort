TimSort
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

Run `make test` to test this program on your environment.

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
    Apple clang version 4.1 (tags/Apple/clang-421.11.65) (based on LLVM 3.1svn)
    Target: x86_64-apple-darwin12.2.0
    Thread model: posix
    ./.bin/bench
    RANDOMIZED SEQUENCE
    [int]
    size	100000
    std::sort        0.659631
    std::stable_sort 0.882459
    timsort          1.23149
    [boost::rational]
    size	100000
    std::sort        4.11522
    std::stable_sort 5.16115
    timsort          5.63257
    REVERSED SEQUENCE
    [int]
    size	100000
    std::sort        0.088988
    std::stable_sort 0.238711
    timsort          0.018056
    [boost::rational]
    size	100000
    std::sort        2.19554
    std::stable_sort 2.28298
    timsort          0.292725
    SORTED SEQUENCE
    [int]
    size	100000
    std::sort        0.111402
    std::stable_sort 0.174865
    timsort          0.010545
    [boost::rational]
    size	100000
    std::sort        2.11468
    std::stable_sort 2.44897
    timsort          0.292203
