TimSort
==================
A C++ port of Java timsort implementation

The original is http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/raw_files/new/src/share/classes/java/util/TimSort.java

BENCHMARK
==================
bench.cpp, invoked by `make bench`, is a simple benchmark.
An example output is (scale: sec.):
    int
    size	100000
    std::sort	0.39
    std::stable_sort	0.82
    timsort	0.12
    double
    size	100000
    std::sort	0.67
    std::stable_sort	2.13
    timsort	0.19
    boost::rational
    size	100000
    std::sort	13.66
    std::stable_sort	10.52
    timsort	1.01
