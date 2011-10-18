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
    std::sort        0.12273
    std::stable_sort 0.182555
    timsort          0.021382
    double
    size	100000
    std::sort        0.121308
    std::stable_sort 0.221106
    timsort          0.026283
    boost::rational
    size	100000
    std::sort        3.60624
    std::stable_sort 2.60706
    timsort          0.319079

Looks great.
