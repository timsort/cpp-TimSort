[![Latest Release](https://img.shields.io/badge/release-cpp--TimSort%2F2.0.0-blue.svg)](https://github.com/timsort/cpp-TimSort/releases)
[![Conan Package](https://img.shields.io/badge/conan-2.0.0-blue.svg)](https://bintray.com/conan/conan-center/timsort%3A_)
[![Build Status](https://travis-ci.org/timsort/cpp-TimSort.svg?branch=master)](https://travis-ci.org/timsort/cpp-TimSort)
[![License](https://img.shields.io/:license-mit-yellow.svg)](https://doge.mit-license.org)

## TimSort

A C++ implementation of TimSort, an O(n log n) stable sorting algorithm, ported from Python's and OpenJDK's.

See also the following links for a detailed description of TimSort:
* http://svn.python.org/projects/python/trunk/Objects/listsort.txt
* http://en.wikipedia.org/wiki/Timsort

This library is compatible with C++11. If you need a C++98 version, you can check the 1.x.y branch of this repository.

According to the benchmarks, it is slower than `std::sort()` on randomized sequences, but faster on partially-sorted
ones. `gfx::timsort` should be usable as a drop-in replacement for `std::stable_sort`, with the difference that it
can't fallback to a O(n log² n) algorithm when there isn't enough extra heap memory available.

`gfx::timsort` also has a few additional features and guarantees compared to `std::stable_sort`:
* It can take a [projection function](https://ezoeryou.github.io/blog/article/2019-01-22-ranges-projection.html)
  after the comparison function. The support is a bit rougher than in the linked article or the C++20 standard library:
  unless `std::invoke` is available, only instances of types callable with parentheses can be used, there is no support
  for pointer to members.
* It can also be passed a range instead of a pair of iterators, in which case it will sort the whole range.
* This implementation of timsort notably avoids using the postfix `++` or `--` operators: only their prefix equivalents
  are used, which means that timsort will work even if the postfix operators are not present or return an incompatible
  type such as `void`.


The full list of available signatures is as follows (in namespace `gfx`):

```cpp
// Overloads taking a pair of iterators

template <typename RandomAccessIterator>
void timsort(RandomAccessIterator const first, RandomAccessIterator const last);

template <typename RandomAccessIterator, typename Compare>
void timsort(RandomAccessIterator const first, RandomAccessIterator const last,
             Compare compare);

template <typename RandomAccessIterator, typename Compare, typename Projection>
void timsort(RandomAccessIterator const first, RandomAccessIterator const last,
             Compare compare, Projection projection);

// Overloads taking a range

template <typename RandomAccessRange>
void timsort(RandomAccessRange &range);

template <typename RandomAccessRange, typename Compare>
void timsort(RandomAccessRange &range, Compare compare);

template <typename RandomAccessRange, typename Compare, typename Projection>
void timsort(RandomAccessRange &range, Compare compare, Projection projection);
```

## EXAMPLE

Example of using timsort with a comparison function and a projection function to sort a vector of strings by length:

```cpp
#include <string>
#include <vector>
#include <gfx/timsort.hpp>

size_t len(const std::string& str) {
    return str.size();
}

// Sort a vector of strings by length
std::vector<std::string> collection = { /* ... */ };
gfx::timsort(collection, std::less<std::string>{}, &len);
```

## INSTALLATION & COMPATIBILITY

The library has been tested with the following compilers:
* GCC 5
* Clang 3.8
* The AppleClang version that ships with Xcode 8.3
* MSVC 2017 update 9

It should also work with more recent compilers, and most likely with some older compilers too.

The library can be installed on the system via CMake with the following commands:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
cd build
make install
```

Alternatively the library is also available on conan-center-index and can be installed in your local Conan cache via
the following command:

```sh
conan install timsort/2.0.0
```

## DIAGNOSTICS & INFORMATION

A few configuration macros allow gfx::timsort to emit diagnostic, which might be helpful to diagnose issues:
* Defining `GFX_TIMSORT_ENABLE_ASSERT` inserts assertions in key locations in the algorithm to avoid logic errors.
* Defining `GFX_TIMSORT_ENABLE_LOG` inserts logs in key locations, which allow to follow more closely the flow of the algorithm.

**cpp-TimSort** follows semantic versioning and provides the following macros to retrieve the current major, minor
and patch versions:

```cpp
GFX_TIMSORT_VERSION_MAJOR
GFX_TIMSORT_VERSION_MINOR
GFX_TIMSORT_VERSION_PATCH
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
