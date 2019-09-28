## TimSort [![Build Status](https://travis-ci.org/gfx/cpp-TimSort.svg?branch=master)](https://travis-ci.org/gfx/cpp-TimSort)

A C++ implementation of TimSort, an O(n log n) stable sorting algorithm, ported from Python's and OpenJDK's.

See also the following links for a detailed description of TimSort:
* http://svn.python.org/projects/python/trunk/Objects/listsort.txt
* http://en.wikipedia.org/wiki/Timsort

According to the benchmarks, it is a bit slower than `std::sort()` on randomized sequences, but much faster on
partially-sorted ones. `gfx::timsort` should be usable as a drop-in replacement for `std::stable_sort`, with the
difference that it's can't fallback to a O(n log² n) algorithm when there isn't enough extra heap memory available.

## EXAMPLE

```cpp
#include <string>
#include <vector>
#include <gfx/timsort.hpp>

std::vector<std::string> a;
// ... initialize a ...
gfx::timsort(a.begin(), a.end(), std::less<string>());
```

## COMPATIBILITY

This library is compatible with C++98, but if you compile it with C++11 or later it will try to use `std::move()`
when possible instead of copying vaues around, which notably allows to sort collections of move-only types (see
[#9](https://github.com/gfx/cpp-TimSort/pull/9) for details).

You can explicity control the use of `std::move()` by setting the macro `GFX_TIMSORT_USE_STD_MOVE` to `0` or `1`.

The library has been tested with the following compilers:
* GCC 5
* Clang 3.8
* The Clang version that ships with Xcode 8.3
* MSVC 2017 update 9

It should also work with more recent compilers, and most likely with some older compilers too.

The library can be installed on the system via CMake with the following commands:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
cd build
make install
```

## TESTS

The tests are written with Catch2 (branch 1.x) and can be compiled with CMake and run through CTest.

When using the project's main `CMakeLists.txt`, the CMake variable `BUILD_TESTING` is `ON` by default unless the
project is included as a subdirectory. The following CMake variables are available to change the way the tests are
built with CMake:
* `GFX_TIMSORT_USE_VALGRIND`: if `ON`, the tests will be run through Valgrind (`OFF` by default)
* `GFX_TIMSORT_SANITIZE`: this variable takes a comma-separated list of sanitizers options to run the tests (empty by default)

## BENCHMARKS

Benchmarks are available in the `benchmarks` subdirectory, and can be constructed directly by passing `BUILD_BENCHMARKS=ON`
variable to CMake during the configuration step.

Example output (timing scale: sec.):

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
