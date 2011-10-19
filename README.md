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
    std::sort        0.631852
    std::stable_sort 0.846656
    timsort          1.1865
    double
    size	100000
    std::sort        0.759048
    std::stable_sort 1.04307
    timsort          1.30662
    boost::rational
    size	100000
    std::sort        5.94573
    std::stable_sort 5.41838
    timsort          5.33095
