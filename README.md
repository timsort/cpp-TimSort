TimSort
==================
A C++ port of Java timsort implementation

The original is http://cr.openjdk.java.net/~martin/webrevs/openjdk7/timsort/raw_files/new/src/share/classes/java/util/TimSort.java

BENCHMARK
==================
bench.cpp, invoked by `make bench`, is a simple benchmark.
An example output is (scale: sec.):

    RANDOMIZED SEQUENCE
    int
    size	100000
    std::sort        0.64711
    std::stable_sort 0.858373
    timsort          1.19961
    double
    size	100000
    std::sort        0.764102
    std::stable_sort 1.01592
    timsort          1.35223
    boost::rational
    size	100000
    std::sort        5.76009
    std::stable_sort 5.35419
    timsort          5.52264
    REVERSED SEQUENCE
    int
    size	100000
    std::sort        0.129067
    std::stable_sort 0.290258
    timsort          0.029547
    double
    size	100000
    std::sort        0.145228
    std::stable_sort 0.281365
    timsort          0.023615
    boost::rational
    size	100000
    std::sort        3.66021
    std::stable_sort 2.36177
    timsort          0.280779
    SORTED SEQUENCE
    int
    size	100000
    std::sort        0.113937
    std::stable_sort 0.183452
    timsort          0.012675
    double
    size	100000
    std::sort        0.121672
    std::stable_sort 0.242327
    timsort          0.020005
    boost::rational
    size	100000
    std::sort        3.87167
    std::stable_sort 2.78654
    timsort          0.302464
