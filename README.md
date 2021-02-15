[![Latest Release](https://img.shields.io/badge/release-2.1.0-blue.svg)](https://github.com/timsort/cpp-TimSort/releases/tag/v2.1.0)
[![Conan Package](https://img.shields.io/badge/conan-cpp--TimSort%2F2.1.0-blue.svg)](https://conan.io/center/timsort?version=2.1.0)
[![Pitchfork Layout](https://img.shields.io/badge/standard-PFL-orange.svg)](https://github.com/vector-of-bool/pitchfork)

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


Merging sorted ranges efficiently is an important part of the TimSort algorithm. This library exposes its merge
algorithm in the public API. According to the benchmarks, `gfx::timmerge` is slower than `std::inplace_merge` on
heavily/randomly overlapping subranges of simple elements, but it is faster for complex elements such as `std::string`
and on sparsely overlapping subranges. `gfx::timmerge` should be usable as a drop-in replacement for
`std::inplace_merge`, with the difference that it can't fallback to a O(n log n) algorithm when there isn't enough
extra heap memory available. Like `gfx::timsort`, `gfx::timmerge` can take a projection function and avoids using the
postfix `++` or `--` operators.


The full list of available signatures is as follows (in namespace `gfx`):

```cpp
// timsort overloads taking a pair of iterators

template <typename RandomAccessIterator>
void timsort(RandomAccessIterator const first, RandomAccessIterator const last);

template <typename RandomAccessIterator, typename Compare>
void timsort(RandomAccessIterator const first, RandomAccessIterator const last,
             Compare compare);

template <typename RandomAccessIterator, typename Compare, typename Projection>
void timsort(RandomAccessIterator const first, RandomAccessIterator const last,
             Compare compare, Projection projection);

// timsort overloads taking a range

template <typename RandomAccessRange>
void timsort(RandomAccessRange &range);

template <typename RandomAccessRange, typename Compare>
void timsort(RandomAccessRange &range, Compare compare);

template <typename RandomAccessRange, typename Compare, typename Projection>
void timsort(RandomAccessRange &range, Compare compare, Projection projection);

// timmerge overloads

template <typename RandomAccessIterator>
void timmerge(RandomAccessIterator first, RandomAccessIterator middle,
              RandomAccessIterator last);

template <typename RandomAccessIterator, typename Compare>
void timmerge(RandomAccessIterator first, RandomAccessIterator middle,
              RandomAccessIterator last, Compare compare);

template <typename RandomAccessIterator, typename Compare, typename Projection>
void timmerge(RandomAccessIterator first, RandomAccessIterator middle,
              RandomAccessIterator last, Compare compare, Projection projection);
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

![Ubuntu builds status](https://github.com/timsort/cpp-TimSort/workflows/Ubuntu%20Builds/badge.svg?branch=master)
![Windows builds status](https://github.com/timsort/cpp-TimSort/workflows/Windows%20Builds/badge.svg?branch=master)
![MacOS builds status](https://github.com/timsort/cpp-TimSort/workflows/MacOS%20Builds/badge.svg?branch=master)

The library has been tested with the following compilers:
* GCC 5.5
* Clang 6
* MSVC 2017

It should also work with more recent compilers, and most likely with some older compilers too. We used to guarantee support as far back as Clang 3.8, but the new continuous integration environment doesn't go that far.

The library can be installed on the system via CMake with the following commands:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
cd build
make install
```

Alternatively the library is also available on conan-center-index and can be installed in your local Conan cache via
the following command:

```sh
conan install timsort/2.1.0
```

## DIAGNOSTICS & INFORMATION

A few configuration macros allow `gfx::timsort` and `gfx::timmerge` to emit diagnostic, which might be helpful to diagnose issues:
* Defining `GFX_TIMSORT_ENABLE_ASSERT` inserts assertions in key locations in the algorithm to avoid logic errors.
* Defining `GFX_TIMSORT_ENABLE_AUDIT` inserts assertions that verify pre- and postconditions. These verifications can slow the
  algorithm down significantly. Enable the audit only while testing or debugging.
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

Example bench_sort output (timing scale: sec.):

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

Example bench_merge output (timing scale: milliseconds; omitted detailed results for different
middle iterator positions, reformatted to improve readability):

    c++ -v
    Using built-in specs.
    ...
    Target: x86_64-pc-linux-gnu
    ...
    gcc version 10.2.0 (GCC)
    c++ -I ../include -Wall -Wextra -g -DNDEBUG -O2 -std=c++11 bench_merge.cpp -o bench_merge
    ./bench_merge
    size	100000
    element type\algorithm:      	std::inplace_merge	timmerge
    RANDOMIZED SEQUENCE
    [int] approx. average        	 33.404430        	 37.047990
    [std::string] approx. average	324.964249        	210.297207
    REVERSED SEQUENCE
    [int] approx. average        	 11.441404        	  4.017482
    [std::string] approx. average	305.649503        	114.773898
    SORTED SEQUENCE
    [int] approx. average        	  4.291098        	  0.105571
    [std::string] approx. average	158.238114        	  0.273858

Detailed bench_merge results for different middle iterator positions can be found at
https://github.com/timsort/cpp-TimSort/wiki/Benchmark-results
