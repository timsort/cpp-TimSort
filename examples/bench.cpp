#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <boost/rational.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/timer.hpp>

#include "timsort.hpp"

using namespace gfx;

enum state_t { sorted, randomized, reversed };

template <typename value_t> static void bench(int const size, state_t const state) {
    std::cerr << "size\t" << size << std::endl;

    std::vector<value_t> a;
    for (int i = 0; i < size; ++i) {
        a.push_back(boost::lexical_cast<value_t>((i + 1) * 10));
    }

    switch (state) {
    case randomized:
        std::random_shuffle(a.begin(), a.end());
        break;
    case reversed:
        std::stable_sort(a.begin(), a.end());
        std::reverse(a.begin(), a.end());
        break;
    case sorted:
        std::stable_sort(a.begin(), a.end());
        break;
    default:
        assert(!"not reached");
    }

    {
        std::vector<value_t> b(a);
        boost::timer t;

        for (int i = 0; i < 100; ++i) {
            std::copy(a.begin(), a.end(), b.begin());
            std::sort(b.begin(), b.end());
        }

        std::cerr << "std::sort        " << t.elapsed() << std::endl;
    }

    {
        std::vector<value_t> b(a);
        boost::timer t;

        for (int i = 0; i < 100; ++i) {
            std::copy(a.begin(), a.end(), b.begin());
            std::stable_sort(b.begin(), b.end());
        }

        std::cerr << "std::stable_sort " << t.elapsed() << std::endl;
    }

    {
        std::vector<value_t> b(a);
        boost::timer t;

        for (int i = 0; i < 100; ++i) {
            std::copy(a.begin(), a.end(), b.begin());
            timsort(b.begin(), b.end());
        }

        std::cerr << "timsort          " << t.elapsed() << std::endl;
    }
}

static void doit(int const n, state_t const state) {
    std::cerr << "[int]" << std::endl;
    bench<int>(n, state);

    std::cerr << "[std::string]" << std::endl;
    bench<std::string>(n, state);
}

int main(int argc, const char *argv[]) {
    const int N = argc > 1 ? boost::lexical_cast<int>(argv[1]) : 100 * 1000;

    std::cerr << std::setprecision(6) << std::setiosflags(std::ios::fixed);

    std::srand(0);

    std::cerr << "RANDOMIZED SEQUENCE" << std::endl;
    doit(N, randomized);

    std::cerr << "REVERSED SEQUENCE" << std::endl;
    doit(N, reversed);

    std::cerr << "SORTED SEQUENCE" << std::endl;
    doit(N, sorted);
}
