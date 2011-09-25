TimSort
==================
A C++ port of Java timsort implementation

The original is http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/raw_files/new/src/share/classes/java/util/TimSort.java

BENCHMARK
==================
bench.cpp, invoked by `make bench`, is a simple benchmark.
An example output is (scale: sec.):

    size	1024
    std::sort	0
    std::stable_sort	0
    timsort	0
    size	2048
    std::sort	0.01
    std::stable_sort	0.02
    timsort	0
    size	4096
    std::sort	0.01
    std::stable_sort	0.02
    timsort	0.01
    size	8192
    std::sort	0.02
    std::stable_sort	0.05
    timsort	0
    size	16384
    std::sort	0.05
    std::stable_sort	0.09
    timsort	0.02
    size	32768
    std::sort	0.09
    std::stable_sort	0.26
    timsort	0.04
    size	65536
    std::sort	0.21
    std::stable_sort	0.63
    timsort	0.08
    size	131072
    std::sort	0.54
    std::stable_sort	1.48
    timsort	0.17
    size	262144
    std::sort	1.08
    std::stable_sort	4.36
    timsort	0.31
    size	524288
    std::sort	2.41
    std::stable_sort	8.44
    timsort	0.68
    size	1048576
    std::sort	4.92
    std::stable_sort	14.2
    timsort	1.3
