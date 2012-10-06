#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <boost/rational.hpp>

#include <boost/timer.hpp>

#include "timsort.hpp"

enum state_t {
    sorted, randomized, reversed
};

template <typename value_t>
static void bench(int const size, state_t const state) {
    std::cerr << "size\t" << size << std::endl;

    std::less<value_t> lt;

    std::vector<value_t> a;
    for(int i = 0; i < size; ++i) {
        a.push_back((i+1) * 10);
    }

    switch(state) {
    case randomized:
        std::random_shuffle(a.begin(), a.end());
        break;
    case reversed:
        std::stable_sort(a.begin(), a.end(), lt);
        std::reverse(a.begin(), a.end());
        break;
    case sorted:
        std::stable_sort(a.begin(), a.end(), lt);
        break;
    default:
        assert(!"not reached");
    }

    {
        std::vector<value_t> b(a);
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::copy(a.begin(), a.end(), b.begin());
            std::sort(b.begin(), b.end(), lt);
        }

        std::cerr << "std::sort        " << t.elapsed() << std::endl;
    }


    {
        std::vector<value_t> b(a);
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::copy(a.begin(), a.end(), b.begin());
            std::stable_sort(b.begin(), b.end(), lt);
        }

        std::cerr << "std::stable_sort " << t.elapsed() << std::endl;
    }


    {
        std::vector<value_t> b(a);
        boost::timer t;

        for(int i = 0; i < 100; ++i) {
            std::copy(a.begin(), a.end(), b.begin());
            timsort(b.begin(), b.end(), lt);
        }

        std::cerr << "timsort          " << t.elapsed() << std::endl;
    }
}

static void doit(int const n, state_t const state) {
    std::cerr << "[int]" << std::endl;
    bench<int>(n, state);

    std::cerr << "[double]" << std::endl;
    bench<double>(n, state);

    std::cerr << "[boost::rational]" << std::endl;
    bench< boost::rational<long long> >(n, state);
}

int main() {
    const int N =  100 * 1000;

    std::srand(std::time(NULL));

    std::cerr << "RANDOMIZED SEQUENCE" << std::endl;
    doit(N, randomized);

    std::cerr << "REVERSED SEQUENCE" << std::endl;
    doit(N, reversed);

    std::cerr << "SORTED SEQUENCE" << std::endl;
    doit(N, sorted);
}
