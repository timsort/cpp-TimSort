[![Latest Release](https://img.shields.io/badge/release-3.0.0-blue.svg)](https://github.com/timsort/cpp-TimSort/releases/tag/v3.0.0)
[![Conan Package](https://img.shields.io/badge/conan-cpp--TimSort%2F3.0.0-blue.svg)](https://conan.io/center/timsort?version=3.0.0)
[![Pitchfork Layout](https://img.shields.io/badge/standard-PFL-orange.svg)](https://github.com/vector-of-bool/pitchfork)

## TimSort

A C++ implementation of TimSort, an O(n log n) stable sorting algorithm, ported from Python's and OpenJDK's.

See also the following links for a detailed description of TimSort:
* http://svn.python.org/projects/python/trunk/Objects/listsort.txt
* http://en.wikipedia.org/wiki/Timsort

This version of the library requires at least C++20. Older versions of the library, available in different branches,
offer support for older standards though implement fewer features:
* Branch `2.x.y` is compatible with C++11, and is slightly more permissive in some reagard due to the lack of
  concepts to constrain its interface (it notaby supports iterators without postfix operator++/--).
* Branch `1.x.y` is compatible with C++03.
Older versions are not actively maintained anymore. If you need extended support for those, please open specific
issues for the problems you want solved.

According to the benchmarks, `gfx::timsort` is slower than [`std::ranges::sort`][std-sort] on randomized sequences,
but faster on partially-sorted ones. It can be used as a drop-in replacement for [`std::ranges::stable_sort`][std-stable-sort],
with the difference that it can't fall back to a O(n log² n) algorithm when there isn't enough extra heap memory
available.

Merging sorted ranges efficiently is an important part of the TimSort algorithm. This library exposes `gfx::timmerge`
in the public API, a drop-in replacement for [`std::ranges::inplace_merge`][std-inplace-merge] with the difference
that it can't fall back to a O(n log n) algorithm when there isn't enough extra heap memory available. According to
the benchmarks, `gfx::timmerge` is slower than `std::ranges::inplace_merge` on heavily/randomly overlapping subranges
of simple elements, but it is faster for complex elements such as `std::string`, and on sparsely overlapping subranges.

The ibrary exposes the following functions in namespace `gfx`:

```cpp
// timsort

template <
    std::random_access_iterator Iterator,
    std::sentinel_for<Iterator> Sentinel,
    typename Compare = std::ranges::less,
    typename Projection = std::identity
>
    requires std::sortable<Iterator, Compare, Projection>
auto timsort(Iterator first, Sentinel last,
             Compare compare={}, Projection projection={})
    -> Iterator;

template <
    std::ranges::random_access_range Range,
    typename Compare = std::ranges::less,
    typename Projection = std::identity
>
    requires std::sortable<std::ranges::iterator_t<Range>, Compare, Projection>
auto timsort(Range &range, Compare compare={}, Projection projection={})
    -> std::ranges::borrowed_iterator_t<Range>;

// timmerge

template <
    std::random_access_iterator Iterator,
    std::sentinel_for<Iterator> Sentinel,
    typename Compare = std::ranges::less,
    typename Projection = std::identity
>
    requires std::sortable<Iterator, Compare, Projection>
auto timmerge(Iterator first, Iterator middle, Sentinel last,
              Compare compare={}, Projection projection={})
    -> Iterator;

template <
    std::ranges::random_access_range Range,
    typename Compare = std::ranges::less,
    typename Projection = std::identity
>
    requires std::sortable<std::ranges::iterator_t<Range>, Compare, Projection>
auto timmerge(Range &&range, std::ranges::iterator_t<Range> middle,
              Compare compare={}, Projection projection={})
    -> std::ranges::borrowed_iterator_t<Range>;
```

## EXAMPLE

Example of using timsort with a defaulted comparison function and a projection function to sort a vector of strings
by length:

```cpp
#include <string>
#include <vector>
#include <gfx/timsort.hpp>

size_t len(const std::string& str) {
    return str.size();
}

// Sort a vector of strings by length
std::vector<std::string> collection = { /* ... */ };
gfx::timsort(collection, {}, &len);
```

## INSTALLATION & COMPATIBILITY

[![Ubuntu Builds](https://github.com/timsort/cpp-TimSort/actions/workflows/build-ubuntu.yml/badge.svg?branch=3.x.y)](https://github.com/timsort/cpp-TimSort/actions/workflows/build-ubuntu.yml)
[![MSVC Builds](https://github.com/timsort/cpp-TimSort/actions/workflows/build-msvc.yml/badge.svg?branch=3.x.y)](https://github.com/timsort/cpp-TimSort/actions/workflows/build-msvc.yml)
[![MinGW-w64 Builds](https://github.com/timsort/cpp-TimSort/actions/workflows/build-mingw.yml/badge.svg?branch=3.x.y)](https://github.com/timsort/cpp-TimSort/actions/workflows/build-mingw.yml)
[![MacOS Builds](https://github.com/timsort/cpp-TimSort/actions/workflows/build-macos.yml/badge.svg?branch=3.x.y)](https://github.com/timsort/cpp-TimSort/actions/workflows/build-macos.yml)

The library is tested with the following compilers:
* Ubuntu: GCC 10, Clang 11
* Windows: MSVC 19.37.32826.1, MinGW-w64 GCC 12
* MacOS: GCC 10, Clang 17

The library can be installed on the system via [CMake][cmake] (at least 3.14) with the following commands:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --install build
```

Alternatively the library is also available [Conan Center][conan-center] and can be directly installed in your local
[Conan][conan] cache with the following command:

```sh
conan install --requires=timsort/3.0.0
```

## DIAGNOSTICS & INFORMATION

The following configuration macros allow `gfx::timsort` and `gfx::timmerge` to emit diagnostics, which can be helpful
to diagnose issues:
* Defining `GFX_TIMSORT_ENABLE_ASSERT` inserts assertions in key locations in the algorithm to avoid logic errors.
* Defining `GFX_TIMSORT_ENABLE_AUDIT` inserts assertions that verify pre- and postconditions. These verifications can
  slow the algorithm down significantly. Enable the audits only while testing or debugging.
* Defining `GFX_TIMSORT_ENABLE_LOG` inserts logs in key locations, which allow to follow more closely the flow of the
  algorithm.

**cpp-TimSort** follows semantic versioning and provides the following macros to retrieve the current major, minor
and patch versions:

```cpp
GFX_TIMSORT_VERSION_MAJOR
GFX_TIMSORT_VERSION_MINOR
GFX_TIMSORT_VERSION_PATCH
```

## TESTS

The tests are written with Catch2 and can be compiled with CMake and run through CTest.

When using the project's main `CMakeLists.txt`, the CMake option `BUILD_TESTING` is `ON` by default unless the
project is included as a subdirectory. The following CMake options are available to change the way the tests are
built with CMake:
* `GFX_TIMSORT_USE_VALGRIND`: if `ON`, the tests will be run through Valgrind (`OFF` by default)
* `GFX_TIMSORT_SANITIZE`: this variable takes a comma-separated list of sanitizers options to run the tests (empty by default)

## BENCHMARKS

Benchmarks are available in the `benchmarks` subdirectory, and can be constructed directly by passing the option
`-DBUILD_BENCHMARKS=ON` to CMake during the configuration step.

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


  [cmake]: https://cmake.org/
  [conan]: https://conan.io/
  [conan-center]: https://conan.io/center
  [std-inplace-merge]: https://en.cppreference.com/w/cpp/algorithm/ranges/inplace_merge
  [std-sort]: https://en.cppreference.com/w/cpp/algorithm/ranges/sort
  [std-stable-sort]: https://en.cppreference.com/w/cpp/algorithm/ranges/stable_sort
