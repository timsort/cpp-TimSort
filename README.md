TimSort
==================
A C++ implementation of TimSort, O(n log n) in worst case and stable sort algorithm

* http://en.wikipedia.org/wiki/Timsort

SYNOPSIS
==================

    #include "timsort.hpp"

    std::vector<string> a;

    // initialize a

    gfx::timsort(a.begin(), a.end(), std::less<string>());

BENCHMARK
==================
bench.cpp, invoked by `make bench`, is a simple benchmark.
An example output is as follows (timing scale: sec.):

    c++ -v
    Apple clang version 4.1 (tags/Apple/clang-421.11.65) (based on LLVM 3.1svn)
    Target: x86_64-apple-darwin12.2.0
    Thread model: posix
    ./.bin/bench
    RANDOMIZED SEQUENCE
    [int]
    size	100000
    std::sort        0.648058
    std::stable_sort 0.871686
    timsort          1.25026
    [double]
    size	100000
    std::sort        0.863128
    std::stable_sort 0.999469
    timsort          1.46419
    [boost::rational]
    size	100000
    std::sort        4.33464
    std::stable_sort 5.36463
    timsort          5.81233
    REVERSED SEQUENCE
    [int]
    size	100000
    std::sort        0.087545
    std::stable_sort 0.229006
    timsort          0.016407
    [double]
    size	100000
    std::sort        0.133877
    std::stable_sort 0.276708
    timsort          0.034984
    [boost::rational]
    size	100000
    std::sort        2.20984
    std::stable_sort 2.47303
    timsort          0.321253
    SORTED SEQUENCE
    [int]
    size	100000
    std::sort        0.088589
    std::stable_sort 0.155547
    timsort          0.010833
    [double]
    size	100000
    std::sort        0.142421
    std::stable_sort 0.226581
    timsort          0.016947
    [boost::rational]
    size	100000
    std::sort        2.25787
    std::stable_sort 2.55729
    timsort          0.292502
