/*
 * Copyright (c) 2024 Morwenn.
 *
 * SPDX-License-Identifier: MIT
 */
#include <version>

#ifdef _LIBCPP_VERSION
#if defined(_LIBCPP_ENABLE_ASSERTIONS) && _LIBCPP_ENABLE_ASSERTIONS

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace std::inline __1
{
    /*
     * Required to avoid linking issues with AppleClang when
     * compiling with _LIBCPP_ENABLE_ASSERTIONS.
     * See https://releases.llvm.org/16.0.0/projects/libcxx/docs/UsingLibcxx.html#enabling-the-safe-libc-mode
     */
    [[noreturn]]
    void __libcpp_verbose_abort(char const* format, ...) {
        std::va_list list;
        va_start(list, format);
        std::vfprintf(stderr, format, list);
        va_end(list);

        std::abort();
    }
    }

#endif
#endif
